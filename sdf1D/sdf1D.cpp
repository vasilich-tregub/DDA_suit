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
    VOID, SOWN, SET, FIXED
};

struct D1_IC
{
    double left;
    double right;
    double dir;
};

int main()
{
    // when ic.left and ic.right within the same pixel, do not flip fld sign at borders
    //std::vector<D1_IC> ics{ {3.33, 29.67, 1.0}, {6.25, 25.75, -1.0}, {16.25, 16.75, 1.0} };
    //std::vector<D1_IC> ics{ {5.5, 21.5, 1.0}, {24.25, 24.75, 0.0} };
    //std::vector<D1_IC> ics{ {16.25, 16.75, 0.0} };
    // process a border with endpoinds lying in the same pixel
    std::vector<D1_IC> ics{ {5.5, 15.5, 1.0}, {21.25, 21.75, 1.0} };
    //std::vector<D1_IC> ics{ {3.33, 29.67, 1.0}, {9.25, 24.75, -1.0}, {13.125, 20.875, 1.0} };
    //std::vector<D1_IC> ics{ {5.5, 26.5, 1.0},  {11.5, 21.5, -1.0} };
    //std::vector<D1_IC> ics{ {5.5, 13.5, 1.0}, {21.5, 27.5, 1.0} };
    //std::vector<D1_IC> ics{ {5.5, 26.5, 1.0} };
    for (int i = 0; i < WIDTH; ++i)
    {
        fld[i] = 0;
        trait[i] = VOID;
    }
    for (const auto& ic : ics)
    {
        int ix = (int)std::floor(ic.left);
        fld[ix] = ic.dir * (ic.left - (double)ix);
        trait[ix] = FIXED;
        fld[ix + 1] = fld[ix] - ic.dir;
        trait[ix + 1] = FIXED;
        ix = (int)std::floor(ic.right);
        fld[ix] = ic.dir * ((double)ix - ic.right);
        trait[ix] = FIXED;
        fld[ix + 1] = fld[ix] + ic.dir;
        trait[ix + 1] = FIXED;
    }
    for (int i = 1; i < WIDTH; ++i)
    {
        if (trait[i] == VOID && trait[i - 1] != VOID)
        {
            fld[i] = fld[i - 1] + (fld[i - 1] > 0 ? 1.0 : -1.0); // replace '1.0' with a value inferred from ic.dir for this range
            trait[i] = SOWN;
        }
        else if (trait[i] != FIXED && abs(fld[i]) >= abs(fld[i - 1]) + 1.0)
            fld[i] = fld[i - 1] + ((fld[i - 1] > 0) ? 1.0 : -1.0);
    }
    for (int i = WIDTH - 2; i >= 0; --i)
    {
        if (trait[i] == VOID && trait[i + 1] != VOID)
        {
            fld[i] = fld[i + 1] + (fld[i + 1] > 0 ? 1.0 : -1.0); // replace '1.0' with a value inferred from ic.dir for this range
            trait[i] = SOWN;
        }
        else if (trait[i] != FIXED && abs(fld[i]) >= abs(fld[i + 1]) + 1.0)
            fld[i] = fld[i + 1] + (fld[i + 1] > 0 ? 1.0 : -1.0);
    }
    std::string html = HTMLheader;
    for (int i = 0; i < WIDTH; ++i)
    {
        html.append(std::format("<div title=\"{}\">{:.3f}</div>\n", /*fld[i]*/i, fld[i]));
    }
    html.append(HTMLfooter);
    std::ofstream outf("sdfNumerical.html");
    outf << html;
    outf.close();
}
