//
// Created by vinh on 30/05/25.
//

#ifndef ANALYSE_H
#define ANALYSE_H
#include "structs.h"


void weightTrack(int nbCities,Track*** Matrix);
void weightBasedFirstTurnObjectiveChoice(bool choice[], Objective tab[], Track*** Matrix, int nbCities);

#endif //ANALYSE_H
