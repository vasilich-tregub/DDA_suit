// contour-viz.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "borderpolyline.h"
#include "display.h"

int main()
{
    std::size_t WIDTH = 16, HEIGHT = 16;
    std::vector<double> datavec(WIDTH * HEIGHT);
    constexpr double nan = std::numeric_limits<double>::quiet_NaN();
    for (int ix = 0; ix < WIDTH * HEIGHT; ++ix) datavec[ix] = nan;
    array2D<double> fld(datavec, WIDTH, HEIGHT);

    std::vector<std::pair<std::pair<int, int>, double>> va;
    drawEdge(2.8, 3.5, 6.9, 6.5, va, fld);
    std::cout << "Points in border line: " << va.size() << "\n";
    for (auto vec : va)
    {
        std::cout << "point " << vec.first.first << ", " << vec.first.second << "; value=" << vec.second << "\n";
    }
    display(WIDTH, HEIGHT, fld.buffer().data(), L"contourViz.png");
}

