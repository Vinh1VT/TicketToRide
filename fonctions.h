
#ifndef FONCTIONS_H
#define FONCTIONS_H
#include "structs.h"
#include "tickettorideapi/ticketToRide.h"

void parseTrack(Track* tableau,int* trackData, int nbTracks);
Track*** createProximityMatrix(Track* tableau, int nbTracks, int nbCities,Track* TrackZero);
void afficherMatrice(FILE* stream, Track*** matrice, int n);
void freeMatrix(Track*** matrice, int n);
void parseHand(int Hand[], CardColor starting[]);
void addToHand(int Hand[], CardColor color);
CardColor claimableTrackWithoutLocomotives(Track t, int Hand[]);
void updateClaimedTrack(Track tab[],int nbTracks,unsigned int from, unsigned int to, Claim Claimer);
void removeFromHand(int Hand[], CardColor color, int n);
bool isAnyTrackClaimable(Track t[], int Hand[],int nbTracks);
void Dijkstra(unsigned int src,Track*** Matrix,unsigned int N,unsigned int* D, int* Prec);
void freeMessage(MoveResult* Result);
Objective objectiveCopy(Objective original);
void printHand(int Hand[]);
void removeObjective(Objective tab[], Objective o, int objectiveCount);
void sortObjective(Objective tab[],int objectiveCount);
CardColor claimableTrack(Track t, int Hand[], int* locomotives);
CardColor maxHand(int Hand[]);
int nbCardInHand(int Hand[]);
unsigned int max(unsigned int a,  unsigned int b, unsigned int c);
void objectiveChoice(int choice[],Objective newObjectives[],Track*** Matrix, int nbCities, int* objectiveCount,int* objectiveDeck);
#endif //FONCTIONS_H
