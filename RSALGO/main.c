#include "all_include_main.h"
// Fonction principale
int main()
{
    int nodeA;
    int nodeB;
    srand(time(NULL)); // Seed pour le mélange aléatoire
    // Création du graphe à partir du fichier JSON
    Graph *graph = parseurFromJSON("parseur.json");
    if (!graph)
    {
        fprintf(stderr, "Erreur lors de la creation du graphe\n");
        return 1;
    }

    printDepartDestination(graph);
    do
    {
        printf("Choisir un lieu de depart (entre 0 et %d) : ", graph->V - 1);
        if (scanf("%d", &nodeA) != 1)
        {
            fprintf(stderr, "Entree invalide.\n");
            // Vider le buffer en cas d'entrée non entière
            while (getchar() != '\n')
                ;
            nodeA = -1; // force à recommencer
        }
    } while (nodeA < 0 || nodeA >= graph->V);

    do
    {
        printf("Choisir un lieu de destination (entre 0 et %d) : ", graph->V - 1);
        if (scanf("%d", &nodeB) != 1)
        {
            fprintf(stderr, "Entree invalide.\n");
            // Vider le buffer en cas d'entrée non entière
            while (getchar() != '\n')
                ;
            nodeB = -1; // force à recommencer
        }
    } while (nodeB < 0 || nodeB >= graph->V);

    time_t new_date = time(NULL);
    struct tm *pTime = localtime(&new_date);

    char buffer[100];
    strftime(buffer, sizeof(buffer), "%d/%m/%Y %H:%M:%S", pTime);
    printf("SCENARIO : Jour normale\n");
    printf("DATE : %s\n\n", buffer);

    // Afficher le graphe
    printf("Graphe charge depuis le fichier parseur.js:\n");
    printGraph(graph);
    printAdjList(graph);

    // Exécuter DFS à partir du nœud nodeA
    printf("\n // Parcours DFS\n");
    int *pi_dfs = depthFirstSearch(graph, nodeA, NULL);
    printDFSTree(pi_dfs, graph->V, graph->nodes);
    free(pi_dfs);

    printf("\n //Parcours BFS\n");
    // Exécuter BFS à partir du nœud nodeA
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
    printf("\n//Algorithme de floyd");
    ShortestPaths *sp = findShortestPath(graph);

    seeShortestPath(sp, nodeA, nodeB, graph->nodes);

    // Affichage de la matrice des distances
    printf("\nMatrice des distances:\n");
    for (int i = 0; i < graph->V; i++)
    {
        for (int j = 0; j < graph->V; j++)
        {
            if (sp->distance[i][j] == FLT_MAX)
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

    // Bellman ford
    printf("\n // Algorithme de Bellman-Ford");
    BellmanResult *br = findShortestPathBellman(graph, nodeA);
    if (br != NULL)
    {
        seeBellmanPath(br, nodeA, nodeB, graph->nodes);

        // Affichage des distances depuis la source
        printf("\nDistances depuis %s (%d):\n", graph->nodes[nodeA].name, nodeA);
        for (int i = 0; i < graph->V; i++)
        {
            if (br->distance[i] == FLT_MAX)
                printf("Vers %s (%d): INF\n", graph->nodes[i].name, i);
            else
                printf("Vers %s (%d): %.1f\n", graph->nodes[i].name, i, br->distance[i]);
        }

        freeBellmanResult(br);
    }

    printf("\n//Solution au probleme du voyageur de commerce");
    TSPResult *tour = solveTSP(sp->distance, graph->V);
    printTSPResult(tour);
    freeTSPResult(tour);

    // Programmation dynamique
    printf("\nProgrammation dynamique\n");
    TimeContext timeContext;
    timeContext.season = 1;
    timeContext.congestionRate = 0.7; // Par exemple, 70% de congestion
    int start = nodeA;
    int numDays = 7;
    char vehicle[20];
    strcpy(vehicle, "Camion frigorifique (800kg)");
    dynamicprog(graph, start, numDays, timeContext, vehicle);

    // Approche gloutonne
    testGreedyApproach(graph);

    // représentation chromosomique
    Chromosome *chrom = createRandomChromosome(graph->V);
    if (!chrom)
        return 1;

    // Découpage en tournées (capacité max = 3 clients/tournée)
    decodeChromosome(chrom, 3);

    // Validation et calcul des coûts
    if (!isValidChromosome(chrom, graph))
    {
        printf("Chromosome invalide !\n");
        freeChromosome(chrom);
        return 1;
    }

    Season season = SAISON_PLUVIEUSE;
    TimePeriod timer = NUIT;
    ChromosomeCost(chrom, graph, season, timer);

    // Affichage
    printf("\n=== Chromosome optimise ===\n");
    printChromosome(chrom, graph);
    printf("\n-> Cout total : %.2f FCFA\n", chrom->totalCost);
    printf("-> Duree totale : %.2f min\n", chrom->totalTime);
    printf("-> Distance totale : %.2f km\n", chrom->totalDistance);

    // Crossing
    printf("\n=== Croisement ===\n");
    Chromosome *parent1 = createRandomChromosome(graph->V);
    Chromosome *parent2 = createRandomChromosome(graph->V);
    Chromosome *child = createRandomChromosome(graph->V); // Enfant vide

    // Appliquer le croisement
    crossover(parent1, parent2, child, graph, season, timer);

    printf("Parent 1 : ");
    printChromosome(parent1, graph);
    printf("\nParent 2 : ");
    printChromosome(parent2, graph);
    printf("\nEnfant : ");
    printChromosome(child, graph);

    // Mutation
    printf("\n=== MUTATION ===\n");
    adaptiveMutation(child, graph, season, timer);

    printf("==> AVANT MUTATION\n");
    printChromosome(chrom, graph);
    printf("Cout total: %.2f FCFA | Duree: %.2f min | Distance: %.2f km\n\n",
           chrom->totalCost, chrom->totalTime, chrom->totalDistance);

    // Copie pour mutation (protège l'original)
    Chromosome *mutated = copyChromosome(chrom); // À implémenter

    adaptiveMutation(mutated, graph, season, timer);
    ChromosomeCost(mutated, graph, season, timer);

    // Affichage après mutation
    printf("==> APRES MUTATION \n");
    printMutation(mutated, graph);
    printf("Cout total: %.2f FCFA | Duree: %.2f min | Distance: %.2f km\n\n",
           mutated->totalCost, mutated->totalTime, mutated->totalDistance);

    // Validation
    if (!isValidChromosome(mutated, graph))
    {
        printf("Solution invalide detectee ! Reparation...\n");
        repairChromosome(mutated, graph, season, timer);
        ChromosomeCost(mutated, graph, season, timer);
        printf("=== APRES REPARATION ===\n");
        printMutation(mutated, graph);
    }

    // Fitness
    //  1. Création de la population initiale
    printf("=== FITNESS ===\n");
    const int populationSize = 5;
    Chromosome *population[populationSize];
    for (int i = 0; i < populationSize; i++)
    {
        population[i] = createRandomChromosome(graph->V);
        decodeChromosome(population[i], 3); // Capacité max = 3
    }

    // 2. Évaluation de la population
    printf("==> EVALUATION INITIALE\n");
    for (int i = 0; i < populationSize; i++)
    {
        float fitness = evaluateFitness(population[i], graph, season, timer);
        printf("Solution %d:\n", i + 1);
        printChromosome(population[i], graph);
        printf("Fitness: %.2f (Cout: %.2f FCFA | Temps: %.2f min | Distance: %.2f Km)\n\n",
               fitness, population[i]->totalCost, population[i]->totalTime, population[i]->totalDistance);
    }

    // 3. Sélection du meilleur
    Chromosome *best = findBestSolution(population, populationSize);
    printf("\n==> MEILLEURE SOLUTION INITIALE \n");
    printChromosome(best, graph);
    printf("Fitness: %.2f\n", evaluateFitness(best, graph, season, timer));

    // 4. exemple d'évolution (1 generation avec élitise + tournoi)
    printf("\n==== APRES GENERATION (ELITISME + TOURNOI=====\n");

    Chromosome *newPopulation[populationSize];
    // Elitisme: on copie le meilleur directement
    newPopulation[0] = copyChromosome(best);

    // generation des autres individus via sélection par tournoi
    for (int i = 1; i < populationSize; i++)
    {
        // selection des parents par tournoi
        Chromosome *parent1 = selectParent(population, populationSize);
        Chromosome *parent2 = selectParent(population, populationSize);
        Chromosome *child = copyChromosome(parent1);

        // Mutation adaptative
        adaptiveMutation(child, graph, season, timer);
        newPopulation[i] = child;
    }

    // Évaluation de la nouvelle population
    for (int i = 0; i < populationSize; i++)
    {
        evaluateFitness(newPopulation[i], graph, season, timer);
    }

    // Affichage des résultats
    Chromosome *newBest = findBestSolution(newPopulation, populationSize);

    printChromosome(newBest, graph);
    float amelioration = (best->totalCost - newBest->totalCost) / best->totalCost * 100;
    printf("Amelioration: %.2f  \n", amelioration);

    // Nettoyage
    for (int i = 0; i < populationSize; i++)
    {
        freeChromosome(population[i]);
        freeChromosome(newPopulation[i]);
    }

    // Nettoyage
    freeChromosome(chrom);

    // partie TSP
    freeShortestPaths(sp);

    // Sauvegarder l'état du réseau
    saveNetworkState(graph, "saveNetwork.json");

    free(graph->array);
    free(graph->nodes);
    free(graph);

    return 0;
}
