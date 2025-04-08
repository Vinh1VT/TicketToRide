#include <stdlib.h>
#include <stdio.h>
#include "tickettorideapi/ticketToRide.h"
#include "fonctions.h"

#include <limits.h>

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
        tableau[i].Claimed = UNCLAIMED;
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
        Hand[starting[i] - 1] += 1;
    }
}


void addToHand(int Hand[], CardColor color){ //Hand must be an integer tab of 9 elements, each one being the number of card of the n+1 color in hand
    Hand[color-1] += 1;
}
void removeFromHand(int Hand[], CardColor color, int n){
    Hand[color-1] -= n;
}



CardColor claimableTrackWithoutLocomotives(Track t, int Hand[]){ //Hand must be an integer tab of 9 elements, each one being the number of card of the n+1 color in hand
    //returns the color with which you can claim 0 if not claimable
    if(t.Claimed == UNCLAIMED){
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

CardColor claimableTrack(Track t, int Hand[], int* locomotives){
    CardColor color = claimableTrackWithoutLocomotives(t,Hand);
    if (color == 0){
        CardColor C1 = t.Couleur1;
        CardColor C2 = t.Couleur2;
        for (int i = 0; i<Hand[8];i++){
            if (t.Longueur <= Hand[C1-1] + i){
                color = C1;
                *locomotives = i;
            }else if (t.Longueur <= Hand[C2-1]){
                color = C2;
                *locomotives = i;
            }
        }
    }
    return color;
}


void updateClaimedTrack(Track tab[],int nbTracks,unsigned int from, unsigned int to, Claim Claimer){
    for (int i = 0; i<nbTracks;i++){
        if ((tab[i].Ville1==from && tab[i].Ville2==to )||(tab[i].Ville2==from && tab[i].Ville1==to)){
            tab[i].Claimed = Claimer;
        }
    }
}


bool isAnyTrackClaimable(Track t[], int Hand[],int nbTracks){
    for (int i = 0; i<nbTracks; i++){
        if (claimableTrackWithoutLocomotives(t[i],Hand)){
            return true;
        }
    }
    return false;
}

int distanceMini(unsigned int* D, bool visited[],unsigned int N){
    unsigned int min = INT_MAX;
    int indice_min;
    for (int i = 0; i<N;i++){
        if (visited[i] == false && D[i] < min){
            min = D[i];
            indice_min = i;
        }
    }
    return indice_min;
}

void Dijkstra(unsigned int src,Track*** Matrix,unsigned int N,unsigned int* D, unsigned int* Prec){
    bool visited[N];
    int u;
    for(int i = 0;i<N;i++){
        D[i] = INT_MAX;
        visited[i] = false;
    }
    D[src] = 0;

    for (int i = 0; i < N-1;i++){
        u = distanceMini(D,visited,N);
        visited[u] = true;
        for (int v = 0; v<N;v++){
            if (visited[v] == false && Matrix[u][v] != NULL && D[u] + Matrix[u][v]->Longueur < D[v]){
                D[v] =D[u] + Matrix[u][v]->Longueur;
                Prec[v] = u;
            }
        }
    }
}


void freeMessage(MoveResult* Result){
    free(Result->message);
    free(Result->opponentMessage);
}

Objective objectiveCopy(Objective original){
    Objective copy;
    copy.score = original.score;
    copy.from = original.from;
    copy.to = original.to;
    return copy;
}

void printHand(int Hand[]){
    for (int i = 0;i<9;i++){
        printf("%d : %d \n", i+1,Hand[i]);
    }
}

void removeObjective(Objective tab[], Objective o, int objectiveCount){
    int index = 200;
    for (int i = 0; i<objectiveCount ;i++){
        if (tab[i].from == o.from && tab[i].to == o.to && tab[i].score == o.score ){
            index = i;
        }
        if (i>index){
            tab[i-1] = tab[i];
        }
    }
}

void sortObjective(Objective tab[],int objectiveCount){
    //Sort by score the objective tab, it's a bubble sort because why tf would I implement a harder one
    for (int i =0; i<objectiveCount-1;i++){
        if (tab[i+1].score<tab[i].score){
            Objective temp = tab[i];
            tab[i] = tab[i+1];
            tab[i+1] = temp;
        }
    }
}