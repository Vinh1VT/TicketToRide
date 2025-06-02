//
// Created by vinh on 01/04/25.
//
#include "firstBot.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

#include "fonctions.h"
#include "structs.h"
#include "analyse.h"



void firstTurnObjectiveChoice(bool choice[], Objective tab[], Track*** Matrix, int nbCities){
    //if it's the first turn, claims 2 objectives, claims only one otherwise
    //always chooses the objectives with the better score/distance ratio
    float rateTab[3] = {objectiveRate(tab[0],Matrix,nbCities),objectiveRate(tab[1],Matrix,nbCities),objectiveRate(tab[2],Matrix,nbCities)};
    float maxRate = floatMax(rateTab[0],rateTab[1],rateTab[2]);
    for (int i = 0; i<3;i++){
        if (rateTab[i] == maxRate){
            choice[i] = true;
            rateTab[i] = 0;
            break;
        }
    }

    maxRate = floatMax(rateTab[0],rateTab[1],rateTab[2]);

    for (int i = 0; i<3;i++){
        if (rateTab[i] == maxRate){
            choice[i] = true;
            rateTab[i] = 0;
            break;
        }
    }
}

ResultCode firstTurnBot(int starter, int* objectiveDeck, Objective objectiveTab[], Track*** Matrix, int nbCities){
    Tour t = starter;
    MoveResult* Result = malloc(sizeof(MoveResult));
    MoveData* Data = malloc(sizeof(MoveData));
    ResultCode Code;
    bool choice[3] = {false,false,false};
    if (t == ADVERSAIRE){
        //Gets the opponent's first move (Drawing objectives)
        Code = getMove(Data, Result);
        if (Code!=ALL_GOOD){
            return Code;
        }
        Code = getMove(Data,Result);
        if (Code!=ALL_GOOD){
            return Code;
        }
        for (int i = 0; i<3;i++){
            if (Data->chooseObjectives[i]){
                *objectiveDeck -=  1;
            }
        }

        //Plays our move
        Data->action = DRAW_OBJECTIVES;
        Code = sendMove(Data,Result);
        if (Code != ALL_GOOD){
            return Code;
        }
        weightBasedFirstTurnObjectiveChoice(choice,Result->objectives, Matrix, nbCities);
        *objectiveDeck -= 2;
        int p = 0;
        Data->action = CHOOSE_OBJECTIVES;
        //This loop makes a copy of the objectives the bot chooses and inputs the good data
        for (int i =0 ; i<3 ; i++){
            if (choice[i]){
                objectiveTab[p] = objectiveCopy(Result->objectives[i]);
                p++;
            }
            Data -> chooseObjectives[i] = choice[i];
        }
        Code = sendMove(Data,Result);
    }else{
        //Plays our move
        Data->action = DRAW_OBJECTIVES;
        Code = sendMove(Data,Result);
        if (Code != ALL_GOOD){
            return Code;
        }
        weightBasedFirstTurnObjectiveChoice(choice,Result->objectives, Matrix, nbCities);
        *objectiveDeck -= 2;
        int p = 0;
        Data->action = CHOOSE_OBJECTIVES;
        //This loop makes a copy of the objectives the bot chooses and inputs the good data
        for (int i =0 ; i<3 ; i++){
            if (choice[i]){
                objectiveTab[p] = objectiveCopy(Result->objectives[i]);
                p++;
            }
            Data -> chooseObjectives[i] = choice[i];
        }
        Code = sendMove(Data,Result);
        if (Code!=ALL_GOOD){
            return Code;
        }

        //Gets the opponent first move (Drawing objectives)
        Code = getMove(Data, Result);
        if (Code!=ALL_GOOD){
            return Code;
        }
        Code = getMove(Data,Result);

        for (int i = 0; i<3;i++){
            if (Data->chooseObjectives[i]){
                *objectiveDeck -=  1;
            }
        }

    }
    free(Result);
    free(Data);
    return Code;
}

