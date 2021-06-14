#pragma once

namespace Test
{

class RandomNumber
{
public:
	explicit RandomNumber(unsigned int const seed) : seed(seed)
	{}
	int getRandomNumber();
	int getMaxRandomNumber();
private:
	unsigned int const seed;
};

} // namespace Test
