//
// Created by vinh on 28/03/25.
//
#ifndef STRUCTS_H
#define STRUCTS_H
#include "tickettorideapi/ticketToRide.h"
typedef enum{
    JOUEUR = 0,
    ADVERSAIRE = 1,
    }Tour;

typedef enum{
    UNCLAIMED = 0,
    PLAYER = 1,
    ADV = 2,
    NOT_CLAIMABLE = 3 //For virtual tracks like TrackZero
}Claim;

struct Track_ {
    unsigned int Ville1;
    unsigned int Ville2;
    unsigned int Longueur;
    CardColor Couleur1;
    bool Double;
    CardColor Couleur2;
    Claim Claimed;
    int weight;
};


typedef struct Track_ Track;



#endif //STRUCTS_H
