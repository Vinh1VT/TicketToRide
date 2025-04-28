//
// Created by vinh on 18/04/25.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "firstBot.h"
#include "fonctions.h"
#include "structs.h"
#include "tickettorideapi/ticketToRide.h"

#define REPETITIONS 50

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

    FILE* tour = fopen("tour","w");
    for (int i = 0; i<REPETITIONS;i++){
        /*FILE* f = fopen("objective.txt","w");
        fclose(f);//empties the log objective file, for debug purposes*/
        sleep(1);
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
        fprintf(tour,"%d\n",i+1);
    }
    fclose(tour);
    return EXIT_SUCCESS;
}
