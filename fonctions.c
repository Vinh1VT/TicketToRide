#include <stdlib.h>
#include <stdio.h>
#include "tickettorideapi/ticketToRide.h"
#include "fonctions.h"

#include <limits.h>

    //Remplit la structure Track pour en faire un tableau avec les bons champs
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
        tableau[i].weight = 0;
        p = p + 5;
    }
}

    //Crée une matrice contenant un pointeur de la route entre i et j (les dimensions de la matrice)
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


    //Affiche la matrice (utile pour du debug)
void printMatrix(FILE* stream, Track*** matrix, int n){
    for(int ligne = 0; ligne<n;ligne++){
        printf("%d : ",ligne);
        for(int colonne=0;colonne<n;colonne++){
          if(matrix[ligne][colonne] == NULL){
          	fprintf(stream,"x ");
          }else{
            fprintf(stream,"%p ",matrix[ligne][colonne]);
          }
        }
        printf("\n");
    }


}
    //Free la matrice
void freeMatrix(Track*** matrice, int n){
    for (int i = 0; i<n;i++){
        free(matrice[i]);
    }
    free(matrice);
}

    //Parse la main au début du jeu, en un tableau de 9 entiers
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


    //Renvoie la couleur avec laquelle on peut prendre une route si c'est le cas, None sinon
CardColor claimableTrackWithoutLocomotives(Track t, int Hand[], int wagon){ //Hand must be an integer tab of 9 elements, each one being the number of card of the n+1 color in hand
    //returns the color with which you can claim 0 if not claimable
    if(t.Claimed == UNCLAIMED){
        if (t.Couleur1 == LOCOMOTIVE){
            for (int i = 1; i<9;i++){
                if (t.Longueur<=Hand[i-1]){
                    return i;
                }
            }
        } else{
            if (t.Longueur <= Hand[t.Couleur1-1] && wagon >= t.Longueur ){
                return t.Couleur1;
            }else if (t.Couleur2 != NONE && t.Longueur <= Hand[t.Couleur2-1] && wagon>= t.Longueur){
                return t.Couleur2;
            }
        }
    }
    return NONE;
}
    //Renvoie la couleur avec laquelle on peut prendre une route si c'est le cas, et le nombre de locomotives avec lequel c'est possible. None sinon
CardColor claimableTrack(Track t, int Hand[], int* locomotives, int wagon){
    CardColor color = claimableTrackWithoutLocomotives(t,Hand,wagon);
    if (t.Claimed == UNCLAIMED && color == NONE){
        CardColor C1 = t.Couleur1;
        CardColor C2 = t.Couleur2;
        if (C1 == LOCOMOTIVE){
            for (int i = 1; i<9 ; i++){
                if (Hand[8] >= t.Longueur - Hand[i-1]){
                    color = i;
                    *locomotives = t.Longueur - Hand[i-1];
                    break;
                }
            }
        }else if (Hand[8] >= t.Longueur - Hand[C1-1]){
            color = C1;
            *locomotives = t.Longueur - Hand[C1-1];
        }else if (C2 != NONE && Hand[8]>= t.Longueur - Hand[C2-1]){
            color = C2;
            *locomotives = t.Longueur - Hand[C2-1];
        }else{
            color = NONE;
        }
    }
    if (t.Longueur > wagon){
        return NONE;
    }
    return color;
}

    //Update les route qui ont été prises (plus utilisé)
void updateClaimedTrack(Track tab[],int nbTracks,unsigned int from, unsigned int to, Claim Claimer){
    for (int i = 0; i<nbTracks;i++){
        if ((tab[i].Ville1==from && tab[i].Ville2==to )||(tab[i].Ville2==from && tab[i].Ville1==to)){
            tab[i].Claimed = Claimer;
        }
    }
}

    //Regarde si une route dans le plateau (n'importe laquelle) peut être prise, utilisé par le random bot
bool isAnyTrackClaimable(Track t[], int Hand[], int nbTracks, int wagon){
    for (int i = 0; i<nbTracks; i++){
        if (claimableTrackWithoutLocomotives(t[i],Hand,wagon)){
            return true;
        }
    }
    return false;
}
    //Fonction de distance mini de Dijkstra
int distanceMini(unsigned int* D, bool visited[], unsigned int N){
    unsigned int min = INT_MAX;
    int indice_min = -1;
    for (int i = 0; i<N;i++){
        if (visited[i] == false && D[i] < min){
            min = D[i];
            indice_min = i;
        }
    }
    return indice_min;
}
    //Dijkstra, renvoie la distance entre src et les autres villes dans D, et la ville précédente pour y arriver dans prec
void Dijkstra(unsigned int src,Track*** Matrix,unsigned int N,unsigned int* D, int* Prec){
    bool visited[N];
    for (int i = 0; i<N;i++){//Reset the Disjktra algorithm, IDK if it will fix bugs and infinite cycles, but I sure hope so
        D[i] = INT_MAX;
        Prec[i] = -1;
        visited[i] = false;
    }
    D[src] = 0;
    for (int i = 0; i < N-1;i++){
        int u = distanceMini(D, visited, N);
        if (u!=-1){
            visited[u] = true;
            for (int v = 0; v<N;v++){
                if ((D[u] != INT_MAX) && (visited[v] == false) && (Matrix[u][v] != NULL) && (D[u] + Matrix[u][v]->Longueur < D[v])){
                    D[v] = D[u] + Matrix[u][v]->Longueur;
                    Prec[v] = u;
                }
            }
        }
    }
}

    //Free les messages du serveur
