#include "Test.h"
#include <stdio.h>

int main()
{
    int const seed = 3878542;
    printf("Random: %d\n", getRandomNumber(seed));
    printf("Max random: %d\n", getMaxRandomNumber());
    return 0;
}
