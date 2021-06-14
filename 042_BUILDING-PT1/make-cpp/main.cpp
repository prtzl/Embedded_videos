#include "Test.h"
#include <iostream>

int main()
{
    constexpr int seed = 3878542;
	Test::RandomNumber rn(seed);
    printf("Random: %d\n", rn.getRandomNumber());
    printf("Max random: %d\n", rn.getMaxRandomNumber());
    return 0;
}
