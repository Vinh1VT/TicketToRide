//
// Created by vinh on 01/04/25.
//
#include "firstBot.h"

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
        freeMessage(Result);
        if (Code!=ALL_GOOD){
            return Code;
        }
        Code = getMove(Data,Result);
        freeMessage(Result);
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



void firstBotPlay(int starter,Track tab[],int nbTracks,int Hand[]){
//This bot will try to do the shortest path to his objectives, without any priority on which track to claim
//it will choose the objectives that always get the most points
    int cardDeck = 97;
    int objectiveDeck = 30;
    Objective objectiveTab[10];

}