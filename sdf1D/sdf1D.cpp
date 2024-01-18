// sdf1D.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <iostream>
#include <format>
#include <fstream>
#include "sdf1D.h"

const int WIDTH = 32;
double fld[WIDTH];

int main()
{
    constexpr double nan = std::numeric_limits<double>::quiet_NaN();
    for (int i = 0; i < WIDTH; ++i)
    {
        fld[i] = nan;
    }
    fld[WIDTH / 2] = 0.3333;
    fld[WIDTH / 2] = -0.6667;
    for (int i = 1; i < WIDTH; ++i)
    {
        if (std::isnan(fld[i]) && !std::isnan(fld[i - 1]))
            fld[i] = fld[i - 1] - 1.0;
    }
    for (int i = WIDTH - 2; i >= 0; --i)
    {
        if (std::isnan(fld[i]) && !std::isnan(fld[i + 1]))
            fld[i] = fld[i + 1] + 1.0;
    }
    std::string html = HTMLheader;
    for (int i = 0; i < WIDTH; ++i)
    {
        html.append(std::format("<div title=\"{}\">{:.3f}</div>\n", fld[i], fld[i]));
    }
    html.append(HTMLfooter);
    std::ofstream outf("page.html");
    outf << html;
    outf.close();
}
