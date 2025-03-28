//
// Created by vinh on 28/03/25.
//
//First Bot attempt, will play randomly any move he can play
#include <stdlib.h>
#include <stdio.h>
#include "randomBot.h"
#include "structs.h"
#include "tickettorideapi/ticketToRide.h"
#include <time.h>
#include "fonctions.h"


bool isAnyTrackClaimable(Track t[], int Hand[],int nbTracks){
    for (int i = 0; i<nbTracks; i++){
        if (claimableTrack(t[i],Hand)){
            return true;
        }
    }
    return false;
}


ResultCode firstTurnRandomBot(int starter){ //First turn for the random bot, the bot doesn't care about his objectives so no need to store anything there
    Tour t = starter;
    MoveResult* Result = malloc(sizeof(MoveResult));
    MoveData* Data = malloc(sizeof(MoveData));
    ResultCode Code;
    bool choix[3] = {1,0,1}; //Arbitrary choice of the kept objectives, the bot doesn't care anyway

    if (t == ADVERSAIRE){
        Code = getMove(Data,Result);
        if (Code!=ALL_GOOD){
            return Code;
        }
        Code = getMove(Data,Result);
        if (Code!=ALL_GOOD){
            return Code;
        }
        Data -> action = DRAW_OBJECTIVES;
        Code = sendMove(Data,Result);
        if (Code != ALL_GOOD){
            return Code;
        }
        Data -> action = CHOOSE_OBJECTIVES;
        Data -> chooseObjectives[0] = choix[0];
        Data -> chooseObjectives[1] = choix[1];
        Data -> chooseObjectives[2] = choix[2];
        Code = sendMove(Data,Result);
    }else{
        Data -> action = DRAW_OBJECTIVES;
        Code = sendMove(Data,Result);
        if (Code != ALL_GOOD){
            return Code;
        }
        Data -> action = CHOOSE_OBJECTIVES;
        Data -> chooseObjectives[0] = choix[0];
        Data -> chooseObjectives[1] = choix[1];
        Data -> chooseObjectives[2] = choix[2];
        Code = sendMove(Data,Result);
        if (Code != ALL_GOOD){
            return Code;
        }

        Code = getMove(Data,Result);
        if (Code!=ALL_GOOD){
            return Code;
        }
        Code = getMove(Data,Result);
    }

    free(Data);
    free(Result);
    return Code;
}

ResultCode piocheObjectifRandom(MoveResult* Result){
    MoveData* Data = malloc(sizeof(MoveData));
    bool choix[3] = {1,1,0};
    ResultCode Code;

    Data -> action = DRAW_OBJECTIVES;
    Code = sendMove(Data,Result);
    if (Code != ALL_GOOD){
        return Code;
    }
    Data -> action = CHOOSE_OBJECTIVES;
    Data -> chooseObjectives[0] = choix[0];
    Data -> chooseObjectives[1] = choix[1];
    Data -> chooseObjectives[2] = choix[2];
    Code = sendMove(Data,Result);

    free(Data);
    return Code;
}

ResultCode piocheCarteRandom(MoveResult* Result,int Hand[]){
    //Always draws 2 cards not visible (visible cards would make the code harder for nothing since it would be essentially the same results)
    MoveData* Data = malloc(sizeof(MoveData));
    ResultCode Code;
    Data -> action = DRAW_BLIND_CARD;
    Code = sendMove(Data,Result);
    updateHand(Hand,Result->card);
    if (Code != ALL_GOOD){
        return Code;
    }
    Data -> action = DRAW_BLIND_CARD;
    Code = sendMove(Data,Result);
    updateHand(Hand,Result->card);

    return Code;
}

ResultCode placeTrackRandom(MoveResult* Result, Track tab[], int nbTracks, int Hand[]){
    //Claim the first claimable track
    //The bot cannot use locomotives
    MoveData* Data = malloc(sizeof(MoveData));
    Data -> action = CLAIM_ROUTE;
    ClaimRouteMove Move;
    for (int i =0; i < nbTracks; i++){
        CardColor color  = claimableTrack(tab[i],Hand);
        if (color){
            Move.nbLocomotives = 0;
            Move.color = color;
            Move.from = tab[i].Ville1;
            Move.to = tab[i].Ville2;
            Data->claimRoute = Move;
            ResultCode Code = sendMove(Data,Result);
            updateClaimedTrack(tab,nbTracks,Data->claimRoute.from,Data->claimRoute.to);
            free(Data);
            return Code;
        }
    }

    return LOSING_MOVE;
}

void randomPlay(int starter, Track tab[], int nbTracks, int Hand[]){
    srand(time(NULL));
    Tour t = starter;
    MoveResult* Result = malloc(sizeof(MoveResult));
    Result -> state = NORMAL_MOVE;
    printBoard();
    MoveData* Data = malloc(sizeof(MoveData));
    ResultCode Code = firstTurnRandomBot(starter);
    int choix;

    while(Result->state == NORMAL_MOVE && Code == ALL_GOOD){
        if (t==ADVERSAIRE){
            Code = getMove(Data,Result);
            if(Data->action == DRAW_OBJECTIVES || Data->action == DRAW_BLIND_CARD || (Data->action == DRAW_CARD && Data->drawCard != LOCOMOTIVE)){
                if(Code!= ALL_GOOD){
                    break;
                }
                Code = getMove(Data,Result);
            }

            if (Data->action == CLAIM_ROUTE){
                updateClaimedTrack(tab,nbTracks,Data->claimRoute.from,Data->claimRoute.to);
            }
            t = JOUEUR;
            free(Result->opponentMessage);
        } else{
            if (isAnyTrackClaimable(tab,Hand,nbTracks)){
                choix = (rand() % 3) + 1;
            }else{
                choix = (rand() % 2) + 1;
            }
            printf("Choix : %d\n",choix);
            switch (choix){
            case 1 :
                Code = piocheObjectifRandom(Result);
                break;
            case 2:
                Code = piocheCarteRandom(Result,Hand);
                    break;
            case 3:
                Code = placeTrackRandom(Result,tab,nbTracks,Hand);
                break;
            default:
                Code = LOSING_MOVE;
            }
            t = ADVERSAIRE;
        }
        printBoard();
    }
    free(Data);
    free(Result);
}