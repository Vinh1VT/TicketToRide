#include <stdio.h>
#include <stdlib.h>
#include "tickettorideapi/ticketToRide.h"
#include "structs.h"
#include "fonctions.h"
#include "manualPlayer.h"
#include "randomBot.h"

int main(int const argc, char** argv){

    extern int DEBUG_LEVEL;
    DEBUG_LEVEL = INTERN_DEBUG;
    GameSettings Gsettings = GameSettingsDefaults;
    GameData Gdata = GameDataDefaults;
    Gsettings.gameType = TRAINING;
    Gsettings.botId = RANDOM_PLAYER;
    int Hand[9];
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
    parseHand(Hand,Gdata.cards);
    Track* tableauTrack = malloc(Gdata.nbTracks * sizeof(Track));
    parseTrack(tableauTrack,Gdata.trackData,Gdata.nbTracks);
    Track*** Matrice = createProximityMatrix(tableauTrack,Gdata.nbTracks, Gdata.nbCities,&TrackZero);
    //afficherMatrice(Matrice,Gdata.nbCities);
    //manualPlay(Gdata.starter);
    randomPlay(Gdata.starter,tableauTrack,Gdata.nbTracks,Hand);



    quitGame();
    free(tableauTrack);
    freeMatrix(Matrice, Gdata.nbCities);
    return EXIT_SUCCESS;
}