#include <stdio.h>
#include <stdlib.h>
#include "tickettorideapi/ticketToRide.h"

int main(){

    extern int DEBUG_LEVEL;
    DEBUG_LEVEL = DEBUG;
    GameSettings settings = GameSettingsDefaults;
    GameData* data = malloc(sizeof(GameData));
    settings.gameType = TRAINING;
    settings.botId = RANDOM_PLAYER;
    *data = GameDataDefaults;
    
    

    connectToCGS("cgs.valentin-lelievre.com",15001);
    sendName("Vinh");
    sendGameSettings(settings,data);
    printBoard();

    return EXIT_SUCCESS;
}