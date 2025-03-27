//
// Created by vinh on 27/03/25.
//

#include "manualPlayer.h"
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>

#include "tickettorideapi/ticketToRide.h"

void menuChoix(){
    printf("\n === Tour de Jeu ===\n");
    printf("1. Claim route\n");
    printf("2. Piocher Cart\n");
    printf("3. Piocher Objectif\n");
}

ResultCode claim(MoveResult* Result){
    MoveData* Data = malloc(sizeof(MoveData));
    Data -> action = CLAIM_ROUTE;
    char buffer[10];
    int from = 0;
    int to = 0;
    int locomotive = 0;
    CardColor color = 1;
    printf("Route de : \n");
    fgets(buffer,sizeof(buffer),stdin);
    while (sscanf(buffer,"%d",&from)!=1 || from > 35 || from < 0){
        printf("Entrée invalide\n");
        fgets(buffer,sizeof(buffer),stdin);
    }
    printf("à : \n");
    fgets(buffer,sizeof(buffer),stdin);
    while (sscanf(buffer,"%d",&to)!=1 || to > 35 || to < 0 ){
        printf("Entrée invalide\n");
        fgets(buffer,sizeof(buffer),stdin);
    }
    printf("Couleur : ");
    fgets(buffer,sizeof(buffer),stdin);
    while (sscanf(buffer,"%d",&color)!=1 || color > 8 || color < 1){
        printf("Entrée invalide\n");
        fgets(buffer,sizeof(buffer),stdin);
    }
    printf("nb de Locomotive : \n");
    fgets(buffer,sizeof(buffer),stdin);
    while (sscanf(buffer,"%d",&locomotive)!=1 ||locomotive<0){
        printf("Entrée invalide\n");
        fgets(buffer,sizeof(buffer),stdin);
    }

    Data ->claimRoute.color = color;
    Data ->claimRoute.from = from;
    Data ->claimRoute.to = to;
    Data ->claimRoute.nbLocomotives = locomotive;
    ResultCode Code = sendMove(Data,Result);
    free(Data);
    return Code;
}

ResultCode piocheCarte(MoveResult* Result){
    MoveData* Data = malloc(sizeof(MoveData));
    char buffer[10];
    int choix = 1;
    int choix2 = 1;
    CardColor color = 1;
    ResultCode Code;

    printf("Visible : 1 \n Pioche : 2\n");
    fgets(buffer,sizeof(buffer),stdin);
    while (sscanf(buffer,"%d",&choix) != 1 || choix < 1 || choix > 2){
        printf("Entrée invalide\n");
        fgets(buffer,sizeof(buffer),stdin);
    }
    if (choix == 1){
        Data -> action = DRAW_CARD;
        printf("Quelle couleur ? \n");
        fgets(buffer,sizeof(buffer),stdin);
        while (sscanf(buffer,"%d",&color)!=1 || color > 9 || color < 1){
            printf("Entrée invalide\n");
            fgets(buffer,sizeof(buffer),stdin);
        }
        Data -> drawCard = color;
        Code = sendMove(Data,Result);
        if (color == 9 || Code != ALL_GOOD){
            free(Data);
            return Code;
        }
        printf("Visible : 1 \n Pioche : 2\n");
        fgets(buffer,sizeof(buffer),stdin);
        while (sscanf(buffer,"%d",&choix2) != 1 || choix2 < 1 || choix2 > 2){
            printf("Entrée invalide\n");
            fgets(buffer,sizeof(buffer),stdin);
        }
        if (choix2 == 1 ){
            Data -> action = DRAW_CARD;
            printf("Quelle couleur ? \n");
            fgets(buffer,sizeof(buffer),stdin);
            while (sscanf(buffer,"%d",&color)!=1 || color > 8 || color < 1){
                printf("Entrée invalide\n");
                fgets(buffer,sizeof(buffer),stdin);
            }
            Data -> drawCard = color;
            Code = sendMove(Data,Result);
            free(Data);
            return Code;
        } else{
            Data->action = DRAW_BLIND_CARD;
            Code = sendMove(Data,Result);
            free(Data);
            return Code;
        }
    }
    else{
        Data->action = DRAW_BLIND_CARD;
        Code = sendMove(Data,Result);
        if (Code != ALL_GOOD){
            free(Data);
            return Code;
        }
        printf("Visible : 1 \n Pioche : 2\n");
        fgets(buffer,sizeof(buffer),stdin);
        while (sscanf(buffer,"%d",&choix2) != 1 || choix2 < 1 || choix2 > 2){
            printf("Entrée invalide\n");
            fgets(buffer,sizeof(buffer),stdin);
        }
        if (choix2 == 1 ){
            Data -> action = DRAW_CARD;
            printf("Quelle couleur ? \n");
            fgets(buffer,sizeof(buffer),stdin);
            while (sscanf(buffer,"%d",&color)!=1 || color > 8 || color < 1){
                printf("Entrée invalide\n");
                fgets(buffer,sizeof(buffer),stdin);
            }
            Data -> drawCard = color;
            Code = sendMove(Data,Result);
            free(Data);
            return Code;
        } else{
            Data->action = DRAW_BLIND_CARD;
            Code = sendMove(Data,Result);
            free(Data);
            return Code;
        }
    }
}

