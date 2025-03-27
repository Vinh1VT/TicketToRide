
#ifndef FONCTIONS_H
#define FONCTIONS_H
#include "tickettorideapi/ticketToRide.h"

struct Track_ {
    int Ville1;
    int Ville2;
    int Longueur;
    CardColor Couleur1;
    bool Double;
    CardColor Couleur2;
    bool Claimed;
};
typedef struct Track_ Track;

struct Objectif_{
    int Ville1;
    int Ville2;
    int Score;
};
typedef struct Objectif_ Objectif;



void parseTrack(Track* tableau,int* trackData, int nbTracks);
Track*** createProximityMatrix(Track* tableau, int nbTracks, int nbCities,Track* TrackZero);
void afficherMatrice(Track*** matrice, int n);
void freeMatrix(Track*** matrice, int n);

#endif //FONCTIONS_H
