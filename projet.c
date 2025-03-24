#include <stdio.h>
#include <stdlib.h>
#include "tickettorideapi/ticketToRide.h"
#include "fonctions.h"

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