ResultCode claimRouteBot(MoveResult* Result,CardColor color,int locomotives,Track* track,int Hand[], int* wagon){
    MoveData* Data = malloc(sizeof(MoveData));

    Data->action = CLAIM_ROUTE;
    ClaimRouteMove claim = {
        .from = track->Ville1,
        .to = track->Ville2,
        .color = color,
        .nbLocomotives = locomotives
    };
    Data->claimRoute = claim;
    *wagon -= (int)track->Longueur;
    Hand[8] -= locomotives;
    Hand[color-1] -= (int)track->Longueur - locomotives;
    ResultCode Code = sendMove(Data, Result);
    track->Claimed = PLAYER;
    track->Longueur = 0;
    free(Data);
    return Code;
}

CardColor chooseColorTarget(int Hand[], Track* track, int Prec[],Track*** Matrix){ //the target color is the color we need the most to complete the path
    if (track != NULL){
        int targets[9] = {0,0,0,0,0,0,0,0,0};
        Track* path = track;
        while (Prec[path->Ville1] != -1 && Prec[path->Ville2]!= -1){
            if (path->Claimed == UNCLAIMED){
                targets[path->Couleur1-1] += path->Longueur;
                if (path->Couleur2 != NONE && path->Couleur2 != LOCOMOTIVE){
                    targets[path->Couleur2-1] += path->Longueur;
                }
            }
            Track* temp = Matrix[path->Ville2][Prec[path->Ville2]];
            if (temp != path){
                path = temp;
            } else{
                path = Matrix[path->Ville1][Prec[path->Ville1]];
            }
        }

        for (int i = 0; i<9;i++){
            targets[i] -= Hand[i];
        }

        CardColor target = maxHand(targets);
        if (target == LOCOMOTIVE){
            return maxHand(Hand);
        }
        return target;
    }
    return maxHand(Hand);
}

bool targetColorOnBoard(CardColor target){
    BoardState Board;
    ResultCode Code = getBoardState(&Board);

    if (Code != ALL_GOOD){
        return false;
    }

    for (int i = 0;i<5;i++){
        if (Board.card[i] == target){
            return true;
        }
    }

    return false;
}

ResultCode drawCardBot(MoveResult* Result,int Hand[], CardColor target,int* cardDeck){
    MoveData* Data = malloc(sizeof(MoveData));
    ResultCode Code;

    for (int i = 0 ; i<2 ; i++){ //Rarely seen a more useless loop, but oh well, the code does look better with it
        if (targetColorOnBoard(target) && ~(target == LOCOMOTIVE && i ==1)){
            Data->action = DRAW_CARD;
            Data->drawCard = target;
            Code = sendMove(Data,Result);
            addToHand(Hand,target);
            *cardDeck -= 1;
        }else if (*cardDeck > 6){ //Si il reste des cartes pas sur le board
            Data->action = DRAW_BLIND_CARD;
            Code = sendMove(Data,Result);
            addToHand(Hand,Result->card);
            *cardDeck -= 1;
        }else{
            if (i == 0 && targetColorOnBoard(LOCOMOTIVE)){
                Data->action = DRAW_CARD;
                Data->drawCard = LOCOMOTIVE;
                Code = sendMove(Data,Result);
                addToHand(Hand,LOCOMOTIVE);
                *cardDeck -= 1;
                return Code;
            }
            Data->action = DRAW_CARD;
            BoardState* board = malloc(sizeof(BoardState));
            getBoardState(board);
            for (int j = 0; j < 5;j++){
                if (board->card[j] != LOCOMOTIVE){
                    Data->drawCard = board->card[j];
                    break;
                }
            }
            free(board);
            Code = sendMove(Data,Result);
        }

        if (Code!=ALL_GOOD) return Code;
    }
    free(Data);
    return Code;
}

