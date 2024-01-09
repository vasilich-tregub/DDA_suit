// fmmDDA.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "display.h"
#include "initial_conditions.h"

constexpr int WIDTH = 32;
constexpr int HEIGHT = 32;

double fld[WIDTH * HEIGHT]{};
uint8_t fldtrait[WIDTH * HEIGHT]{};

void setPixel(int x, int y, double Y = 0, uint8_t trait = 0)
{
    double Yfld = fld[x + y * WIDTH];
    uint8_t traitfld = fldtrait[x + y * WIDTH];
    if (Y != 0 && Y + Yfld == 0)
    {
        fld[x + y * WIDTH] = std::numeric_limits<double>::quiet_NaN();
        fldtrait[x + y * WIDTH] = 0;
        return;
    }
    if (std::isnan(fld[x + y * WIDTH]) || (trait < fldtrait[x + y * WIDTH]) ||
        ((trait == fldtrait[x + y * WIDTH]) && (abs(Y) < abs(fld[x + y * WIDTH]))))
    {
        fld[x + y * WIDTH] = Y;
        fldtrait[x + y * WIDTH] = trait;
    }
}

double getPixel(int x, int y)
{
    return fld[x + y * WIDTH];
}

int main()
{
    constexpr double nan = std::numeric_limits<double>::quiet_NaN();
    for (int ix = 0; ix < WIDTH * HEIGHT; ++ix) fld[ix] = nan;
    for (int ix = 0; ix < WIDTH * HEIGHT; ++ix) fldtrait[ix] = 0;

    init(WIDTH, HEIGHT, fld);

    for (int iy = 1; iy < HEIGHT; ++iy)
    {
        for (int ix = 1; ix < WIDTH; ++ix)
        {
            if (!std::isnan(getPixel(ix+1, iy)))
                setPixel(ix, iy, getPixel(ix+1, iy));
        }
    }

    display(WIDTH, HEIGHT, fld);
}
