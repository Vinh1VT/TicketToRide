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


#endif //STRUCTS_H
