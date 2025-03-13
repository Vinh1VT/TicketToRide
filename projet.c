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
} Track;

void parseTrack(Track* tableau,int* trackData, int nbTracks){
    int* p = trackData;
    for(int i =0; i<nbTracks;i++){
        tableau[i].Ville1 = *p;
        tableau[i].Ville2 = *(p+1);
        tableau[i].Longueur = *(p+3);
        tableau[i].Couleur1 = (CardColor)*(p+4);
        tableau[i].Couleur2 = (CardColor)*(p+5);
        if(tableau[i].Couleur2){
            tableau[i].Double = true;
        }
        p = p + 5;
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
    printBoard();
    MoveResult Mresult;
    Track* tableauTrack = malloc(Gdata.nbTracks * sizeof(Track));
    parseTrack(tableauTrack,Gdata.trackData,Gdata.nbTracks);

    
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
    free(tableauTrack);


    return EXIT_SUCCESS;
}