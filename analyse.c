//
// Created by vinh on 30/05/25.
//
#include <stdio.h>
#include <stdlib.h>
#include "analyse.h"
#include "fonctions.h"
#include "structs.h"

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
