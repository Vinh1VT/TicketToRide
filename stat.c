//
// Created by vinh on 18/04/25.
//
#include <stdio.h>
#include <stdlib.h>

#include "firstBot.h"
#include "fonctions.h"
#include "structs.h"
#include "tickettorideapi/ticketToRide.h"


int main(int const argc, char** argv){

    extern int DEBUG_LEVEL;
    DEBUG_LEVEL = INTERN_DEBUG;
    char* Gsettings = "TRAINING NICE_BOT map=USA";
    GameData Gdata;
    int Hand[9];
    Track TrackZero = {.Ville1 = 0,
                        .Ville2 = 0,
                        .Longueur = 0,
                        .Claimed = NOT_CLAIMABLE};

    for (int i = 0; i<10;i++){
        connectToCGS("82.29.170.160",15001,argv[1]);
        sendGameSettings(Gsettings,&Gdata);
        parseHand(Hand,Gdata.cards);
        Track tableauTrack[Gdata.nbTracks];
        parseTrack(tableauTrack,Gdata.trackData,Gdata.nbTracks);
        Track*** Matrice = createProximityMatrix(tableauTrack,Gdata.nbTracks, Gdata.nbCities,&TrackZero);
        firstBotPlay(Gdata.starter,Hand, Matrice,Gdata.nbCities);
        quitGame();
        free(Gdata.gameName);
        free(Gdata.trackData);
        freeMatrix(Matrice, Gdata.nbCities);
    }
    return EXIT_SUCCESS;
}
