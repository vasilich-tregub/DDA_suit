// sdf1D.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <iostream>
#include <format>
#include <fstream>
#include "sdf1D.h"

const int WIDTH = 32;
double fld[WIDTH];
int trait[WIDTH];

enum traits
{
    VAR, SET, FIXED
};

int main()
{
    for (int i = 0; i < WIDTH; ++i)
    {
        fld[i] = 0;
        trait[i] = VAR;
    }
    fld[WIDTH / 2 - 1] = 0.3333;
    trait[WIDTH / 2 - 1] = FIXED;
    fld[WIDTH / 2] = -0.6667;
    trait[WIDTH / 2] = FIXED;
    for (int i = 1; i < WIDTH; ++i)
    {
        if (trait[i] == VAR && trait[i - 1] != VAR)
        {
            fld[i] = fld[i - 1] - 1.0;
            trait[i] = SET;
        }
    }
    for (int i = WIDTH - 2; i >= 0; --i)
    {
        if (trait[i] == VAR && trait[i + 1] != VAR)
        {
            fld[i] = fld[i + 1] + 1.0;
            trait[i] = SET;
        }
    }
    std::string html = HTMLheader;
    for (int i = 0; i < WIDTH; ++i)
    {
        html.append(std::format("<div title=\"{}\">{:.3f}</div>\n", fld[i], fld[i]));
    }
    html.append(HTMLfooter);
    std::ofstream outf("sdfNumerical.html");
    outf << html;
    outf.close();
}
