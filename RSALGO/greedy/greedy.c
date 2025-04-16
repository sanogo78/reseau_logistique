#include "greedy.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <float.h>

//+++++++++ Algorithme de test de l'approche +++++++++++++
void testGreedyApproach(Graph *graph)
{
    if (!graph || graph->V == 0)
    {
        fprintf(stderr, "ERREUR: Graphe invalide ou vide!\n");
        return;
    }

    printf("\n=== TEST APPROCHE GLOUTONNE ===\n");
    printf("Initialisation avec %d noeuds dans le graphe\n", graph->V);

    const int nb_vehicules = 3;
    const int nb_colis = 10;
    printf("Creation scenario: %d vehicules, %d colis\n", nb_vehicules, nb_colis);

    DeliveryScenario *scenario = createDeliveryScenario(graph, nb_vehicules, nb_colis);
    if (!scenario)
    {
        fprintf(stderr, "ERREUR: Echec creation scenario\n");
        return;
    }

    // 1. Affectation initiale
    printf("\n=== PHASE 1: AFFECTATION DES COLIS ===\n");
    greedyPackageAssignment(scenario);

    // Verification affichage colis
    printf("\nColis (%d):\n", scenario->package_count);
    for (int i = 0; i < scenario->package_count; i++)
    {
        if (!scenario->packages[i].assigned)
        {
            printf("! Colis %d NON ASSIGNE !\n", i);
        }
        printPackageStatus(&scenario->packages[i], graph);
    }

    // 2. Planification
    printf("\n=== PHASE 2: PLANIFICATION TOURNEES ===\n");
    greedyRoutePlanning(scenario);

    printf("\nVehicules:\n");
    for (int i = 0; i < scenario->vehicle_count; i++)
    {
        if (scenario->vehicles[i].package_count == 0)
        {
            printf("! Vehicule %d n'a aucun colis !\n", i);
        }
        printVehicleStatus(&scenario->vehicles[i], graph);
    }

    // 3. Test panne
    printf("\n=== PHASE 3: TEST PANNE VEHICULE ===\n");
    const int vehicule_en_panne = 1;

    if (vehicule_en_panne >= scenario->vehicle_count)
    {
        fprintf(stderr, "ERREUR: ID vehicule invalide\n");
        freeDeliveryScenario(scenario);
        return;
    }

    printf("\netat avant panne:\n");
    printVehicleStatus(&scenario->vehicles[vehicule_en_panne], graph);

    dynamicRedistribution(scenario, vehicule_en_panne);

    printf("\netat final:\n");
    printAllVehiclesPackagesStatus(scenario, graph);

    freeDeliveryScenario(scenario);
    printf("\n=== TEST TERMINE ===\n");
}

// ++++++++++ Algorithme de création d'un scenario
DeliveryScenario *createDeliveryScenario(Graph *graph, int vehicle_count, int package_count)
{
    DeliveryScenario *scenario = (DeliveryScenario *)malloc(sizeof(DeliveryScenario));
    scenario->graph = graph;
    scenario->vehicle_count = vehicle_count;
    scenario->package_count = package_count;

    // Initialisation des vehicules
    scenario->vehicles = (Vehicle *)malloc(vehicle_count * sizeof(Vehicle));
    for (int i = 0; i < vehicle_count; i++)
    {
        scenario->vehicles[i].vehicle_id = i;
        scenario->vehicles[i].capacity = 100;
        scenario->vehicles[i].current_load = 0;
        scenario->vehicles[i].package_count = 0;
        scenario->vehicles[i].packages = (int *)malloc(package_count * sizeof(int));
        scenario->vehicles[i].total_distance = 0.0f;
        scenario->vehicles[i].route = (int *)malloc(graph->V * sizeof(int));
        scenario->vehicles[i].route_size = 0;
        scenario->vehicles[i].reassigned_packages = 0;
        scenario->vehicles[i].is_operational = true;
    }

    // Initialisation des colis
    scenario->packages = (Package *)malloc(package_count * sizeof(Package));
    for (int i = 0; i < package_count; i++)
    {
        scenario->packages[i].package_id = i;
        scenario->packages[i].weight = rand() % 20 + 1;
        scenario->packages[i].destination = rand() % graph->V;
        scenario->packages[i].priority = rand() % 3 + 1;
        scenario->packages[i].assigned = false;
    }

    return scenario;
}

