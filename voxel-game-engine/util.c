#pragma once

#include <math.h>

#define PI 3.1415926535f

/* Zastavi radiany na 2PI a 0 */
static float capRad360(float a)
{
    if (a > 2 * PI)
        a -= 2 * PI;
    else if (a < 0)
        a += 2 * PI;

    return a;
}

/* Zastavi radiany na 1/2PI a -1/2PI */
static float capRad90_90(float a)
{
	if (a > PI / 2)
		a = PI / 2;
	else if (a < -PI / 2)
		a = -PI / 2;

	return a;
}