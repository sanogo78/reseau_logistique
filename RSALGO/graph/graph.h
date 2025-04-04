#ifndef GRAPH_H
#define GRAPH_H
#include <stdbool.h>

// Définition des couleurs pour le DFS
typedef enum
{
    WHITE, // Non visité
    GRAY,  // En cours de visite
    BLACK  // Visité
} NodeColor;

// Définition des périodes temporelles
typedef enum
{
    HEURE_CREUSE,
    HEURE_POINTE,
    NUIT
} TimePeriod;

typedef enum
{
    SAISON_SECHE,
    SAISON_PLUVIEUSE
} Season;

// Structure pour les variations temporelles
typedef struct
{
    float timeOfDayMultipliers[3]; // Multiplicateurs pour [HEURE_CREUSE, HEURE_POINTE, NUIT]
    float seasonMultipliers[2];    // Multiplicateurs pour [SAISON_SECHE, SAISON_PLUVIEUSE]
    bool seasonalAvailability[2];  // Disponibilité saisonnière [SAISON_SECHE, SAISON_PLUVIEUSE]
} TemporalVariation;

// Structure pour les attributs d'une arête
typedef struct EdgeAttr
{
    float distance;             // Distance en kilométres
    float baseTime;             // Temps de parcours de base (en minutes)
    float cost;                 // Cout du transport (en monnaie locale)
    int roadType;               // Type de route (0: asphalte, 1: latérite, etc.)
    float reliability;          // Fiabilité de la route (valeur entre 0 et 1)
    int restrictions;           // Restrictions de la route (en bits, ex. poids, hauteur, largeur)
    TemporalVariation temporal; // Variations temporelles
} EdgeAttr;

// Structure pour les informations sur un noeud
typedef struct Node
{
    int id;               // Identifiant unique du noeud
    char *name;           // Nom du noeud (ex. "Hub Abidjan")
    char *type;           // Type du noeud (ex. "hub")
    float coordinates[2]; // Coordonnees [latitude, longitude]
    int capacity;         // Capacite du noeud (ex. capacité de traitement, stockage)
} Node;

// Structure pour un noeud de la liste d'adjacence
typedef struct AdjListNode
{
    int dest;                 // Identifiant du noeud destination
    EdgeAttr attr;            // Attributs de l"arête (distance, temps, etc.)
    struct AdjListNode *next; // Pointeur vers le prochain noeud dans la liste
} AdjListNode;

// Structure pour la liste d'adjacence
typedef struct AdjList
{
    AdjListNode *head; // Tête de la liste d'adjacence
} AdjList;

// Structure pour le graphe
typedef struct Graph
{
    int V;          // Nombre de sommets
    AdjList *array; // Tableau de listes d'adjacence
    Node *nodes;    // Tableau de noeuds avec informations détaillées
} Graph;

// Fonctions de base du graphe
Graph *createGraph(int V);
void addNode(Graph *graph, int nodeIndex, int id, const char *name, const char *type,
             float lat, float lon, int capacity);
void addEdge(Graph *graph, int src, int dest, EdgeAttr attr);
void removeEdge(Graph *graph, int src, int dest);
void removeNode(Graph *graph, int nodeIndex);
void printNodeInfo(Node node);
void printGraph(Graph *graph);
void printAdjList(Graph *graph);
// void freeGraph(Graph *graph);

#endif