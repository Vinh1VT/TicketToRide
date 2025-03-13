#include <stdio.h>
#include <stdlib.h>
#include "tickettorideapi/ticketToRide.h"

typedef struct{
    int Ville1;
    int Ville2;
    int Longueur;
    CardColor Couleur1;
    //bool Double;
    CardColor Couleur2;
} Track;

typedef struct Tracklist_{
    Track Track;
    struct Tracklist_* Next;
}TrackList;


TrackList* parseTrack(int* trackData, int nbTracks){
    int* p = trackData;
    TrackList* Liste = malloc(sizeof(TrackList));
    Liste->Track.Ville1 = *p;
    Liste->Track.Ville2 = *(p+1);
    Liste->Track.Longueur = *(p+2);
    Liste->Track.Couleur1 = (CardColor)*(p+3);
    Liste->Track.Couleur2 = (CardColor)*(p+4);
    p += 5;
    Liste->Next = malloc(sizeof(TrackList));
    TrackList* parcours = Liste->Next;
    for(int i = 1; i<nbTracks;i++){
        parcours->Track.Ville1 = *p;
        parcours->Track.Ville2 = *(p+1);
        parcours->Track.Longueur = *(p+2);
        parcours->Track.Couleur1 = (CardColor)*(p+3);
        parcours->Track.Couleur2 = (CardColor)*(p+4);
        p += 5;
        parcours->Next = malloc(sizeof(TrackList));
        parcours = parcours->Next;
    }
    free(parcours->Next);
    parcours->Next = NULL;
    return Liste;
}

int** createProximityMatrix(TrackList* Liste, int nbTracks, int nbCities){
    int ** matrice = malloc(sizeof(int*)*nbCities);
    for(int i =0; i<nbCities;i++){
        matrice[i] = malloc(sizeof(int)*nbCities);
    } 
    for(int i = 0; i<nbCities;i++){
        for (int j = 0;j<nbCities;j++){
            matrice[i][j] = 0;
        }
    }
    while(Liste!=NULL){
        matrice[Liste->Track.Ville1][Liste->Track.Ville2] = Liste->Track.Longueur;
        matrice[Liste->Track.Ville2][Liste->Track.Ville1] = Liste->Track.Longueur;
        Liste = Liste->Next;
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

void freeMatrix(int** Matrice, int n){
    for(int i = 0; i<n;i++){
        free(Matrice[i]);
    }
    free(Matrice);
}
void freeTrackList(TrackList* Liste){
    TrackList* suivant = Liste->Next;
    while(suivant !=NULL){
        free(Liste);
        Liste = suivant;
        suivant = Liste->Next;
    }
}

int main(int argc, char** argv){

    extern int DEBUG_LEVEL;
    DEBUG_LEVEL = INTERN_DEBUG;
    GameSettings Gsettings = GameSettingsDefaults;
    GameData Gdata = GameDataDefaults;
    Gsettings.gameType = TRAINING;
    Gsettings.botId = RANDOM_PLAYER;
    ResultCode Result;
    
    connectToCGS("cgs.valentin-lelievre.com",15001);
    sendName(argv[1]);
    sendGameSettings(Gsettings,&Gdata);
    //printBoard();
    MoveResult Mresult;
    TrackList* Liste = parseTrack(Gdata.trackData,Gdata.nbTracks);
    int** Matrice = createProximityMatrix(Liste,Gdata.nbTracks, Gdata.nbCities);
    afficherMatrice(Matrice,Gdata.nbCities);
    /*while(Liste->Next!=NULL){ //Debug de la liste chainee
        printf("%d ",Liste->Track.Ville1);
        printf("%d ",Liste->Track.Ville2);
        printf("%d ",Liste->Track.Longueur);
        printf("%d ",(int)Liste->Track.Couleur1);
        printf("%d ",(int)Liste->Track.Couleur2);

        printf("\n");
        Liste = Liste->Next;
    }*/
    /*if (Gdata.starter == 1){
        printf("send\n");
        const MoveData Mdata2 = {.action = DRAW_OBJECTIVES};
        Result = sendMove(&Mdata2,&Mresult);
        printf("0x%x\n",Result);
        printf("0x%x\n",Mresult.state);
    }
    else{
        MoveData OMdata;
        Result = getMove(&OMdata,&Mresult);
        printf("0x%x\n",Result);
        printf("0x%x\n",Mresult.state);
    }
    printBoard();*/
    quitGame();
    freeTrackList(Liste);
    freeMatrix(Matrice,Gdata.nbCities);
    free(Gdata.gameName);
    return EXIT_SUCCESS;
}