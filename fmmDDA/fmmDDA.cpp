// fmmDDA.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "display.h"
#include "initial_conditions.h"

constexpr int WIDTH = 32;
constexpr int HEIGHT = 32;

/*struct vec
{
    double x; double y;
    vec operator+(vec other)
    {
        return { x + other.x, y + other.y };
    }
    vec operator-(vec other)
    {
        return { x - other.x, y - other.y };
    }
    vec operator*(double z)
    {
        return { x * z, y * z };
    }
    vec operator/(double z)
    {
        return { x / z, y / z };
    }
    vec operator=(vec other)
    {
        return { other.x, other.y };
    }
    bool isnan()
    {
        return (std::isnan(x) || std::isnan(y));
    }
};*/

vec fld[WIDTH * HEIGHT]{};
uint8_t fldtrait[WIDTH * HEIGHT]{};

void setPixel(int x, int y, vec displacement)
{
    if ((fld[x + y * WIDTH]).isnan())
    {
        fld[x + y * WIDTH] = displacement;
    }
}

vec getPixel(int x, int y)
{
    return fld[x + y * WIDTH];
}

void iterateStencil(int ix, int iy)
{
    vec mindist = { sqrt((double)(WIDTH * WIDTH + HEIGHT * HEIGHT)), sqrt((double)(WIDTH * WIDTH + HEIGHT * HEIGHT)) };
    if (!(getPixel(ix - 1, iy)).isnan())
        mindist = getPixel(ix - 1, iy);
    else if (!(getPixel(ix + 1, iy)).isnan())
        mindist = getPixel(ix + 1, iy);
    else if (!(getPixel(ix, iy - 1)).isnan())
        mindist = getPixel(ix, iy - 1);
    else if (!(getPixel(ix, iy + 1)).isnan())
        mindist = getPixel(ix, iy + 1);

    if (!(mindist.isnan()))
        setPixel(ix, iy, mindist);
}

int main()
{
    constexpr double nan = std::numeric_limits<double>::quiet_NaN();
    for (int ix = 0; ix < WIDTH * HEIGHT; ++ix) fld[ix] = { nan, nan };
    for (int ix = 0; ix < WIDTH * HEIGHT; ++ix) fldtrait[ix] = 0;

    init(WIDTH, HEIGHT, fld);

    for (int iy = 1; iy < HEIGHT; ++iy)
    {
        for (int ix = 1; ix < WIDTH; ++ix)
        {
            iterateStencil(ix, iy);
        }
    }

    //display(WIDTH, HEIGHT, fld);
}
