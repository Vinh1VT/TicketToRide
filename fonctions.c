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

Track*** createProximityMatrix(Track* tableau, int nbTracks, int nbCities, Track* TrackZero){
    Track *** matrice = malloc(sizeof(Track**)*nbCities);
    for(int i =0; i<nbCities;i++){
        matrice[i] = malloc(sizeof(Track**)*nbCities);
    }
    for(int i = 0; i<nbCities;i++){
        for (int j = 0;j<nbCities;j++){
            if(i==j){
              matrice[i][j] = TrackZero;
            }else{
          		matrice[i][j] = NULL;
            }
        }
    }
    for(int i = 0; i <nbTracks;i++){
		matrice[tableau[i].Ville1][tableau[i].Ville2] = &tableau[i];
    	matrice[tableau[i].Ville2][tableau[i].Ville1] = &tableau[i];
    }

    return matrice;
}


void afficherMatrice(Track*** matrice, int n){
    for(int ligne = 0; ligne<n;ligne++){
        printf("%d : ",ligne);
        for(int colonne=0;colonne<n;colonne++){
          if(matrice[ligne][colonne] == NULL){
          	printf("x ");
          }else{
            printf("%d ",matrice[ligne][colonne]->Longueur);
          }
        }
        printf("\n");
    }


}
void freeMatrix(Track*** matrice, int n){
    for (int i = 0; i<n;i++){
        free(matrice[i]);
    }
    free(matrice);
}
