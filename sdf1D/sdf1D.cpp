// sdf1D.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <iostream>
#include <format>
#include <fstream>
#include <vector>
#include "sdf1D.h"

const int WIDTH = 32;
double fld[WIDTH];
int trait[WIDTH];

enum traits
{
    VAR, SET, FIXED
};

struct D1_IC
{
    double left;
    double right;
    double dir;
};

int main()
{
    std::vector<D1_IC> ics{ {5.5, 26.5, 1.0},  {11.5, 21.5, -1.0}};
    for (int i = 0; i < WIDTH; ++i)
    {
        fld[i] = 0;
        trait[i] = VAR;
    }
    for (const auto& ic : ics)
    {
        int ix = (int)std::floor(ic.left);
        fld[ix] = ic.left - (double)ix;
        trait[ix] = FIXED;
        fld[ix + 1] = fld[ix] - ic.dir;
        trait[ix + 1] = FIXED;
        ix = (int)std::floor(ic.right);
        fld[ix] = ic.right - (double)ix;
        trait[ix] = FIXED;
        fld[ix + 1] = fld[ix] + ic.dir;
        trait[ix + 1] = FIXED;
    }
    /*fld[WIDTH / 2 - 1] = 0.3333;
    trait[WIDTH / 2 - 1] = FIXED;
    fld[WIDTH / 2] = -0.6667;
    trait[WIDTH / 2] = FIXED;*/
    for (int i = 1; i < WIDTH; ++i)
    {
        if (trait[i] == VAR && trait[i - 1] != VAR)
        {
            fld[i] = fld[i - 1] - 1.0; // replace '1.0' with a value inferred from ic.dir for this range
            trait[i] = SET;
        }
    }
    for (int i = WIDTH - 2; i >= 0; --i)
    {
        if (trait[i] == VAR && trait[i + 1] != VAR)
        {
            fld[i] = fld[i + 1] + 1.0; // replace '1.0' with a value inferred from ic.dir for this range
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
