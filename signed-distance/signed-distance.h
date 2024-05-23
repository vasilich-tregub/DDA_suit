#pragma once
#include <vector>
#include <cassert>

void dt(int width, int height, std::vector<int>& distance)
{
    assert(distance.size() >= width * height);         // pos, neg endings in variable names indicate variables used in pos/neg distance areas
    std::vector<int> vpos(std::max(width, height));         // Indices of locations of visible parabola vertices (lowest values), ordered by q 
    std::vector<int> vneg(std::max(width, height));
    std::vector<double> zpos(std::max(width, height) + 1);  // Indices of locations of boundaries, ordered by increasing q 
    std::vector<double> zneg(std::max(width, height) + 1);
    int kpos = 0;                                      // Number of boundaries between parabolas; used to index v and z vectors
    int kneg = 0;
    std::vector<int> coldistpos(height);           // column data used when calculating vert distances
    std::vector<int> coldistneg(height);
    std::vector<int> rowdistpos(width);            // row data used when calculating horz distances
    std::vector<int> rowdistneg(width);

    // vertical run: calculate vertical distances
    for (int X = 0; X < width; ++X)
    {
        // store a copy of column of distances 
        // 'cause column data should be computed in
        // the final loop using unaltered data
        for (int Y = 0; Y < height; ++Y)
        {
            int distcurr = distance[Y * width + X];
            if (distcurr > 0)
            {
                coldistpos[Y] = distcurr;
                coldistneg[Y] = 0;
            }
            else
            {
                coldistpos[Y] = 0;
                coldistneg[Y] = -distcurr;
            }
        }

        kpos = 0;
        vpos[0] = 0;
        zpos[0] = SHRT_MIN;
        zpos[1] = SHRT_MAX;
        for (int q = 1; q <= height - 1; q++)
        {
            // compute Voronoi border: intersection of parabola at q with rightmost currently visible parabola 
            double spos = ((coldistpos[q] + q * q) - (coldistpos[vpos[kpos]] + vpos[kpos] * vpos[kpos])) / (2 * q - 2 * vpos[kpos]);
            // find the parabola that hides the rightmost parabola
            while (spos <= zpos[kpos]) {
                kpos--;
                spos = ((coldistpos[q] + q * q) - (coldistpos[vpos[kpos]] + vpos[kpos] * vpos[kpos])) / (2 * q - 2 * vpos[kpos]);
            }
            kpos++;
            vpos[kpos] = q;
            zpos[kpos] = spos;
            zpos[kpos + 1] = SHRT_MAX;
        }
        kneg = 0;
        vneg[0] = 0;
        zneg[0] = SHRT_MIN;
        zneg[1] = SHRT_MAX;
        for (int q = 1; q <= height - 1; q++)
        {
            // compute Voronoi border: intersection of parabola at q with rightmost currently visible parabola 
            double sneg = ((coldistneg[q] + q * q) - (coldistneg[vneg[kneg]] + vneg[kneg] * vneg[kneg])) / (2 * q - 2 * vneg[kneg]);
            // find the parabola that hides the rightmost parabola
            while (sneg <= zneg[kneg]) {
                kneg--;
                sneg = ((coldistneg[q] + q * q) - (coldistneg[vneg[kneg]] + vneg[kneg] * vneg[kneg])) / (2 * q - 2 * vneg[kneg]);
            }
            kneg++;
            vneg[kneg] = q;
            zneg[kneg] = sneg;
            zneg[kneg + 1] = SHRT_MAX;
        }

        kpos = 0;
        kneg = 0;
        // the final loop: for v[k] < q we use unaltered column data
        for (int q = 0; q <= height - 1; q++)
        {
            while (zpos[kpos + 1] < q)
                kpos++;
            while (zneg[kneg + 1] < q)
                kneg++;
            distance[q * width + X] = (q - vpos[kpos]) * (q - vpos[kpos]) + coldistpos[vpos[kpos]] -
                ((q - vneg[kneg]) * (q - vneg[kneg]) + coldistneg[vneg[kneg]]);
        }
    }

    // horizontal run: process distances row-wise

    for (int Y = 0; Y < height; ++Y)
    {
        // store a copy of row of distances 
        // 'cause row data should be computed in
        // the final loop using unaltered data
        for (int X = 0; X < width; ++X)
        {
            int distcurr = distance[Y * width + X];
            if (distcurr > 0)
            {
                rowdistpos[X] = distance[Y * width + X];
                rowdistneg[X] = 0;
            }
            else
            {
                rowdistpos[X] = 0;
                rowdistneg[X] = -distance[Y * width + X];
            }
        }

        kpos = 0;
        vpos[0] = 0;
        zpos[0] = SHRT_MIN;
        zpos[1] = SHRT_MAX;
        for (int q = 1; q <= width - 1; q++)
        {
            double spos = ((rowdistpos[q] + q * q) - (rowdistpos[vpos[kpos]] + vpos[kpos] * vpos[kpos])) / (2 * q - 2 * vpos[kpos]);
            while (spos <= zpos[kpos]) {
                kpos--;
                spos = ((rowdistpos[q] + q * q) - (rowdistpos[vpos[kpos]] + vpos[kpos] * vpos[kpos])) / (2 * q - 2 * vpos[kpos]);
            }
            kpos++;
            vpos[kpos] = q;
            zpos[kpos] = spos;
            zpos[kpos + 1] = SHRT_MAX;
        }

        kneg = 0;
        vneg[0] = 0;
        zneg[0] = SHRT_MIN;
        zneg[1] = SHRT_MAX;
        for (int q = 1; q <= width - 1; q++)
        {
            double sneg = ((rowdistneg[q] + q * q) - (rowdistneg[vneg[kneg]] + vneg[kneg] * vneg[kneg])) / (2 * q - 2 * vneg[kneg]);
            while (sneg <= zneg[kneg]) {
                kneg--;
                sneg = ((rowdistneg[q] + q * q) - (rowdistneg[vneg[kneg]] + vneg[kneg] * vneg[kneg])) / (2 * q - 2 * vneg[kneg]);
            }
            kneg++;
            vneg[kneg] = q;
            zneg[kneg] = sneg;
            zneg[kneg + 1] = SHRT_MAX;
        }

        kpos = 0;
        kneg = 0;
        // the final loop: for v[k] < q we use unaltered row data
        for (int q = 0; q <= width - 1; q++)
        {
            while (zpos[kpos + 1] < q)
                kpos++;
            while (zneg[kneg + 1] < q)
                kneg++;
            distance[Y * width + q] = (q - vpos[kpos]) * (q - vpos[kpos]) + rowdistpos[vpos[kpos]] -
                ((q - vneg[kneg]) * (q - vneg[kneg]) + rowdistneg[vneg[kneg]]);
        }
    }
}
