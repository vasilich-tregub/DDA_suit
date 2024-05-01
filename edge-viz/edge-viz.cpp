// edge-viz.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "borderline.h"

int main()
{
    std::vector<std::pair<std::pair<int, int>, double>> va;
    drawLine(3.0, 3.1, 7.0, 6.1, va);
    std::cout << "Points in border line: " << va.size() << "\n";
    for (auto vec : va)
    {
        std::cout << "point " << vec.first.first << ", " << vec.first.second << "; value=" << vec.second << "\n";
    }
    va.clear();
    drawLine(3.501, 3.1, 7.501, 7.099, va);
    std::cout << "Points in border line: " << va.size() << "\n";
    for (auto vec : va)
    {
        std::cout << "point " << vec.first.first << ", " << vec.first.second << "; value=" << vec.second << "\n";
    }
    va.clear();
    drawLine(3.501, 3.1, 7.501, 7.101, va);
    std::cout << "Points in border line: " << va.size() << "\n";
    for (auto vec : va)
    {
        std::cout << "point " << vec.first.first << ", " << vec.first.second << "; value=" << vec.second << "\n";
    }
    va.clear();
    drawLine(7.501, 7.101,3.501, 3.1,  va);
    std::cout << "Points in border line: " << va.size() << "\n";
    for (auto vec : va)
    {
        std::cout << "point " << vec.first.first << ", " << vec.first.second << "; value=" << vec.second << "\n";
    }
}