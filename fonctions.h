
#ifndef FONCTIONS_H
#define FONCTIONS_H
#include "structs.h"
#include "tickettorideapi/ticketToRide.h"

void parseTrack(Track* tableau,int* trackData, int nbTracks);
Track*** createProximityMatrix(Track* tableau, int nbTracks, int nbCities,Track* TrackZero);
void afficherMatrice(Track*** matrice, int n);
void freeMatrix(Track*** matrice, int n);
void parseHand(int Hand[], CardColor starting[]);
void updateHand(int Hand[], CardColor color);
CardColor claimableTrack(Track t, int Hand[]);
void updateClaimedTrack(Track tab[],int nbTracks,int from, int to);

#endif //FONCTIONS_H
