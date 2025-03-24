#include "tickettorideapi/ticketToRide.h"

#ifndef FONCTIONS_H
#define FONCTIONS_H


typedef struct Track_ {
    int Ville1;
    int Ville2;
    int Longueur;
    CardColor Couleur1;
    bool Double;
    CardColor Couleur2;
    bool Claimed;
} Track;

void parseTrack(Track* tableau,int* trackData, int nbTracks);
int** createProximityMatrix(Track* tableau, int nbTracks, int nbCities);
void afficherMatrice(int** matrice, int n);
void freeMatrix(int** matrice, int n);

#endif //FONCTIONS_H
