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

void parseHand(int Hand[], CardColor starting[]){
    for(int i = 0; i<9; i++){
        Hand[i] =  0;
    }
    for(int i = 0; i<4; i++){
        Hand[starting[i] - 1] = 1;
    }
}


void addToHand(int Hand[], CardColor color){ //Hand must be an integer tab of 9 elements, each one being the number of card of the n+1 color in hand
    Hand[color-1] += 1;
}
void removeFromHand(int Hand[], CardColor color, int n){
    Hand[color-1] -= n;
}



//For the record, this will need to be updated later
CardColor claimableTrack(Track t, int Hand[]){ //Hand must be an integer tab of 9 elements, each one being the number of card of the n+1 color in hand
    //returns the color with which you can claim 0 if not claimable
    if(!t.Claimed){
        if (t.Couleur1 == LOCOMOTIVE){
            printf("TCHOU TCHOU \n");
            for (int i = 0; i<9;i++){
                if (t.Longueur<=Hand[i]){
                    return i+1;
                }
            }
        } else{
            if (t.Longueur <= Hand[t.Couleur1-1] ){
                return t.Couleur1;
            }else if (t.Double && t.Longueur <= Hand[t.Couleur2-1]){
                return t.Couleur2;
            }
        }
    }
    return 0;
}

void updateClaimedTrack(Track tab[],int nbTracks,int from, int to){
    for (int i = 0; i<nbTracks;i++){
        if ((tab[i].Ville1==from && tab[i].Ville2==to )||(tab[i].Ville2==from && tab[i].Ville1==to)){
            tab[i].Claimed = true;
        }
    }
}


bool isAnyTrackClaimable(Track t[], int Hand[],int nbTracks){
    for (int i = 0; i<nbTracks; i++){
        if (claimableTrack(t[i],Hand)){
            return true;
        }
    }
    return false;
}
