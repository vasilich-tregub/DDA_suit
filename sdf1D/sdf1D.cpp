// sdf1D.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <iostream>
#include <format>
#include <fstream>
#include <vector>
#include "sdf1D.h"
#include <cassert>

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
    // when ic.left and ic.right within the same pixel, ic.left/right positions control 'distance' at borders 
    std::vector<D1_IC> ics{ {3.33, 29.67, 1.0}, {6.25, 25.75, -1.0}, {16.25, 16.75, 1.0} };
    //std::vector<D1_IC> ics{ {5.5, 21.5, 1.0}, {24.25, 24.75, 1.0} };
    // process a border with endpoinds lying in the adjacent pixels
    //std::vector<D1_IC> ics{ {5.5, 15.5, 1.0}, {21.25, 21.75, 1.0}, {22.25, 22.75, 1.0} };
    // process a border with endpoinds lying in the same pixel
    //std::vector<D1_IC> ics{ {16.999, 16.999, -1.0} };
    //std::vector<D1_IC> ics{ {5.5, 15.5, 1.0}, {21.25, 21.75, 1.0} };
    // non-adjacent, non-overlapping borders
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
        assert(ic.left <= ic.right);
        int ixl = (int)std::floor(ic.left);
        int ixr = (int)std::floor(ic.right);
        if (ixl < ixr - 1)
        {
            fld[ixl] = ic.dir * (ic.left - (double)ixl);
            trait[ixl] = FIXED;
            fld[ixl + 1] = fld[ixl] - ic.dir;
            trait[ixl + 1] = FIXED;
            fld[ixr] = ic.dir * ((double)ixr - ic.right);
            trait[ixr] = FIXED;
            fld[ixr + 1] = fld[ixr] + ic.dir;
            trait[ixr + 1] = FIXED;
        }
        else if (ixl == ixr - 1)
        {
            fld[ixl] = ic.dir * (ic.left - (double)ixl);
            trait[ixl] = FIXED;
            double fldmidleft = fld[ixl] - ic.dir;
            double fldmidright = ic.dir * ((double)ixr - ic.right);
            fld[ixl + 1] = 0.5 * (fldmidleft + fldmidright);
            trait[ixl + 1] = FIXED;
            fld[ixr + 1] = fldmidright + ic.dir;
            trait[ixr + 1] = FIXED;
        }
        else
        {
            fld[ixl] = ic.dir * (ic.left - (double)ixl);
            trait[ixl] = FIXED;
            double fldright = ic.dir * ((double)ixr - ic.right);
            fld[ixr + 1] = fldright + ic.dir;
            trait[ixr + 1] = FIXED;
        }
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
