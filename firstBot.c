//
// Created by vinh on 01/04/25.
//
#include "firstBot.h"

#include <stdio.h>
#include <stdlib.h>

#include "fonctions.h"
#include "structs.h"

int max(unsigned int a,  unsigned int b, unsigned int c){
    //returns the max out of 3 numbers, useful for the choice function
    if (a>b && a >c){
        return a;
    }else if (b>a && b >c){
        return b;
    }else{
        return c;
    }
}

void objectiveChoice(bool choice[],Objective tab[],bool firstTurn){
    //if it's the first turn, claims 2 objectives, claims only one otherwise
    //always choose the most point objectives
    int scoreMax = max(tab[0].score,tab[1].score,tab[2].score);
    for (int i =0; i<3;i++){
        if (tab[i].score == scoreMax){
            choice[i] = true;
            break;
        }
    }
    if (firstTurn){ //a bit convoluted code, but just choose the second-highest score for the first turn when we must choose 2 objectives
        int a = 0;
        int b = 0;
        for (int i = 0; i<3; i++){
            if (!choice[i] && !a){
                a = tab[i].score;
            } else if (!choice[i]){
                b = tab[i].score;
            }
        }
        scoreMax = max(a,b,0);
        for (int i = 0; i<3;i++){
            if (tab[i].score == scoreMax && !choice[i]){
                choice[i] = true;
                break;
            }
        }
    }
}

ResultCode firstTurnBot(int starter, int* objectiveDeck,Objective objectiveTab[]){
    Tour t = starter;
    MoveResult* Result = malloc(sizeof(MoveResult));
    MoveData* Data = malloc(sizeof(MoveData));
    ResultCode Code = LOSING_MOVE;
    bool choice[3] = {false,false,false};
    if (t == ADVERSAIRE){
        //Gets the opponent first move (Drawing objectives)
        Code = getMove(Data, Result);
        //freeMessage(Result);
        if (Code!=ALL_GOOD){
            return Code;
        }
        Code = getMove(Data,Result);
        //freeMessage(Result);
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
        freeMessage(Result);
        objectiveChoice(choice,Result->objectives,true);
        *objectiveDeck -= 2;
        int p = 0;
        Data->action = CHOOSE_OBJECTIVES;
        //This loop make a copy of the objectives the bot chooses and inputs the good data
        for (int i =0 ; i<3 ; i++){
            if (choice[i]){
                objectiveTab[p] = objectiveCopy(Result->objectives[i]);
                p++;
            }
            Data -> chooseObjectives[i] = choice[i];
        }
        Code = sendMove(Data,Result);
        freeMessage(Result);
    }else{
        //Plays our move
        Data->action = DRAW_OBJECTIVES;
        Code = sendMove(Data,Result);
        if (Code != ALL_GOOD){
            return Code;
        }
        freeMessage(Result);
        objectiveChoice(choice,Result->objectives,true);
        *objectiveDeck -= 2;
        int p = 0;
        Data->action = CHOOSE_OBJECTIVES;
        //This loop make a copy of the objectives the bot chooses and inputs the good data
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
        freeMessage(Result);

        //Gets the opponent first move (Drawing objectives)
        Code = getMove(Data, Result);
        freeMessage(Result);
        if (Code!=ALL_GOOD){
            return Code;
        }
        Code = getMove(Data,Result);
        freeMessage(Result);

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

ResultCode claimRouteBot(MoveResult* Result,CardColor color,int locomotives,Track* track){
    MoveData* Data = malloc(sizeof(MoveData));

    Data->action = CLAIM_ROUTE;
    ClaimRouteMove claim = {
        .from = track->Ville1,
        .to = track->Ville2,
        .color = color,
        .nbLocomotives = locomotives
    };
    Data->claimRoute = claim;

    ResultCode Code = sendMove(Data, Result);

    free(Data);
    return Code;
}

CardColor chooseColorTarget(int Hand[], Track* track){
    if (Hand[track->Couleur1-1]>= Hand[track->Couleur2-1]){
        return track->Couleur1;
    }
    return track->Couleur2;
}

bool targetOnBoard(CardColor target){
    ResultCode Code;
    BoardState Board;

    Code = getBoardState(&Board);

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


ResultCode drawCardBot(MoveResult* Result, Hand[], CardColor target){
    MoveData* Data = malloc(sizeof(MoveData));
    ResultCode Code;

    for (int i = 0 ; i<2 ; i++){ //Rarely seen a more useless loop, but oh well the code does look better with it
        if (targetOnBoard(target)){
            Data->action = DRAW_CARD;
            Data->drawCard = target;
            Code = sendMove(Data,Result);
            if (Code!=ALL_GOOD){
                return Code;
            }
            addToHand(Hand,target);
        }else{
            Data -> action = DRAW_BLIND_CARD;
            Code = sendMove(Data,Result);
            if (Code != ALL_GOOD){
                return Code;
            }
            addToHand(Hand,Result->card);
        }
    }

    free(Data);
    return Code;
}


void firstBotPlay(int starter,Track tab[],int nbTracks,int Hand[], Track*** Matrix){
//This bot will try to do the shortest path to his objectives, without any priority on which track to claim
//it will choose the objectives that always get the most points
    int cardDeck = 97;
    int objectiveDeck = 30;
    Objective objectiveTab[5];
    int objectiveCount = 2;
    MoveResult* Result = malloc(sizeof(MoveResult));
    MoveData* Data = malloc(sizeof(MoveData));
    Result -> state = NORMAL_MOVE;
    ResultCode Code= firstTurnBot(starter,&objectiveDeck,objectiveTab);
    //so the first objective of the list is the one with the biggest score (might already be true though)
    sortObjective(objectiveTab,objectiveCount);
    Tour t = starter;

    //Dijsktra initialization
    unsigned int D[nbTracks];
    unsigned int Prec[nbTracks];
    Dijkstra(objectiveTab[0].from, Matrix,nbTracks,D,Prec);
    Track* next = Matrix[objectiveTab[0].to][(unsigned)Prec[objectiveTab[0].to]];


    //Main Loop of play
    while (Result->state == NORMAL_MOVE && Code == ALL_GOOD){
        printBoard();
        if (t==ADVERSAIRE){
            Code = getMove(Data,Result);
            //free(Result->message);
            //free(Result->opponentMessage);
            if (Data->action == DRAW_CARD || Data->action == DRAW_BLIND_CARD){
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
        }else{
            int locomotives;
            CardColor color;
            CardColor target;
            target = chooseColorTarget(Hand,next);
            color = claimableTrack(*next,Hand,&locomotives);
            //TODO : Update the dijsktra to look for the next road to take, and the target color
            if (color != 0){
                //TODO : CLAIM TRACK AND UPDATE THE NEXT TRACK TO CLAIM
                Code = claimRouteBot(Result,color,locomotives,next);

            }else{
                drawCardBot(Result,Hand,target); //it never draws locmotives (except by drawing blindly), but oh well
                cardDeck -= 2;

            }
        }
    }


    free(Data);
    free(Result);
    
}