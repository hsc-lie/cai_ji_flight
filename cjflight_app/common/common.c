#include "common.h"


int int_range(int value, int min, int max)
{
	if(value < min)
	{
		return min;
	}
	else if(value > max)
	{
		return max;
	}
	else
	{
		return value;
	}
}


float float_range(float value, float min, float max)
{
	if(value < min)
	{
		return min;
	}
	else if(value > max)
	{
		return max;
	}
	else
	{
		return value;
	}
}

