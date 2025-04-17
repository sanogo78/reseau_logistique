#include "traversal.h"
#include "../queue/queue.h"
#include <stdlib.h>
#include <stdbool.h>

int *depthFirstSearch(Graph *graph, int startNode, int *tree)
{
    NodeColor *colors = (NodeColor *)malloc(graph->V * sizeof(NodeColor));
    int *pi = (int *)malloc(graph->V * sizeof(int));
    for (int i = 0; i < graph->V; i++)
    {
        colors[i] = WHITE;
        pi[i] = -1; // -1 signifie pas de prédécesseur
    }
    int *stack = (int *)malloc(graph->V * sizeof(int));
    int stackTop = -1; // Indice du sommet de la pile
    stack[++stackTop] = startNode;
    colors[startNode] = GRAY;

    while (stackTop >= 0)
    {                                      // Tant que la pile n'est pas vide
        int currentNode = stack[stackTop]; // Prendre le noeud en haut de la pile
        AdjListNode *neighbor = graph->array[currentNode].head;
        bool foundWhite = false;

        while (neighbor != NULL && !foundWhite)
        {
            if (colors[neighbor->dest] == WHITE)
            {
                foundWhite = true;
                stack[++stackTop] = neighbor->dest;
                colors[neighbor->dest] = GRAY;
                pi[neighbor->dest] = currentNode;
            }
            neighbor = neighbor->next;
        }
        if (!foundWhite)
        {
            stackTop--; // Dépiler
            colors[currentNode] = BLACK;
        }
    }

    free(colors);
    free(stack);

    if (tree != NULL)
    {
        for (int i = 0; i < graph->V; i++)
        {
            tree[i] = pi[i];
        }
    }

    return pi;
}

// Fonction pour afficher l'arborescence DFS
void printDFSTree(int *pi, int V, Node *nodes)
{
    printf("\nArborescence DFS:\n");
    for (int i = 0; i < V; i++)
    {
        if (pi[i] != -1)
        {
            printf("%s (%d) -> %s (%d)\n",
                   nodes[pi[i]].name, pi[i],
                   nodes[i].name, i);
        }
        else if (i != 0)
        { // Supposant que 0 est le nœud de départ
            printf("Noeud %s (%d) n'est pas connecté\n", nodes[i].name, i);
        }
    }
}

// Fonction pour le parcours BFS
int *breadthFirstSearch(Graph *graph, int startNode, int *tree)
{
    NodeColor *colors = (NodeColor *)malloc(graph->V * sizeof(NodeColor));
    int *pi = (int *)malloc(graph->V * sizeof(int));

    for (int i = 0; i < graph->V; i++)
    {
        colors[i] = WHITE;
        pi[i] = -1; // -1 signifie pas de prédécesseur
    }
    Queue *queue = createQueue(graph->V);
    enqueue(queue, startNode);
    colors[startNode] = GRAY;

    while (!isEmpty(queue))
    {
        int currentNode = dequeue(queue);

        AdjListNode *neighbor = graph->array[currentNode].head;
        while (neighbor != NULL)
        {
            if (colors[neighbor->dest] == WHITE)
            {
                enqueue(queue, neighbor->dest);
                colors[neighbor->dest] = GRAY;
                pi[neighbor->dest] = currentNode;
            }
            neighbor = neighbor->next;
        }
        colors[currentNode] = BLACK;
    }

    free(colors);
    free(queue->array);
    free(queue);

    if (tree != NULL)
    {
        for (int i = 0; i < graph->V; i++)
        {
            tree[i] = pi[i];
        }
    }

    return pi;
}

// Fonction pour afficher l'arborescence BFS
void printBFSTree(int *pi, int V, Node *nodes)
{
    printf("\nArborescence BFS:\n");
    for (int i = 0; i < V; i++)
    {
        if (pi[i] != -1)
        {
            printf("%s (%d) -> %s (%d)\n",
                   nodes[pi[i]].name, pi[i],
                   nodes[i].name, i);
        }
        else if (i != 0)
        { // Supposant que 0 est le nœud de départ
            printf("Noeud %s (%d) n'est pas connecté\n", nodes[i].name, i);
        }
    }
}

// cycle dans le reseau
bool isCyclicUtil(Graph *graph, int v, bool visited[], int parent)
{
    visited[v] = true;
    AdjListNode *node = graph->array[v].head;
    while (node != NULL)
    {
        if (!visited[node->dest])
        {
            if (isCyclicUtil(graph, node->dest, visited, v))
                return true;
        }
        else if (node->dest != parent)
        {
            return true;
        }
        node = node->next;
    }
    return false;
}
bool hasCycle(Graph *graph)
{
    bool *visited = (bool *)malloc(graph->V * sizeof(bool));
    for (int i = 0; i < graph->V; i++)
        visited[i] = false;
    for (int u = 0; u < graph->V; u++)
    {
        if (!visited[u])
        {
            if (isCyclicUtil(graph, u, visited, -1))
            {
                free(visited);
                return true;
            }
        }
    }
    free(visited);
    return false;
}

// composant cornexe
void DFSUtil(Graph *graph, int v, bool visited[], int component[], int currentComponent)
{
    visited[v] = true;
    component[v] = currentComponent;

    AdjListNode *node = graph->array[v].head;
    while (node != NULL)
    {
        if (!visited[node->dest])
        {
            DFSUtil(graph, node->dest, visited, component, currentComponent);
        }
        node = node->next;
    }
}

