#pragma once

#include <math.h>

#define PI 3.1415926535f

/**
 * @brief Cap radians to the range [0, 2*PI)
 *
 * @param a Angle in radians
 * @return Angle in radians capped to the range [0, 2*PI)
 */
static float capRad360(float a)
{
    if (a > 2 * PI)
        a -= 2 * PI;
    else if (a < 0)
        a += 2 * PI;

    return a;
}

/**
 * @brief Cap radians to the range [-PI/2, PI/2]
 *
 * @param a Angle in radians
 * @return Angle in radians capped to the range [-PI/2, PI/2]	
*/
static float capRad90_90(float a)
{
	if (a > PI / 2)
		a = PI / 2;
	else if (a < -PI / 2)
		a = -PI / 2;

	return a;
}