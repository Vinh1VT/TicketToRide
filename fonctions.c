#include <stdlib.h>
#include <stdio.h>
#include "tickettorideapi/ticketToRide.h"
#include "fonctions.h"
void parseTrack(Track* tableau,int* trackData, int nbTracks){
    int* p = trackData;
    for(int i =0; i<nbTracks;i++){
        tableau[i].Ville1 = *p;
        tableau[i].Ville2 = *(p+1);
        tableau[i].Longueur = *(p+2);
        tableau[i].Couleur1 = (CardColor)*(p+3);
        tableau[i].Couleur2 = (CardColor)*(p+4);
        if(tableau[i].Couleur2){
            tableau[i].Double = true;
        }
        tableau[i].Claimed = false;
        p = p + 5;
    }
}

int** createProximityMatrix(Track* tableau, int nbTracks, int nbCities){
    int ** matrice = malloc(sizeof(int*)*nbCities);
    for(int i =0; i<nbCities;i++){
        matrice[i] = malloc(sizeof(int)*nbCities);
    }
    for(int i = 0; i<nbCities;i++){
        for (int j = 0;j<nbCities;j++){
            matrice[i][j] = 0;
        }
    }
    for(int i = 0; i<nbTracks;i++){
        matrice[tableau[i].Ville1][tableau[i].Ville2] = tableau[i].Longueur;
        matrice[tableau[i].Ville2][tableau[i].Ville1] = tableau[i].Longueur;
    }

    return matrice;
}


void afficherMatrice(int** matrice, int n){
    for(int ligne = 0; ligne<n;ligne++){
        printf("%d : ",ligne);
        for(int colonne=0;colonne<n;colonne++){
            printf("%d",matrice[ligne][colonne]);
        }
        printf("\n");
    }


}
void freeMatrix(int** matrice, int n){
    for (int i = 0; i<n;i++){
        free(matrice[i]);
    }
    free(matrice);
}