/*ResultCode claimNextRoute(MoveResult *Result,Track* nextTrack,int Hand[],int Prec[], Track*** Matrix){ //Deprecated function
    MoveData* Data = malloc(sizeof(MoveData));
    Track* claimTrack=Matrix[nextTrack->Ville2][Prec[nextTrack->Ville2]] ;
    int locomotives = 0;
    CardColor color = NONE;
    while (claimTrack->Longueur == 0 ||  color == NONE){
        Track* temp = Matrix[claimTrack->Ville2][Prec[claimTrack->Ville2]];
        if (temp != claimTrack){
            claimTrack = temp;
        }else{
            claimTrack = Matrix[claimTrack->Ville1][Prec[claimTrack->Ville1]];
        }
        color = claimableTrack(*claimTrack,Hand,&locomotives);
    }


    ResultCode Code = claimRouteBot(Result,color,locomotives,claimTrack,Hand);

    free(Data);
    return Code;
}*/


Track* claimableTrackInPath(Track* firstTrack,int Hand[],int Prec[],Track*** Matrix,int wagon){
    Track* claimTrack = firstTrack;
    int locomotives = 0;
    CardColor color = claimableTrack(*claimTrack,Hand,&locomotives, wagon);
    while (Prec[claimTrack->Ville1] != -1 && Prec[claimTrack->Ville2]!= -1 && color == NONE ){

        Track* temp = Matrix[claimTrack->Ville2][Prec[claimTrack->Ville2]];
        if (temp != claimTrack){
            claimTrack = temp;
        } else{
            claimTrack = Matrix[claimTrack->Ville1][Prec[claimTrack->Ville1]];
        }
        color = claimableTrack(*claimTrack,Hand,&locomotives, wagon);
    }
    if (color != NONE){
        return claimTrack;
    }
    return NULL;
}

ResultCode claimDefaultTrack(MoveResult* Result,int  nbCities, Track*** Matrix, int Hand[], int* wagon){
    //Claim the longest path available connected to our network, that is not of our target color

    for (int i =0;i<nbCities;i++){
        for (int j =i+1;j<nbCities;j++){
            Track* current = Matrix[i][j];
            if (current != NULL && current->Claimed == UNCLAIMED){
                int locomotives = 0;
                CardColor color = claimableTrack(*current,Hand,&locomotives,*wagon);
                if (color != NONE){
                    return claimRouteBot(Result,color,locomotives,current,Hand,wagon);
                }
            }
        }
    }

    return LOSING_MOVE;
}


