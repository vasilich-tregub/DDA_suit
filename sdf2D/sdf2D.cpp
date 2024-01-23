// sdf2D.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <format>
#include <fstream>
#include <vector>
#include "sdf2D.h"
#include <cassert>
#include <array>

const int WIDTH = 32;
const int HEIGHT = 32;
double fld[HEIGHT][WIDTH]{};
int trait[HEIGHT][WIDTH]{};

enum traits
{
    VOID, SOWN, SET, FIXED
};


int main()
{
    std::cout << fld[0][0] << std::endl;
}