void freeDeliveryScenario(DeliveryScenario *scenario)
{
    for (int i = 0; i < scenario->vehicle_count; i++)
    {
        free(scenario->vehicles[i].packages);
        free(scenario->vehicles[i].route);
    }
    free(scenario->vehicles);
    free(scenario->packages);
    free(scenario);
}

// ++++++++++++ Algorithme d'affectation des colis ++++++++++++++
void greedyPackageAssignment(DeliveryScenario *scenario)
{
    // Tri des colis par priorite
    for (int i = 0; i < scenario->package_count - 1; i++)
    {
        for (int j = i + 1; j < scenario->package_count; j++)
        {
            if (scenario->packages[i].priority > scenario->packages[j].priority)
            {
                Package temp = scenario->packages[i];
                scenario->packages[i] = scenario->packages[j];
                scenario->packages[j] = temp;
            }
        }
    }

    // Affectation gloutonne
    for (int p = 0; p < scenario->package_count; p++)
    {
        if (scenario->packages[p].assigned)
            continue;

        int best_vehicle = -1;
        float best_score = INT_MAX;

        for (int v = 0; v < scenario->vehicle_count; v++)
        {
            if (scenario->vehicles[v].current_load + scenario->packages[p].weight <= scenario->vehicles[v].capacity)
            {
                float score = (scenario->vehicles[v].capacity - scenario->vehicles[v].current_load) * (4 - scenario->packages[p].priority);
                if (score < best_score)
                {
                    best_score = score;
                    best_vehicle = v;
                }
            }
        }

        if (best_vehicle != -1)
        {
            Vehicle *vehicle = &scenario->vehicles[best_vehicle];
            vehicle->packages[vehicle->package_count++] = p;
            vehicle->current_load += scenario->packages[p].weight;
            scenario->packages[p].assigned = true;
        }
    }
}

float getDistance(Graph *graph, int node0, int node1)
{
    AdjListNode *temp = graph->array[node0].head;
    while (temp != NULL)
    {
        if (temp->dest == node1)
        {
            return temp->attr.distance;
        }
        temp = temp->next;
    }
    return __FLT_MAX__;
}

// ++++++ Algorithme de planification de tourné +++++++++++
void greedyRoutePlanning(DeliveryScenario *scenario)
{
    for (int v = 0; v < scenario->vehicle_count; v++)
    {
        Vehicle *vehicle = &scenario->vehicles[v];
        if (vehicle->package_count == 0)
            continue;

        vehicle->route[0] = 0;
        vehicle->route_size = 1;
        vehicle->total_distance = 0.0f;

        int *destinations = (int *)malloc(vehicle->package_count * sizeof(int));
        int dest_count = 0;

        for (int p = 0; p < vehicle->package_count; p++)
        {
            int dest = scenario->packages[vehicle->packages[p]].destination;
            bool already_in_list = false;

            for (int i = 0; i < dest_count; i++)
            {
                if (destinations[i] == dest)
                {
                    already_in_list = true;
                    break;
                }
            }

            if (!already_in_list)
            {
                destinations[dest_count++] = dest;
            }
        }

        int current_node = 0;
        while (dest_count > 0)
        {
            int nearest_node = -1;
            float min_distance = __FLT_MAX__;

            for (int i = 0; i < dest_count; i++)
            {
                float distance = getDistance(scenario->graph, current_node, destinations[i]);
                if (distance < min_distance && distance != __FLT_MAX__)
                {
                    min_distance = distance;
                    nearest_node = i;
                }
            }

            if (nearest_node == -1)
            {
                fprintf(stderr, "Avertissement : aucun chemin valide trouve depuis le nœud %d\n", current_node);
                break;
            }

            current_node = destinations[nearest_node];
            vehicle->route[vehicle->route_size++] = current_node;
            vehicle->total_distance += min_distance;
            destinations[nearest_node] = destinations[--dest_count];
        }

        if (current_node != 0)
        {
            float return_distance = getDistance(scenario->graph, current_node, 0);
            if (return_distance != __FLT_MAX__)
            {
                vehicle->route[vehicle->route_size++] = 0;
                vehicle->total_distance += return_distance;
            }
        }

        free(destinations);
    }
}

