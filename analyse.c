//
// Created by vinh on 30/05/25.
//
#include <stdio.h>
#include <stdlib.h>
#include "analyse.h"
#include "fonctions.h"
#include "structs.h"

//Fichiers qui met le poids des route et la fonction de choix des objectifs au premier tour 

//Met les poids avec le nombre de chemin le plus court dans lequel la route apparait
void weightTrack(int nbCities,Track*** Matrix){
    for (int i = 0;i<nbCities;i++){
        unsigned int D[nbCities];
        int Prec[nbCities];
        Dijkstra(i,Matrix,nbCities,D,Prec);
        for (int j = i + 1; j<nbCities;j++){
            int parcours = i;
            while (Prec[parcours] != -1){
                Matrix[parcours][Prec[parcours]]->weight += 1;
                parcours = Prec[parcours];
            }
        }
    }
}

//Rajoute des poids en fonction du nombre de points qu'elle rapporte
void weightWithRoutePoint(int nbCities, Track*** Matrix){
    for (int i = 0;i<nbCities;i++){
        for (int j = i +1; j < nbCities;j++){
            if (Matrix[i][j] != NULL){
                Track* route = Matrix[i][j];
                switch (route->Longueur){
                case 1 :
                    route->weight += 1;
                    break;
                case 2 :
                    route->weight += 2;
                    break;
                case 3 :
                    route->weight += 4;
                    break;
                case 4 :
                    route->weight += 7;
                    break;
                case 5 :
                    route->weight += 10;
                    break;
                case 6 :
                    route->weight += 15;
                    break;
                default :
                    break;
                }
            }
        }
    }
}

void chooseTwoFromFloat(bool choice[3], float tab[3]) {
    for (int i = 0; i < 3; i++) {
        choice[i] = false;
    }

    int max1 = 0, max2 = 1;

    if (tab[max2] > tab[max1]) {
        int temp = max1;
        max1 = max2;
        max2 = temp;
    }

    for (int i = 2; i < 3; i++) {
        if (tab[i] > tab[max1]) {
            max2 = max1;
            max1 = i;
        } else if (tab[i] > tab[max2]) {
            max2 = i;
        }
    }
    choice[max1] = true;
    choice[max2] = true;
}


//Choisit les deux objectifs avec le plus de poids au premier tour
void weightBasedFirstTurnObjectiveChoice(bool choice[], Objective tab[], Track*** Matrix, int nbCities){
    float weightTab[3] = {0,0,0};
    float countTab[3] = {0,0,0};

    for (int i = 0; i < 3; i++){
        int Prec[nbCities];
        unsigned int D[nbCities];
        Dijkstra(tab[i].from,Matrix,nbCities,D,Prec);//On fait le dijkstra sur les objectifs, puis on choisit ceux avec le plus de poids
        Track* parcours = Matrix[tab[i].to][Prec[tab[i].to]];

        weightTab[i] += (float)parcours -> weight;
        countTab[i] += 1;

        while (Prec[parcours->Ville1] != -1 && Prec[parcours->Ville2] != -1){
            Track* temp = Matrix[parcours->Ville2][Prec[parcours->Ville2]];
            if (temp == parcours){
                temp = Matrix[parcours->Ville1][Prec[parcours->Ville1]];
            }
            parcours = temp;

            weightTab[i] += (float)parcours -> weight;
            countTab[i] += 1;
        }

        weightTab[i] = weightTab[i] / countTab[i];
    }



    chooseTwoFromFloat(choice,weightTab);
}