void firstBotPlay(int starter,int Hand[], Track*** Matrix,int nbCities){
//This bot will try to do the shortest path to his objectives, without any priority on which track to claim
//it will choose the objectives that always get the most points
    int cardDeck = 97;
    int objectiveDeck = 30;
    int wagon = 45;
    int opponentWagon = 45;
    Objective objectiveTab[2];
    int objectiveCount = 2;
    MoveResult* Result = malloc(sizeof(MoveResult));
    MoveData* Data = malloc(sizeof(MoveData));
    Result -> state = NORMAL_MOVE;
    ResultCode Code= firstTurnBot(starter,&objectiveDeck,objectiveTab, Matrix, nbCities);
    //so the first objective of the list is the one with the biggest score (might already be true though)
    sortObjective(objectiveTab,objectiveCount);
    Tour t = starter;

    //Dijsktra initialization
    unsigned int D[nbCities];
    int Prec[nbCities];
    Dijkstra(objectiveTab[0].from, Matrix,nbCities,D,Prec);
    Track* firstTrack = Matrix[objectiveTab[0].to][Prec[objectiveTab[0].to]];

    //Main Loop of play
    while (Result->state == NORMAL_MOVE && Code == ALL_GOOD){
        //printBoard();
        if (t==ADVERSAIRE){
            Code = getMove(Data,Result);
            //free(Result->message);
            //free(Result->opponentMessage);
            if (Data-> action == CLAIM_ROUTE){
                opponentWagon -= Matrix[Data->claimRoute.from][Data->claimRoute.to]->Longueur;
                Matrix[Data->claimRoute.from][Data->claimRoute.to]->Longueur = INT_MAX;
                Matrix[Data->claimRoute.from][Data->claimRoute.to]->Claimed = ADV;
            }
            else if (Data->action == DRAW_CARD || Data->action == DRAW_BLIND_CARD){
                cardDeck-=1;
            }
            if(Data->action == DRAW_OBJECTIVES || Data->action == DRAW_BLIND_CARD || (Data->action == DRAW_CARD && Data->drawCard != LOCOMOTIVE)){
                if(Code!= ALL_GOOD){
                    break;
                }
                if (Data->action == DRAW_CARD || Data->action == DRAW_BLIND_CARD){
                    cardDeck-=1;
                }
                Code = getMove(Data,Result);
                //free(Result->message);
                //free(Result->opponentMessage);
                if (Data->action ==CHOOSE_OBJECTIVES){
                    for (int i = 0; i<3;i++){
                        if (Data->chooseObjectives[i]){
                            objectiveDeck -= 1;
                        }
                    }
                }
            }
            t = JOUEUR;
        }else{
            int locomotives = 0;

            //Dijsktra and objectives update
            Dijkstra(objectiveTab[0].from,Matrix,nbCities,D,Prec);
            if (D[objectiveTab[0].to] == 0 || D[objectiveTab[0].to] == INT_MAX){ //Switch objectives if it is completed or impossible
                if (objectiveCount == 2){
                    objectiveTab[0] = objectiveTab[1]; //Shift left of objectives, I hope this won't segfault or anything
                    objectiveTab[1].from = 0;
                    objectiveTab[1].to = 0;
                    objectiveTab[1].score = 0;
                }
                objectiveCount -= 1;
                if (objectiveCount>0) {
                    Dijkstra(objectiveTab[0].from,Matrix,nbCities,D,Prec);
                    firstTrack = Matrix[objectiveTab[0].to][Prec[objectiveTab[0].to]];
                }else {
                    firstTrack = NULL;
                }
            }else if (objectiveCount == 0){
                firstTrack = NULL;
            }


            Track* claimTrack;
            if (firstTrack != NULL) {
                if (firstTrack->Longueur == INT_MAX){
                    firstTrack = Matrix[objectiveTab[0].to][Prec[objectiveTab[0].to]];
                }
                claimTrack = claimableTrackInPath(firstTrack,Hand,Prec,Matrix,wagon); //A track on the path to the first objective that is claimable
            }else {
                claimTrack = NULL;
            }


            //Play
            if (objectiveCount > 0 || wagon < 8 || opponentWagon < 8){
                if (claimTrack != NULL){
                    CardColor color = claimableTrack(*claimTrack,Hand,&locomotives, wagon);
                    Code = claimRouteBot(Result,color,locomotives,claimTrack,Hand,&wagon);
                }else if (totalCardsInHand(Hand)<48){
                    CardColor targetColor = chooseColorTarget(Hand, firstTrack,Prec,Matrix);
                    Code = drawCardBot(Result,Hand,targetColor,&cardDeck);
                }else{
                    Code = claimDefaultTrack(Result,nbCities,Matrix,Hand,&wagon);
                }
            }else{
                int choice[3]= {0,0,0};
                MoveData* drawObjectiveMove = malloc(sizeof(MoveData));
                drawObjectiveMove ->action = DRAW_OBJECTIVES;
                Code = sendMove(drawObjectiveMove,Result);
                if (Code == ALL_GOOD){
                    drawObjectiveMove -> action = CHOOSE_OBJECTIVES;
                    objectiveChoice(choice,Result->objectives,Matrix,nbCities,&objectiveCount,&objectiveDeck);
                    for (int i = 0; i<3;i++){
                        drawObjectiveMove->chooseObjectives[i] = choice[i];
                        if (choice[i] == 1){
                            objectiveTab[0] = objectiveCopy(Result->objectives[i]);

                        }
                    }
                    Code = sendMove(drawObjectiveMove,Result);
                    Dijkstra(objectiveTab[0].from,Matrix,nbCities,D,Prec);
                    firstTrack = Matrix[objectiveTab[0].to][Prec[objectiveTab[0].to]];
                }
                free(drawObjectiveMove);
            }
            t = ADVERSAIRE;
        }
    }
    free(Data);
    free(Result);

}