#include "json_utils.h"
#include "../cJSON.h"
#include <stdio.h>
#include <stdlib.h>

Graph *createGraphFromJSON(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        fprintf(stderr, "Erreur d'ouverture du fichier JSON\n");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *buffer = (char *)malloc(size + 1);
    fread(buffer, 1, size, file);
    fclose(file);
    buffer[size] = '\0';

    cJSON *json = cJSON_Parse(buffer);
    if (!json)
    {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
        {
            fprintf(stderr, "Erreur JSON avant: %s\n", error_ptr);
        }
        free(buffer);
        return NULL;
    }

    cJSON *nodes = cJSON_GetObjectItemCaseSensitive(json, "nodes");
    int node_count = cJSON_GetArraySize(nodes);
    Graph *graph = createGraph(node_count);

    for (int i = 0; i < node_count; i++)
    {
        cJSON *node = cJSON_GetArrayItem(nodes, i);
        cJSON *id = cJSON_GetObjectItemCaseSensitive(node, "id");
        cJSON *name = cJSON_GetObjectItemCaseSensitive(node, "name");
        cJSON *type = cJSON_GetObjectItemCaseSensitive(node, "type");
        cJSON *coordinates = cJSON_GetObjectItemCaseSensitive(node, "coordinates");
        cJSON *capacity = cJSON_GetObjectItemCaseSensitive(node, "capacity");

        // Nouveaux champs
        cJSON *earliest = cJSON_GetObjectItemCaseSensitive(node, "earliestTime");
        cJSON *latest = cJSON_GetObjectItemCaseSensitive(node, "latestTime");
        cJSON *service = cJSON_GetObjectItemCaseSensitive(node, "serviceTime");

        float lat = cJSON_GetArrayItem(coordinates, 0)->valuedouble;
        float lon = cJSON_GetArrayItem(coordinates, 1)->valuedouble;

        // Ajout du noeud avec les champs temporels
        addNode(graph, i, id->valueint, name->valuestring, type->valuestring,
                lat, lon, capacity->valueint,
                earliest->valuedouble, latest->valuedouble, service->valuedouble);
    }

    cJSON *edges = cJSON_GetObjectItemCaseSensitive(json, "edges");
    int edge_count = cJSON_GetArraySize(edges);

    for (int i = 0; i < edge_count; i++)
    {
        cJSON *edge = cJSON_GetArrayItem(edges, i);
        cJSON *source = cJSON_GetObjectItemCaseSensitive(edge, "source");
        cJSON *destination = cJSON_GetObjectItemCaseSensitive(edge, "destination");

        EdgeAttr attr;
        attr.distance = cJSON_GetObjectItemCaseSensitive(edge, "distance")->valuedouble;
        attr.baseTime = cJSON_GetObjectItemCaseSensitive(edge, "baseTime")->valuedouble;
        attr.cost = cJSON_GetObjectItemCaseSensitive(edge, "cost")->valuedouble;
        attr.roadType = cJSON_GetObjectItemCaseSensitive(edge, "roadType")->valueint;
        attr.reliability = cJSON_GetObjectItemCaseSensitive(edge, "reliability")->valuedouble;
        attr.restrictions = cJSON_GetObjectItemCaseSensitive(edge, "restrictions")->valueint;

        addEdge(graph, source->valueint, destination->valueint, attr);
    }

    cJSON_Delete(json);
    free(buffer);
    return graph;
}

void saveNetworkState(Graph *graph, const char *filename)
{
    cJSON *root = cJSON_CreateObject();
    cJSON *nodes_array = cJSON_AddArrayToObject(root, "nodes");

    for (int i = 0; i < graph->V; i++)
    {
        cJSON *node = cJSON_CreateObject();
        cJSON_AddNumberToObject(node, "id", graph->nodes[i].id);
        cJSON_AddStringToObject(node, "name", graph->nodes[i].name);
        cJSON_AddStringToObject(node, "type", graph->nodes[i].type);

        cJSON *coords = cJSON_AddArrayToObject(node, "coordinates");
        cJSON_AddItemToArray(coords, cJSON_CreateNumber(graph->nodes[i].coordinates[0]));
        cJSON_AddItemToArray(coords, cJSON_CreateNumber(graph->nodes[i].coordinates[1]));

        cJSON_AddNumberToObject(node, "capacity", graph->nodes[i].capacity);

        // Sauvegarde des nouveaux champs
        cJSON_AddNumberToObject(node, "earliestTime", graph->nodes[i].earliestTime);
        cJSON_AddNumberToObject(node, "latestTime", graph->nodes[i].latestTime);
        cJSON_AddNumberToObject(node, "serviceTime", graph->nodes[i].serviceTime);

        cJSON_AddItemToArray(nodes_array, node);
    }

    cJSON *edges_array = cJSON_AddArrayToObject(root, "edges");
    for (int i = 0; i < graph->V; i++)
    {
        AdjListNode *temp = graph->array[i].head;
        while (temp)
        {
            if (i < temp->dest)
            {
                cJSON *edge = cJSON_CreateObject();
                cJSON_AddNumberToObject(edge, "source", i);
                cJSON_AddNumberToObject(edge, "destination", temp->dest);
                cJSON_AddNumberToObject(edge, "distance", temp->attr.distance);
                cJSON_AddNumberToObject(edge, "baseTime", temp->attr.baseTime);
                cJSON_AddNumberToObject(edge, "cost", temp->attr.cost);
                cJSON_AddNumberToObject(edge, "roadType", temp->attr.roadType);
                cJSON_AddNumberToObject(edge, "reliability", temp->attr.reliability);
                cJSON_AddNumberToObject(edge, "restrictions", temp->attr.restrictions);

                cJSON_AddItemToArray(edges_array, edge);
            }
            temp = temp->next;
        }
    }

    char *json_str = cJSON_Print(root);
    FILE *file = fopen(filename, "w");
    if (file)
    {
        fprintf(file, "%s", json_str);
        fclose(file);
        printf("\nEtat du reseau sauvegarde dans %s\n", filename);
    }
    else
    {
        fprintf(stderr, "Erreur lors de l'ecriture du fichier\n");
    }

    cJSON_Delete(root);
    free(json_str);
}
