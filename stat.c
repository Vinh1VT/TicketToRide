//
// Created by vinh on 18/04/25.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "firstBot.h"
#include "fonctions.h"
#include "structs.h"
#include "tickettorideapi/ticketToRide.h"

#define REPETITIONS 50

int main(int const argc, char** argv){

    extern int DEBUG_LEVEL;
    DEBUG_LEVEL = INTERN_DEBUG;
    srand(time(NULL)^getpid());

    GameData Gdata;
    int Hand[9];

    Track TrackZero = {.Ville1 = 0,
                        .Ville2 = 0,
                        .Longueur = 0,
                        .Claimed = NOT_CLAIMABLE};

    for (int i = 0; i<REPETITIONS;i++){
        char Gsettings[100];
        int n = rand() % 1000000;
        sprintf(Gsettings,"TRAINING NICE_BOT seed=%d start=0",n);

        char fileName[100] = "LogSeedCrash/";
        strcat(fileName,argv[1]);
        FILE* f = fopen(fileName,"w");
        fprintf(f,"Seed : %d Round : %d\n",n,i);
        fclose(f);

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
