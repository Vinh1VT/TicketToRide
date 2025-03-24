#include <stdio.h>
#include <stdlib.h>
#include "tickettorideapi/ticketToRide.h"

typedef struct Track_ {
    int Ville1;
    int Ville2;
    int Longueur;
    CardColor Couleur1;
    bool Double;
    CardColor Couleur2;
    bool Claimed;
} Track;

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

int main(int argc, char** argv){

    extern int DEBUG_LEVEL;
    DEBUG_LEVEL = INTERN_DEBUG;
    GameSettings Gsettings = GameSettingsDefaults;
    GameData Gdata = GameDataDefaults;
    Gsettings.gameType = TRAINING;
    Gsettings.botId = RANDOM_PLAYER;
    ResultCode Result;
    

    connectToCGS("cgs.valentin-lelievre.com",15001);
    if (argc>1){
        sendName(argv[1]);
    }else{
        sendName("Vinh");
    }
    sendGameSettings(Gsettings,&Gdata);
    printBoard();
    MoveResult Mresult;
    MoveData Mdata;
    Track* tableauTrack = malloc(Gdata.nbTracks * sizeof(Track));
    parseTrack(tableauTrack,Gdata.trackData,Gdata.nbTracks);
    int** Matrice = createProximityMatrix(tableauTrack,Gdata.nbTracks, Gdata.nbCities);
    //afficherMatrice(Matrice,Gdata.nbCities);


    /*if (Gdata.starter == 1){
        getMove(&Mdata, &Mresult);
        getMove(&Mdata, &Mresult);
    }
    else{
        Mdata.action = DRAW_OBJECTIVES;
        sendMove(&Mdata,&Mresult);
        MoveData Mdata2;
        Mdata2.action = CHOOSE_OBJECTIVES;
        Mdata2.chooseObjectives[0] = true;
        Mdata2.chooseObjectives[1] = false;
        Mdata2.chooseObjectives[2] = false;
        sendMove(&Mdata,&Mresult);
        getMove(&Mdata,&Mresult);
        getMove(&Mdata,&Mresult);

    }*/
    printBoard();
    quitGame();
    free(tableauTrack);
    freeMatrix(Matrice, Gdata.nbCities);
    return EXIT_SUCCESS;
}