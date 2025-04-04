#include <stdio.h>
#include <stdlib.h>
#include "graph/graph.h"
#include "traversal/traversal.h"
#include "json/json_utils.h"
#include "floyd/floyd.h"
#include "temp/temp.h"
#include "bellman-ford/bellman.h"

// Fonction principale
int main()
{
    // Création du graphe à partir du fichier JSON
    Graph *graph = createGraphFromJSON("parseur.json");
    if (!graph)
    {
        fprintf(stderr, "Erreur lors de la creation du graphe\n");
        return 1;
    }

    // Afficher le graphe
    printf("Graphe charge depuis le fichier parseur.js:\n");
    printGraph(graph);
    printAdjList(graph);

    // Exécuter DFS à partir du nœud 0 (optionnel)
    int *pi_dfs = depthFirstSearch(graph, 0, NULL);
    printDFSTree(pi_dfs, graph->V, graph->nodes);
    free(pi_dfs);

    // Exécuter BFS à partir du nœud 0
    int *pi_bfs = breadthFirstSearch(graph, 0, NULL);
    printBFSTree(pi_bfs, graph->V, graph->nodes);
    free(pi_bfs);

    // 1. Détection de cycles
    if (hasCycle(graph))
    {
        printf("\nLe reseau contient au moins un cycle.\n");
    }
    else
    {
        printf("\nLe reseau est acyclique.\n");
    }

    // 2. Composantes connexes
    findConnectedComponents(graph);

    // 3.1 Points d'articulation
    findArticulationPoints(graph);

    // 3.2 Accessibilité
    int nodeA = 0, nodeB = 3;
    if (graph && graph->nodes &&
        nodeA >= 0 && nodeA < graph->V &&
        nodeB >= 0 && nodeB < graph->V &&
        graph->nodes[nodeA].name && graph->nodes[nodeB].name)
    {

        printf("\nAccessibilite entre %s (%d) et %s (%d): %s\n",
               graph->nodes[nodeA].name, nodeA,
               graph->nodes[nodeB].name, nodeB,
               isReachable(graph, nodeA, nodeB) ? "OUI" : "NON");
    }
    else
    {
        printf("\nErreur : Indices de noeuds invalides ou graphe non initialiser.\n");
    }

    // 5. Statistiques
    NetworkStats stats = calculateNetworkStats(graph);
    printf("\nStatistiques du reseau:\n");
    printf("- Nombre de noeuds: %d\n", stats.nodeCount);
    printf("- Nombre d'aretes: %d\n", stats.edgeCount);
    printf("- Degre moyen: %.2f\n", stats.averageDegree);
    printf("- Nombre de points d'articulation: %d\n", stats.articulationPointsCount);
    printf("- Nombre de composantes connexes: %d\n", stats.connectedComponents);
    printf("- Contient des cycles: %s\n", stats.isCyclic ? "OUI" : "NON");

        // Algorithme de floyd
    // Calcul des plus courts chemins
    ShortestPaths *sp = findShortestPath(graph);
    seeShortestPath(sp, nodeA, nodeB, graph->nodes);

    // Affichage de la matrice des distances
    printf("\nMatrice des distances:\n");
    for (int i = 0; i < graph->V; i++)
    {
        for (int j = 0; j < graph->V; j++)
        {
            if (sp->distance[i][j] == __FLT_MAX__)
            {
                printf("INF ");
            }
            else
            {
                printf("%.1f ", sp->distance[i][j]);
            }
        }
        printf("\n");
    }

    // Sauvegarder l'état du réseau
    saveNetworkState(graph, "saveNetwork.json");

    free(graph->array);
    free(graph->nodes);
    free(graph);
    freeShortestPaths(sp);

    return 0;

    //Le plus court chemin
    TimeContext context;
    context.timePeriod = HEURE_POINTE;
    context.season = SAISON_PLUVIEUSE;

    BellmanResult *br = runBellmanFord(graph, nodeA, context);
    if (br) {
        showPathBellman(br, nodeA, nodeB, graph->nodes);
        freeBellmanResult(br);
    }
}