void freeMessage(MoveResult* Result){
    free(Result->message);
    free(Result->opponentMessage);
}

    //Copie un objectif, pour pas garder les pointeurs dans Result qui sont perdus lorsque Result est édité
Objective objectiveCopy(Objective original){
    Objective copy;
    copy.score = original.score;
    copy.from = original.from;
    copy.to = original.to;
    return copy;
}
    //Affiche la main, pour du debug
void printHand(int Hand[]){
    for (int i = 0;i<9;i++){
        printf("%d : %d \n", i+1,Hand[i]);
    }
}

    //Sort by score the objective tab, it's a bubble sort because why tf would I implement a harder one
void sortObjective(Objective tab[],int objectiveCount){
    for (int i =0; i<objectiveCount-1;i++){
        if (tab[i+1].score<tab[i].score){
            Objective temp = tab[i];
            tab[i] = tab[i+1];
            tab[i+1] = temp;
        }
    }
}

    //Renvoie la couleur la plus présente en main
CardColor maxHand(int Hand[]){
    int max = 0;
    for (int i = 0; i<8 ; i++){
        if (Hand[i]>max){
            max = i + 1;
        }
    }
    return max;
}

    //Renvoie le nombre total de cartes en main
int totalCardsInHand(int Hand[]){
    int total = 0;
    for (int i  = 0; i<9;i++){
        total += Hand[i];
    }
    return total;
}

    //returns the max out of 3 numbers, useful for the choice function
unsigned int unsignedMax(unsigned int a,  unsigned int b, unsigned int c){
    unsigned int maxi = a;
    if (b>maxi) {
        maxi = b;
    }
    if (c>maxi) {
        maxi = c;
    }
    return maxi;
}

    //Pareil que la fonction précédente mais en float
float floatMax(float a, float b, float c){
    float maxi = a;
    if (b>maxi) {
        maxi = b;
    }
    if (c>maxi) {
        maxi = c;
    }
    return maxi;
}

    //Renvoie la distance entre 2 villes, d'apres Dijkstra
unsigned int distance(Objective obj,Track*** Matrix,int nbCities){
    unsigned int D[nbCities];
    int Prec[nbCities];

    Dijkstra(obj.from,Matrix,nbCities,D,Prec);

    return D[obj.to];
}

    //Renvoie l'index de la valeur minimum dans un tableau d'entiers
unsigned int minIndex(unsigned int a, unsigned int b, unsigned int  c){
    unsigned int mini = a;
    if (mini>b){
        mini = b;
    }
    if (mini > c){
        mini = c;
    }

    if (mini == a ){
        return 0;
    }else if (mini == b){
        return 1;
    }
    return 2;
}

    //Choisit les objecitfs (a partir du Rate)
void objectiveChoice(int choice[], Objective newObjectives[], Track*** Matrix, int nbCities, int* objectiveCount, int* objectiveDeck, int
                     wagon){ //always chooses already completed objectives (free points), and chooses the most valuable one in score
    float rateTab[3] = {objectiveRate(newObjectives[0],Matrix,nbCities, wagon),objectiveRate(newObjectives[1],Matrix,nbCities, wagon),objectiveRate(newObjectives[2],Matrix,nbCities, wagon)};
    bool chosen = false;
    for (int i =0;i<3;i++){
        if (rateTab[i] == INT_MAX){
            choice[i] = 2;
            *objectiveDeck -=1;
            chosen = true;
            rateTab[i] = -1;
        }
    }

    float maxRate = floatMax(rateTab[0],rateTab[1],rateTab[2]);
    if (maxRate==0){
        if (chosen){
            return;
        }else{
            choice[0] = 1; //if we must choose an objective, chooses the first one (to be upgraded)
            *objectiveDeck-=1;
            *objectiveCount+=1;
            return;
        }
    }

    for (int i = 0; i<3;i++){
        if (rateTab[i] == maxRate ){
            choice[i] = 1;
            *objectiveDeck-=1;
            *objectiveCount+=1;
            break;
        }
    }

}

void printRoutesAddress(FILE* stream,int src, int dest, int Prec[],Track*** Matrix){ //This function was implemented strictly for debug,
    //bcs sometimes nextTrack decided his address should be 0x131
    int v = dest;
    while (v!=src){
        fprintf(stream,"%d->%d\n",v,Prec[v]);
        Track* track = Matrix[v][Prec[v]];
        fprintf(stream,"%p\n",track);
        v = Prec[v];
    }
}
    //Renvoie la "note" d'un objectif (a quel point il rapporte des points par rapport à la distance parcourue)
float objectiveRate(Objective objective, Track*** Matrix, int nbCities, int wagon){
    unsigned int objDistance = distance(objective,Matrix,nbCities);
    if (objDistance > wagon){ //On ne prend pas si on ne peut pas faire l'objectif
        return 0;
    }
    if (objDistance == 0){ //On prend tout de suite si l'objectif est déjà réalisé
        return INT_MAX;
    }

    unsigned int D[nbCities];
    int Prec[nbCities];
    Dijkstra(objective.from,Matrix,nbCities,D,Prec);

    Track* Parcours = Matrix[objective.to][Prec[objective.to]];
    float note = (float) objective.score + (float) Parcours->weight;
    while (Prec[Parcours->Ville1] != -1 && Prec[Parcours->Ville2] != -1){

        Track * temp = Matrix[Parcours->Ville1][Prec[Parcours->Ville1]];
        if (temp == Parcours){
            temp = Matrix[Parcours->Ville2][Prec[Parcours->Ville2]];
        }
        Parcours = temp;

        note += (float)Parcours->weight;
    }


    return note / (float)objDistance;
}