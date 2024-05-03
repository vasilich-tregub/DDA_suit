// contour-viz.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "borderpolyline.h"

int main()
{
        //ep0 = std::make_pair(2.3f, 3.5f),        ep1 = std::make_pair(21.7f, 14.1f); va.size() 42
        //ep0 = std::make_pair(2.3f, 3.5f),  
        //ep2 = std::make_pair(3.4f, 7.15f),
        //ep3 = std::make_pair(2.3f, 3.5f);

    std::size_t WIDTH = 16, HEIGHT = 16;
    std::vector<double> datavec(WIDTH * HEIGHT);
    array2D<double> fld(datavec, WIDTH, HEIGHT);

    std::vector<std::pair<std::pair<int, int>, double>> va;
    /*auto
        ep0 = std::make_pair(2.3f, 3.5f),
        ep1 = std::make_pair(7.7f, 5.4f),
        ep2 = std::make_pair(3.4f, 9.1f);
    drawLine(ep0.first, ep0.second, ep1.first, ep1.second, va);
    std::cout << "Points in border line: " << va.size() << "\n";
    std::cout << "endpoint0 " << ep1.first << ", " << ep1.second << "\n";
    for (auto vec : va)
    {
        std::cout << "point " << vec.first.first << ", " << vec.first.second << "; value=" << vec.second << "\n";
    }
    va.clear();
    std::cout << "endpoint1 " << ep1.first << ", " << ep1.second << "\n";
    drawLine(ep1.first, ep1.second, ep2.first, ep2.second, va);
    std::cout << "Points in border line: " << va.size() << "\n";
    for (auto vec : va)
    {
        std::cout << "point " << vec.first.first << ", " << vec.first.second << "; value=" << vec.second << "\n";
    }
    va.clear();
    std::cout << "endpoint2 " << ep2.first << ", " << ep2.second << "\n";
    drawLine(ep2.first, ep2.second, ep0.first, ep0.second, va);
    std::cout << "Points in border line: " << va.size() << "\n";
    for (auto vec : va)
    {
        std::cout << "point " << vec.first.first << ", " << vec.first.second << "; value=" << vec.second << "\n";
    }*/ 
    // NEXT TEST WITH swapped = true; `drawLine(2.8, 3.5, 6.2, 7.5, va);` or simply drawLine(5, 3.5, 5, 6.5, va);
    // but first with drawLine(2.8, 3.5, 6.4, 6.5, va); : one of ep2 points is on line (continuation)? 
    drawEdge(2.8, 3.5, 6.1, 6.5, va, fld);
    std::cout << "Points in border line: " << va.size() << "\n";
    for (auto vec : va)
    {
        std::cout << "point " << vec.first.first << ", " << vec.first.second << "; value=" << vec.second << "\n";
    }
}