ResultCode piocheObjectif(MoveResult* Result){
    MoveData* Data = malloc(sizeof(MoveData));
    char buffer[10];
    ResultCode Code;
    bool a,b,c;


    Data -> action = DRAW_OBJECTIVES;
    Code = sendMove(Data,Result);
    if (Code!=ALL_GOOD){
        free(Data);
        return Code;
    }
    Data -> action = CHOOSE_OBJECTIVES;
    printf("CHOIX DES OBJECTIFS\n");
    fgets(buffer,sizeof(buffer),stdin);
    while (sscanf(buffer,"%d",&a)!=1){
        printf("Entree invalide \n");
        fgets(buffer,sizeof(buffer),stdin);
    }
    printf("CHOIX 2 \n");
    fgets(buffer,sizeof(buffer),stdin);
    while (sscanf(buffer,"%d",&b)!=1){
        printf("Entree invalide \n");
        fgets(buffer,sizeof(buffer),stdin);
    }
    printf("CHOIX 3 \n");
    fgets(buffer,sizeof(buffer),stdin);
    while (sscanf(buffer,"%d",&c)!=1){
        printf("Entree invalide \n");
        fgets(buffer,sizeof(buffer),stdin);
    }
    Data -> chooseObjectives[0] = (bool)a;
    Data -> chooseObjectives[1] = (bool)b;
    Data -> chooseObjectives[2] = (bool)c;
    Code = sendMove(Data,Result);
    free(Data);
    return Code;
}

void manualPlay(int starter)    ²{
    Tour t = starter;
    MoveResult* Result = malloc(sizeof(MoveResult));
    Result -> state = NORMAL_MOVE;
    printBoard();
    MoveData* Data = malloc(sizeof(MoveData));
    ResultCode Code = ALL_GOOD;
    char buffer[10];
    int choix;


    while(Result->state == NORMAL_MOVE && Code == ALL_GOOD){
        if(t == ADVERSAIRE){
            Code = getMove(Data,Result);
            if(Data->action == DRAW_OBJECTIVES || Data->action == DRAW_BLIND_CARD || (Data->action == DRAW_CARD && Data->drawCard != LOCOMOTIVE)){
              if(Code!= ALL_GOOD){
                  break;
              }
              Code = getMove(Data,Result);
            }
            t = JOUEUR;
            free(Result->opponentMessage);
        }else{
            menuChoix();
            fgets(buffer,sizeof(buffer),stdin);
            while(sscanf(buffer,"%d", &choix) != 1 || choix > 3 || choix < 1){
                printf("Entrée invalide\n");
                fgets(buffer,sizeof(buffer),stdin);
            }
            switch(choix){
                case 1 :
                    Code = claim(Result);
                    break;
                case 2 :
                    Code = piocheCarte(Result);
                    break;
                case 3 :
                    Code = piocheObjectif(Result);
                    break;
                default :
                    ;
            }

            t = ADVERSAIRE;
        }
        printBoard();
    }
    free(Data);
    free(Result);
}
