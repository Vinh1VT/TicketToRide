//
// Created by vinh on 01/04/25.
//
#include "firstBot.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include "fonctions.h"
#include "structs.h"



void firstTurnObjectiveChoice(bool choice[], Objective tab[]){
    //if it's the first turn, claims 2 objectives, claims only one otherwise
    //always chooses the objectives with the most point
    int scoreMax = max(tab[0].score,tab[1].score,tab[2].score);
    for (int i =0; i<3;i++){
        if (tab[i].score == scoreMax){
            choice[i] = true;
            break;
        }
    }
     //a bit convoluted code, but it just chooses the second-highest score for the first turn when we must choose 2 objectives
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

ResultCode firstTurnBot(int starter, int* objectiveDeck,Objective objectiveTab[]){
    Tour t = starter;
    MoveResult* Result = malloc(sizeof(MoveResult));
    MoveData* Data = malloc(sizeof(MoveData));
    ResultCode Code;
    bool choice[3] = {false,false,false};
    FILE* f = fopen("objective","a"); //for debug purspose
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
        firstTurnObjectiveChoice(choice,Result->objectives);
        *objectiveDeck -= 2;
        int p = 0;
        Data->action = CHOOSE_OBJECTIVES;
        //This loop makes a copy of the objectives the bot chooses and inputs the good data
        for (int i =0 ; i<3 ; i++){
            if (choice[i]){
                objectiveTab[p] = objectiveCopy(Result->objectives[i]);
                fprintf(f,"%u -> %u \n",Result->objectives[i].from, Result->objectives[i].to);
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
        firstTurnObjectiveChoice(choice,Result->objectives);
        *objectiveDeck -= 2;
        int p = 0;
        Data->action = CHOOSE_OBJECTIVES;
        //This loop makes a copy of the objectives the bot chooses and inputs the good data
        for (int i =0 ; i<3 ; i++){
            if (choice[i]){
                objectiveTab[p] = objectiveCopy(Result->objectives[i]);
                fprintf(f,"%u -> %u \n",Result->objectives[i].from, Result->objectives[i].to);
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
    fclose(f);
    free(Result);
    free(Data);
    return Code;
}

ResultCode claimRouteBot(MoveResult* Result,CardColor color,int* locomotives,Track* track,int Hand[]){
    MoveData* Data = malloc(sizeof(MoveData));

    Data->action = CLAIM_ROUTE;
    ClaimRouteMove claim = {
        .from = track->Ville1,
        .to = track->Ville2,
        .color = color,
        .nbLocomotives = *locomotives
    };
    Data->claimRoute = claim;
    Hand[8] -= *locomotives;
    Hand[color-1] -= track->Longueur - *locomotives;
    ResultCode Code = sendMove(Data, Result);
    track->Claimed = PLAYER;
    track->Longueur = 0;

    free(Data);
    return Code;
}

CardColor chooseColorTarget(int Hand[], Track* track){
    if (track->Couleur1 == LOCOMOTIVE){
        return maxHand(Hand);
    }
    else{
        if (!track->Double || Hand[track->Couleur1-1]>= Hand[track->Couleur2-1]){
            return track->Couleur1;
        }
        return track->Couleur2;
    }
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

ResultCode drawCardBot(MoveResult* Result,int Hand[], CardColor target){
    MoveData* Data = malloc(sizeof(MoveData));
    ResultCode Code;

    for (int i = 0 ; i<2 ; i++){ //Rarely seen a more useless loop, but oh well, the code does look better with it
        if (targetColorOnBoard(target)){
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

ResultCode claimNextRoute(MoveResult *Result,Track* nextTrack,int Hand[],int Prec[], Track*** Matrix){
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


    ResultCode Code = claimRouteBot(Result,color,&locomotives,claimTrack,Hand);

    free(Data);
    return Code;
}


void firstBotPlay(int starter,int nbTracks,int Hand[], Track*** Matrix,int nbCities){
//This bot will try to do the shortest path to his objectives, without any priority on which track to claim
//it will choose the objectives that always get the most points
    int cardDeck = 97;
    int objectiveDeck = 30;
    Objective objectiveTab[2];
    int objectiveCount = 2;
    MoveResult* Result = malloc(sizeof(MoveResult));
    MoveData* Data = malloc(sizeof(MoveData));
    Result -> state = NORMAL_MOVE;
    ResultCode Code= firstTurnBot(starter,&objectiveDeck,objectiveTab);
    //so the first objective of the list is the one with the biggest score (might already be true though)
    sortObjective(objectiveTab,objectiveCount);
    Tour t = starter;

    //Dijsktra initialization
    unsigned int D[nbCities]; //why tf does it need a +1
    int Prec[nbCities];
    Dijkstra(objectiveTab[0].from, Matrix,nbCities,D,Prec);
    Track* nextTrack = Matrix[objectiveTab[0].to][Prec[objectiveTab[0].to]];

    //Main Loop of play
    while (Result->state == NORMAL_MOVE && Code == ALL_GOOD){
        printBoard();
        if (t==ADVERSAIRE){
            Code = getMove(Data,Result);
            //free(Result->message);
            //free(Result->opponentMessage);
            if (Data-> action == CLAIM_ROUTE){
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
            //printf("%d->%d\n",objectiveTab[0].from,objectiveTab[0].to);
            int locomotives = 0;
            CardColor targetColor = chooseColorTarget(Hand, nextTrack);
            CardColor color = claimableTrack(*nextTrack, Hand, &locomotives);


            //Dijsktra and objectives update
            Dijkstra(objectiveTab[0].from,Matrix,nbCities,D,Prec);
            if (D[objectiveTab[0].to] == 0){
                printf("YOUHOU\n");
                if (objectiveCount == 2){
                    objectiveTab[0] = objectiveTab[1]; //Shift left of objectives, I hope this won't segfault or anything
                    objectiveTab[1].from = 0;
                    objectiveTab[1].to = 0;
                    objectiveTab[1].score = 0;
                }
                objectiveCount -= 1;
                if (objectiveCount!=0){
                    Dijkstra(objectiveTab[0].from,Matrix,nbCities,D,Prec);
                    nextTrack = Matrix[objectiveTab[0].to][Prec[objectiveTab[0].to]];
                }
            }
            while (objectiveCount != 0 && nextTrack->Longueur == 0){
                unsigned int Ville = nextTrack->Ville2;
                Track* nextTrack2 = Matrix[Ville][Prec[Ville]];
                if ( nextTrack != nextTrack2){
                    nextTrack = nextTrack2;
                }else{
                    Ville = nextTrack->Ville1;
                    nextTrack = Matrix[Ville][Prec[Ville]];
                }
            }


            //Play
            if (objectiveCount > 0){
                if (color != 0){
                    Code = claimRouteBot(Result,color,&locomotives,nextTrack,Hand);
                }else if (nbCardInHand(Hand) > 45){ //To avoid overflowing the hand, the bot claims the next available track
                    Code = claimNextRoute(Result,nextTrack,Hand,Prec, Matrix);
                }else{
                    Code = drawCardBot(Result,Hand,targetColor); //it never draws locmotives (except by drawing blindly), but oh well
                    cardDeck -= 2;
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
                            FILE* f = fopen("objective","a");
                            fprintf(f,"%u -> %u \n",objectiveTab[0].from,objectiveTab[0].to);
                            fclose(f);
                        }
                    }
                    Code = sendMove(drawObjectiveMove,Result);
                    Dijkstra(objectiveTab[0].from,Matrix,nbCities,D,Prec);
                    nextTrack = Matrix[objectiveTab[0].to][Prec[objectiveTab[0].to]];
                }
                free(drawObjectiveMove);
            }
            t = ADVERSAIRE;
        }
    }
    free(Data);
    free(Result);

}