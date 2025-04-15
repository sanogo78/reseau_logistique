#ifndef GREEDY_H
#define GREEDY_H

#include "../graph/graph.h"

typedef struct {
    int vehicle_id;
    int capacity;
    int current_load;
    int* packages;
    int package_count;
    float total_distance;
    int* route;
    int route_size;
    int reassigned_packages;
    bool is_operational; //pour le statut du vehicule
} Vehicle;

typedef struct {
    int package_id;
    int weight;
    int destination;
    int priority; // 1=high, 2=medium, 3=low
    bool assigned;
} Package;

typedef struct {
    Vehicle* vehicles;
    int vehicle_count;
    Package* packages;
    int package_count;
    Graph* graph;
} DeliveryScenario;

// Fonctions pour l'approche gloutonne
DeliveryScenario* createDeliveryScenario(Graph* graph, int vehicle_count, int package_count);
void freeDeliveryScenario(DeliveryScenario* scenario);
void greedyPackageAssignment(DeliveryScenario* scenario);
void greedyRoutePlanning(DeliveryScenario* scenario);
void dynamicRedistribution(DeliveryScenario* scenario, int failed_vehicle_id);
void printVehicleStatus(const Vehicle* vehicle, const Graph* graph);
void printPackageStatus(const Package* package, const Graph* graph);
void printAllVehiclesPackagesStatus(const DeliveryScenario* scenario, const Graph* graph);
void testGreedyApproach(Graph* graph);
float getDistance(Graph* Graph, int node0, int node1); //distance

#endif