// algorithme de redistribution
void dynamicRedistribution(DeliveryScenario *scenario, int failed_vehicle_id)
{
    printf("\nDebut redistribution pour vehicule %d en panne...\n", failed_vehicle_id);
    Vehicle *failed_vehicle = &scenario->vehicles[failed_vehicle_id];
    failed_vehicle->is_operational = false;
    int total_reassigned = 0;

    printf("\nColis a reaffecter : %d\n", failed_vehicle->package_count);

    for (int p = 0; p < failed_vehicle->package_count; p++)
    {
        Package *package = &scenario->packages[failed_vehicle->packages[p]];
        package->assigned = false;

        int best_vehicle = -1;
        float best_score = INT_MAX;

        for (int v = 0; v < scenario->vehicle_count; v++)
        {
            if (v == failed_vehicle_id || !scenario->vehicles[v].is_operational)
                continue;

            if (scenario->vehicles[v].current_load + package->weight <= scenario->vehicles[v].capacity)
            {
                float additional_distance = getDistance(scenario->graph,
                                                        scenario->vehicles[v].route[scenario->vehicles[v].route_size - 2],
                                                        package->destination);

                float score = (scenario->vehicles[v].capacity - scenario->vehicles[v].current_load) *
                              (4 - package->priority) * additional_distance;

                if (score < best_score)
                {
                    best_score = score;
                    best_vehicle = v;
                }
            }
        }

        if (best_vehicle != -1)
        {
            Vehicle *vehicle = &scenario->vehicles[best_vehicle];
            vehicle->packages[vehicle->package_count++] = failed_vehicle->packages[p];
            vehicle->reassigned_packages++;
            total_reassigned++;
            vehicle->current_load += package->weight;
            package->assigned = true;

            if (vehicle->route_size > 0)
            {
                vehicle->route[vehicle->route_size - 1] = package->destination;
                vehicle->route[vehicle->route_size++] = 0;
                vehicle->total_distance += getDistance(scenario->graph,
                                                       vehicle->route[vehicle->route_size - 3], package->destination);
                vehicle->total_distance += getDistance(scenario->graph,
                                                       package->destination, 0);
            }
        }
    }

    failed_vehicle->current_load = 0;
    failed_vehicle->package_count = 0;
    failed_vehicle->route_size = 0;
    failed_vehicle->total_distance = 0;
    failed_vehicle->reassigned_packages = total_reassigned;

    printf("Redistribution terminee. %d/%d colis reaffectes\n",
           total_reassigned, failed_vehicle->package_count);
}

//
void printPackageStatus(const Package *package, const Graph *graph)
{
    printf("Colis %d:\n", package->package_id);
    printf("  Poids: %d kg\n", package->weight);
    printf("  Destination: %s\n", graph->nodes[package->destination].name);
    printf("  Priorite: %d\n", package->priority);
    printf("  Statut: %s\n", package->assigned ? "Assigne" : "Non assigne");
}

void printVehicleStatus(const Vehicle *vehicle, const Graph *graph)
{
    printf("Vehicule %d (%s):\n",
           vehicle->vehicle_id,
           vehicle->is_operational ? "Operationnel" : "EN PANNE");
    printf("  Capacite: %d/%d\n", vehicle->current_load, vehicle->capacity);
    printf("  Colis: %d (dont %d reaffectes)\n",
           vehicle->package_count, vehicle->reassigned_packages);
    printf("  Distance totale: %.2f km\n", vehicle->total_distance);
    printf("  Tournee: ");

    for (int i = 0; i < vehicle->route_size; i++)
    {
        if (i > 0)
            printf(" -> ");
        printf("%s", graph->nodes[vehicle->route[i]].name);
    }
    printf("\n");
}

//++++++++++ Afficher un resumé globale sur le statut des colis par véhicule ++++++++++++++
void printAllVehiclesPackagesStatus(const DeliveryScenario *scenario, const Graph *graph)
{
    if (!scenario || !graph)
    {
        fprintf(stderr, "Erreur : scenario ou graphe invalide\n");
        return;
    }

    printf("\n=== STATUT DES COLIS PAR VEHICULE ===\n");

    for (int v = 0; v < scenario->vehicle_count; v++)
    {
        printf("\n--- Vehicule %d ---\n", scenario->vehicles[v].vehicle_id);
        printVehicleStatus(&scenario->vehicles[v], graph);

        if (scenario->vehicles[v].package_count > 0)
        {
            printf("Colis assignes (%d):\n", scenario->vehicles[v].package_count);
            for (int p = 0; p < scenario->vehicles[v].package_count; p++)
            {
                int package_id = scenario->vehicles[v].packages[p];
                printPackageStatus(&scenario->packages[package_id], graph);
            }
        }
        else
        {
            printf("Aucun colis assigne\n");
        }
    }
}
