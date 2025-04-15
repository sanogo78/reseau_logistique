#include "graph.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>

Graph *createGraph(int V)
{
    Graph *graph = (Graph *)malloc(sizeof(Graph));
    graph->V = V;
    graph->array = (AdjList *)malloc(V * sizeof(AdjList));
    graph->nodes = (Node *)malloc(V * sizeof(Node));

    for (int i = 0; i < V; i++)
    {
        graph->array[i].head = NULL;
    }
    // Initialisation de la matrice des distances
    graph->distance = malloc(V * sizeof(float *));
    for (int i = 0; i < V; i++)
    {
        graph->distance[i] = malloc(V * sizeof(float));
        for (int j = 0; j < V; j++)
        {
            if (i == j)
            {
                graph->distance[i][j] = 0; // Distance de soi-même à soi-même
            }
            else
            {
                graph->distance[i][j] = FLT_MAX; // Initialiser à l'infini
            }
        }
    }

    return graph;
}

void addNode(Graph *graph, int nodeIndex, int id, const char *name, const char *type,
             float lat, float lon, int capacity, float earliest, float latest, float service)
{
    graph->nodes[nodeIndex].id = id;
    graph->nodes[nodeIndex].name = strdup(name);
    graph->nodes[nodeIndex].type = strdup(type);
    graph->nodes[nodeIndex].coordinates[0] = lat;
    graph->nodes[nodeIndex].coordinates[1] = lon;
    graph->nodes[nodeIndex].capacity = capacity;
    graph->nodes[nodeIndex].earliestTime = earliest;
    graph->nodes[nodeIndex].latestTime = latest;
    graph->nodes[nodeIndex].serviceTime = service;
}

void addEdge(Graph *graph, int src, int dest, EdgeAttr attr)
{
    // Ajouter l'arête dans la liste d'adjacence du sommet src
    AdjListNode *newNode = (AdjListNode *)malloc(sizeof(AdjListNode));
    newNode->dest = dest;
    newNode->attr = attr;
    newNode->next = graph->array[src].head;
    graph->array[src].head = newNode;

    // Ajouter l'arête dans la liste d'adjacence du sommet dest (pour un graphe non orienté)
    newNode = (AdjListNode *)malloc(sizeof(AdjListNode));
    newNode->dest = src;
    newNode->attr = attr;
    newNode->next = graph->array[dest].head;
    graph->array[dest].head = newNode;

    // Mise à jour de la matrice des distances
    graph->distance[src][dest] = attr.distance;
    graph->distance[dest][src] = attr.distance; // Si le graphe est non dirigé
}

// Fonction pour supprimer une arête entre src et dest
void removeEdge(Graph *graph, int src, int dest)
{
    // Supprimer l'arête dans la liste d'adjacence de src
    AdjListNode *temp = graph->array[src].head;
    AdjListNode *prev = NULL;
    while (temp != NULL && temp->dest != dest)
    {
        prev = temp;
        temp = temp->next;
    }
    if (temp != NULL)
    { // Si l'arête existe
        if (prev == NULL)
        { // Si l'arête est au début de la liste
            graph->array[src].head = temp->next;
        }
        else
        { // Sinon, ajuster le pointeur du précédent
            prev->next = temp->next;
        }
        free(temp);
    }

    // Supprimer l'arête dans la liste d'adjacence de dest (graphe non orienté)
    temp = graph->array[dest].head;
    prev = NULL;
    while (temp != NULL && temp->dest != src)
    {
        prev = temp;
        temp = temp->next;
    }
    if (temp != NULL)
    { // Si l'arête existe
        if (prev == NULL)
        { // Si l'arête est au début de la liste
            graph->array[dest].head = temp->next;
        }
        else
        { // Sinon, ajuster le pointeur du précédent
            prev->next = temp->next;
        }
        free(temp);
    }
}

// Fonction pour supprimer un noeud (et ses arêtes associées)
void removeNode(Graph *graph, int nodeIndex)
{
    // Supprimer toutes les arêtes associées à ce noeud dans toutes les listes d'adjacence
    for (int i = 0; i < graph->V; i++)
    {
        if (i != nodeIndex)
        { // Ne pas supprimer les arêtes liées au même noeud
            removeEdge(graph, i, nodeIndex);
        }
    }

    // Libérer la mémoire du nom et du type du noeud
    free(graph->nodes[nodeIndex].name);
    free(graph->nodes[nodeIndex].type);

    // Décaler les noeuds restants pour supprimer le noeud de l'index donné
    for (int i = nodeIndex; i < graph->V - 1; i++)
    {
        graph->nodes[i] = graph->nodes[i + 1]; // Décaler le noeud vers la gauche
    }

    // Décaler les listes d'adjacence pour supprimer la liste du noeud
    for (int i = nodeIndex; i < graph->V - 1; i++)
    {
        graph->array[i] = graph->array[i + 1];
    }

    // Réduire le nombre de sommets du graphe
    graph->V--;

    // Réallouer la mémoire du tableau de noeuds et de la liste d'adjacence
    graph->nodes = (Node *)realloc(graph->nodes, graph->V * sizeof(Node));
    graph->array = (AdjList *)realloc(graph->array, graph->V * sizeof(AdjList));
}

// Fonction pour afficher les informations détaillées d'un noeud
void printNodeInfo(Node node)
{
    printf("ID: %d, Name: %s, Type: %s, Coordinates: [%.6f, %.6f], Capacity: %d, Earliest Time: %.2f, Latest Time: %.2f, Service Time: %.2f\n\n",
           node.id, node.name, node.type, node.coordinates[0], node.coordinates[1], node.capacity, node.earliestTime, node.latestTime, node.serviceTime);
}

// Fonction pour afficher le graphe avec les informations détaillées des noeuds
void printGraph(Graph *graph)
{
    for (int i = 0; i < graph->V; i++)
    {
        printNodeInfo(graph->nodes[i]);
        AdjListNode *temp = graph->array[i].head;
        while (temp)
        {
            printf("  -> %s [Dist: %.2f km, Time: %.2f min, Cost: %.2f, Road Type: %d, Reliability: %.2f, Restrictions: %d]\n \n",
                   graph->nodes[temp->dest].name, temp->attr.distance, temp->attr.baseTime, temp->attr.cost,
                   temp->attr.roadType, temp->attr.reliability, temp->attr.restrictions);
            temp = temp->next;
        }
    }
}

// Affichage sous forme de liste d'adjacence
void printAdjList(Graph *graph)
{
    printf("\nRepresentation en liste d'adjacence:\n");
    for (int v = 0; v < graph->V; ++v)
    {
        AdjListNode *temp = graph->array[v].head;
        printf("%d:", v); // Affiche le sommet de départ

        while (temp)
        {
            printf(" -> %d", temp->dest); // Affiche chaque destination
            temp = temp->next;
        }
        printf("\n"); // Nouvelle ligne pour le sommet suivant
    }
}

EdgeAttr getEdgeAttr(Graph *graph, int from, int to)
{
    // On parcourt la liste d’adjacence pour trouver l'arête entre 'from' et 'to'
    AdjListNode *current = graph->array[from].head;
    while (current != NULL)
    {
        if (current->dest == to)
        {
            return current->attr; // Retourne les attributs de l'arête
        }
        current = current->next;
    }
    // Retourne des valeurs par défaut si l'arête n'existe pas
    EdgeAttr defaultAttr = {0.0, 0.0, 0.0, 0, 0.0, 0};
    return defaultAttr;
}