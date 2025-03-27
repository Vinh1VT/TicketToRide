#include <stdio.h>
#include <stdlib.h>
#include "tickettorideapi/ticketToRide.h"
#include "fonctions.h"
#include "manualPlayer.h"

int main(int const argc, char** argv){

    extern int DEBUG_LEVEL;
    DEBUG_LEVEL = INTERN_DEBUG;
    GameSettings Gsettings = GameSettingsDefaults;
    GameData Gdata = GameDataDefaults;
    Gsettings.gameType = TRAINING;
    Gsettings.botId = RANDOM_PLAYER;
    ResultCode Result;
    Track TrackZero = {.Ville1 = 0,
                        .Ville2 = 0,
                        .Longueur = 0,
                        .Claimed = true};

    connectToCGS("cgs.valentin-lelievre.com",15001);
    if (argc>1){
        sendName(argv[1]);
    }else{
        sendName("Vinh");
    }
    sendGameSettings(Gsettings,&Gdata);
    //printBoard();

    Track* tableauTrack = malloc(Gdata.nbTracks * sizeof(Track));
    parseTrack(tableauTrack,Gdata.trackData,Gdata.nbTracks);
    Track*** Matrice = createProximityMatrix(tableauTrack,Gdata.nbTracks, Gdata.nbCities,&TrackZero);
    //afficherMatrice(Matrice,Gdata.nbCities);
    manualPlay(Gdata.starter);

    quitGame();
    free(tableauTrack);
    freeMatrix(Matrice, Gdata.nbCities);
    return EXIT_SUCCESS;
}