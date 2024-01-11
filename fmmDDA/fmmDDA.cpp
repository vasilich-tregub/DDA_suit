// fmmDDA.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "display.h"
#include "fmmDDA.h"

int main()
{
    constexpr double nan = std::numeric_limits<double>::quiet_NaN();
    for (int ix = 0; ix < WIDTH * HEIGHT; ++ix) 
    {
        fld[ix].x = nan;
        fld[ix].y = nan;
        fldY[ix] = nan;
    };
    for (int ix = 0; ix < WIDTH * HEIGHT; ++ix) fldtrait[ix] = 0;

    init(WIDTH, HEIGHT, fld);

    for (int iy = 1; iy < HEIGHT; ++iy)
    {
        for (int ix = 1; ix < WIDTH; ++ix)
        {
            iterateStencil(ix, iy);
        }
    }
    /*for (int ix = 1; ix < WIDTH; ++ix)
    {
        for (int iy = 1; iy < HEIGHT; ++iy)
        {
            iterateStencil(ix, iy);
        }
    }*/

    // (fld[ix + iy * WIDTH].x > 0.0 || fld[ix + iy * WIDTH].y > 0.0 ) is Ok, but is it a truly right condition?
    for (int iy = 1; iy < HEIGHT; ++iy)
    {
        for (int ix = 1; ix < WIDTH; ++ix)
        {
            if (!fld[ix + iy * WIDTH].isnan())
            fldY[ix + iy * WIDTH] = sqrt(fld[ix + iy * WIDTH].x * fld[ix + iy * WIDTH].x + fld[ix + iy * WIDTH].y * fld[ix + iy * WIDTH].y) *
                (fld[ix + iy * WIDTH].x > 0.0 || fld[ix + iy * WIDTH].y > 0.0 )?1.0:-1.0;
        }
    }

    display(WIDTH, HEIGHT, fldY);
}
