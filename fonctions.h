
#ifndef FONCTIONS_H
#define FONCTIONS_H
#include "structs.h"
#include "tickettorideapi/ticketToRide.h"

void parseTrack(Track* tableau,int* trackData, int nbTracks);
Track*** createProximityMatrix(Track* tableau, int nbTracks, int nbCities,Track* TrackZero);
void afficherMatrice(Track*** matrice, int n);
void freeMatrix(Track*** matrice, int n);
void parseHand(int Hand[], CardColor starting[]);
void addToHand(int Hand[], CardColor color);
CardColor claimableTrack(Track t, int Hand[]);
void updateClaimedTrack(Track tab[],int nbTracks,int from, int to);
void removeFromHand(int Hand[], CardColor color, int n);
bool isAnyTrackClaimable(Track t[], int Hand[],int nbTracks);
#endif //FONCTIONS_H