void findConnectedComponents(Graph *graph)
{
    bool *visited = (bool *)malloc(graph->V * sizeof(bool));
    int *component = (int *)malloc(graph->V * sizeof(int));
    int currentComponent = 0;

    for (int v = 0; v < graph->V; v++)
    {
        visited[v] = false;
        component[v] = -1;
    }

    for (int v = 0; v < graph->V; v++)
    {
        if (!visited[v])
        {
            DFSUtil(graph, v, visited, component, currentComponent);
            currentComponent++;
        }
    }

    printf("\nComposantes connexes:\n");
    for (int i = 0; i < currentComponent; i++)
    {
        printf("Composante %d: ", i);
        for (int v = 0; v < graph->V; v++)
        {
            if (component[v] == i)
            {
                printf("%s (%d), ", graph->nodes[v].name, v);
            }
        }
        printf("\n");
    }

    free(visited);
    free(component);
}

void APUtil(Graph *graph, int u, bool visited[], int disc[], int low[], int parent[], bool ap[])
{
    static int time = 0;
    int children = 0;

    visited[u] = true;
    disc[u] = low[u] = ++time;

    AdjListNode *node = graph->array[u].head;
    while (node != NULL)
    {
        int v = node->dest;

        if (!visited[v])
        {
            children++;
            parent[v] = u;
            APUtil(graph, v, visited, disc, low, parent, ap);

            low[u] = (low[u] < low[v]) ? low[u] : low[v];

            if (parent[u] == -1 && children > 1)
                ap[u] = true;

            if (parent[u] != -1 && low[v] >= disc[u])
                ap[u] = true;
        }
        else if (v != parent[u])
        {
            low[u] = (low[u] < disc[v]) ? low[u] : disc[v];
        }
        node = node->next;
    }
}

// Points d'articulation
void findArticulationPoints(Graph *graph)
{
    bool *visited = (bool *)malloc(graph->V * sizeof(bool));
    int *disc = (int *)malloc(graph->V * sizeof(int));
    int *low = (int *)malloc(graph->V * sizeof(int));
    int *parent = (int *)malloc(graph->V * sizeof(int));
    bool *ap = (bool *)malloc(graph->V * sizeof(bool));

    for (int i = 0; i < graph->V; i++)
    {
        parent[i] = -1;
        visited[i] = false;
        ap[i] = false;
    }

    for (int i = 0; i < graph->V; i++)
    {
        if (!visited[i])
        {
            APUtil(graph, i, visited, disc, low, parent, ap);
        }
    }

    printf("\nPoints d'articulation:\n");
    bool found = false;
    for (int i = 0; i < graph->V; i++)
    {
        if (ap[i])
        {
            printf("%s (%d)\n", graph->nodes[i].name, i);
            found = true;
        }
    }
    if (!found)
    {
        printf("Aucun point d'articulation trouve\n");
    }

    free(visited);
    free(disc);
    free(low);
    free(parent);
    free(ap);
}

// vérifications de l'accessibilité entre différents Zone de reseau
bool isReachable(Graph *graph, int src, int dest)
{
    if (src == dest)
        return true;

    bool *visited = (bool *)malloc(graph->V * sizeof(bool));
    for (int i = 0; i < graph->V; i++)
        visited[i] = false;

    Queue *queue = createQueue(graph->V);
    visited[src] = true;
    enqueue(queue, src);
    while (!isEmpty(queue))
    {
        int current = dequeue(queue);

        AdjListNode *node = graph->array[current].head;
        while (node != NULL)
        {
            if (node->dest == dest)
            {
                free(queue->array);
                free(queue);
                free(visited);
                return true;
            }
            if (!visited[node->dest])
            {
                visited[node->dest] = true;
                enqueue(queue, node->dest);
            }
            node = node->next;
        }
    }
    free(queue->array);
    free(queue);
    free(visited);
    return false;
}

NetworkStats calculateNetworkStats(Graph *graph)
{
    NetworkStats stats;
    stats.nodeCount = graph->V;

    // Compter les arêtes
    stats.edgeCount = 0;
    for (int i = 0; i < graph->V; i++)
    {
        AdjListNode *node = graph->array[i].head;
        while (node != NULL)
        {
            // Pour éviter de compter deux fois (graphe non orienté)
            if (i < node->dest)
            {
                stats.edgeCount++;
            }
            node = node->next;
        }
    }

    // Degré moyen
    stats.averageDegree = (float)(2 * stats.edgeCount) / stats.nodeCount;

    // Points d'articulation
    bool *ap = (bool *)malloc(graph->V * sizeof(bool));
    bool *visited = (bool *)malloc(graph->V * sizeof(bool));
    int *disc = (int *)malloc(graph->V * sizeof(int));
    int *low = (int *)malloc(graph->V * sizeof(int));
    int *parent = (int *)malloc(graph->V * sizeof(int));

    for (int i = 0; i < graph->V; i++)
    {
        parent[i] = -1;
        visited[i] = false;
        ap[i] = false;
    }

    for (int i = 0; i < graph->V; i++)
    {
        if (!visited[i])
        {
            APUtil(graph, i, visited, disc, low, parent, ap);
        }
    }

    stats.articulationPointsCount = 0;
    for (int i = 0; i < graph->V; i++)
    {
        if (ap[i])
        {
            stats.articulationPointsCount++;
        }
    }

    // Détection des composantes connexes
    int *component = (int *)malloc(graph->V * sizeof(int));
    stats.connectedComponents = 0;

    // Réinitialiser visited pour le nouveau parcours
    memset(visited, 0, graph->V * sizeof(bool));

    for (int v = 0; v < graph->V; v++)
    {
        if (!visited[v])
        {
            DFSUtil(graph, v, visited, component, stats.connectedComponents);
            stats.connectedComponents++;
        }
    }
    // Cyclique
    stats.isCyclic = hasCycle(graph);

    free(ap);
    free(component);
    free(visited);
    free(disc);
    free(parent);
    free(low);

    return stats;
}