#include "Test.h"
#include <cstdlib>

namespace Test
{

int RandomNumber::getRandomNumber()
{
    std::srand(this->seed);
    return std::rand();
}

int RandomNumber::getMaxRandomNumber()
{
    return RAND_MAX;
}

} // namespace Test
