#include "../graph/graph.h"
#include <time.h>
#include <math.h>

// Déterminer la période de la journée
TimePeriod getCurrentTimePeriod()
{
    time_t now = time(NULL);
    struct tm *tm = localtime(&now);
    int hour = tm->tm_hour;

    if (hour >= 7 && hour <= 9)
        return HEURE_POINTE; // Matin
    if (hour >= 16 && hour <= 19)
        return HEURE_POINTE; // Soir
    if (hour >= 22 || hour <= 5)
        return NUIT;     // Nuit
    return HEURE_CREUSE; // Heure creuse
}

// Déterminer la saison actuelle (simplifié)
Season getCurrentSeason()
{
    time_t now = time(NULL);
    struct tm *tm = localtime(&now);
    int month = tm->tm_mon + 1;

    // Exemple pour l'Afrique de l'Ouest:
    // Saison pluvieuse d'avril à octobre
    if (month >= 4 && month <= 10)
        return SAISON_PLUVIEUSE;
    return SAISON_SECHE;
}

// Obtenir les attributs actuels en fonction du temps
EdgeAttr getCurrentEdgeAttributes(EdgeAttr baseAttr)
{
    TimePeriod period = getCurrentTimePeriod();
    Season season = getCurrentSeason();
    EdgeAttr current = baseAttr;

    // Appliquer les multiplicateurs temporels
    float timeMultiplier = baseAttr.temporal.timeOfDayMultipliers[period];
    float seasonMultiplier = baseAttr.temporal.seasonMultipliers[season];

    current.baseTime *= timeMultiplier;
    current.cost *= timeMultiplier;
    current.reliability *= seasonMultiplier;

    // Vérifier la disponibilité saisonnière
    if (!baseAttr.temporal.seasonalAvailability[season])
    {
        current.baseTime = INFINITY; // Rend le chemin indisponible
    }

    return current;
}

// Initialiser les variations temporelles par défaut
void initDefaultTemporalVariation(TemporalVariation *tv)
{
    // Valeurs par défaut (peuvent être modifiées)
    tv->timeOfDayMultipliers[HEURE_CREUSE] = 1.0f;
    tv->timeOfDayMultipliers[HEURE_POINTE] = 1.5f; // +50% de temps en heure de pointe
    tv->timeOfDayMultipliers[NUIT] = 0.9f;         // -10% la nuit

    tv->seasonMultipliers[SAISON_SECHE] = 1.0f;
    tv->seasonMultipliers[SAISON_PLUVIEUSE] = 0.7f; // Fiabilité réduite en saison pluvieuse

    tv->seasonalAvailability[SAISON_SECHE] = true;
    tv->seasonalAvailability[SAISON_PLUVIEUSE] = true; // Disponible par défaut toute l'année
}

void setTimeVariation(EdgeAttr *attr, TimePeriod period, float timeMultiplier)
{
    if (period >= HEURE_CREUSE && period <= NUIT)
    {
        attr->temporal.timeOfDayMultipliers[period] = timeMultiplier;
    }
}

void setSeasonVariation(EdgeAttr *attr, Season season, float reliabilityMultiplier, bool available)
{
    if (season >= SAISON_SECHE && season <= SAISON_PLUVIEUSE)
    {
        attr->temporal.seasonMultipliers[season] = reliabilityMultiplier;
        attr->temporal.seasonalAvailability[season] = available;
    }
}

void printCurrentTimeInfo()
{
    TimePeriod period = getCurrentTimePeriod();
    Season season = getCurrentSeason();

    const char *periodNames[] = {"Heure creuse", "Heure de pointe", "Nuit"};
    const char *seasonNames[] = {"Saison sèche", "Saison pluvieuse"};

    printf("\nConditions actuelles:\n");
    printf("- Periode: %s\n", periodNames[period]);
    printf("- Saison: %s\n", seasonNames[season]);
}