#include "frand.h"

float frand()
{
    return rand() / (float)RAND_MAX;
}