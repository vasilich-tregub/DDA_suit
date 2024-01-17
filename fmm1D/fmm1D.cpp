// fmm1D.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <format>
#include <fstream>

const char* HTMLheader = R"(<HTML>
<HEAD>
<META NAME="GENERATOR" Content="Microsoft Visual Studio">
<TITLE></TITLE>
    <style>
        .box {
            display: table;
        }
        .box > div {
            display: table-cell;
            width: 64px;
            height: 64px;
            box-sizing: border-box;
            padding: 10px;
            text-align: center;
            vertical-align: middle;
            border: 1px solid black;
        }
    </style>
</HEAD>
<BODY>
    <div class="box">
)";
const char* HTMLfooter = R"(
    </div>
</BODY>
</HTML>
)";

int main()
{
    std::string html = HTMLheader;
    for (int i = 0; i < 16; ++i)
    {
        html.append(std::format("<div>{}</div>\n", 0.5 * (8 - i)));
    }
    html.append(HTMLfooter);
    std::ofstream outf("page.html");
    outf << html;
    outf.close();
}
