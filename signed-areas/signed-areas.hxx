#pragma once

void signed_areas(int WIDTH, int HEIGHT, int* distance)
{
    int width = WIDTH;
    int height = HEIGHT - 1;
    int ndots = width * height;
    int* coldistance = new int[height] {};
    int* rowdistance = new int[width] {};

    int cyclecount = 0;

    // vertical run: calculate vertical distances
    for (int X = 0; X < width; ++X)
    {
        // store a copy of column of distances 
        // because the next loop contains inner loop over the row index
        // and can modify the column data
        for (int Y = 0; Y < height; ++Y)
        {
            coldistance[Y] = distance[Y * width + X];
            ++cyclecount;
        }
        int* v = new int[height];
        double* z = new double[height + 1];
        int k = 0;
        v[0] = 0;
        z[0] = SHRT_MIN;
        z[1] = SHRT_MAX;
        for (int q = 1; q <= height - 1; q++)
        {
            double s = ((coldistance[q] + q * q) - (coldistance[v[k]] + v[k] * v[k])) / (2 * q - 2 * v[k]);
            while (s <= z[k]) {
                k--;
                s = ((coldistance[q] + q * q) - (coldistance[v[k]] + v[k] * v[k])) / (2 * q - 2 * v[k]);
                ++cyclecount;
            }
            k++;
            v[k] = q;
            z[k] = s;
            z[k + 1] = SHRT_MAX;
            ++cyclecount;
        }

        k = 0;
        for (int q = 0; q <= height - 1; q++)
        {
            while (z[k + 1] < q)
                k++;
            distance[q * width + X] = (q - v[k]) * (q - v[k]) + coldistance[v[k]];
            ++cyclecount;
        }

        delete[] v;
        delete[] z;

        // horizontal run: process distances row-wise

        for (int Y = 0; Y < height; ++Y)
        {
            // store a copy of row of distances 
            // because the next loop contains inner loop over the column index
            // and can modify the row data
            for (int X = 0; X < width; ++X)
            {
                rowdistance[X] = distance[Y * width + X];
                ++cyclecount;
            }
            int* v = new int[width];
            double* z = new double[width + 1];
            int k = 0;
            v[0] = 0;
            z[0] = SHRT_MIN;
            z[1] = SHRT_MAX;
            for (int q = 1; q <= width - 1; q++)
            {
                double s = ((rowdistance[q] + q * q) - (rowdistance[v[k]] + v[k] * v[k])) / (2 * q - 2 * v[k]);
                while (s <= z[k]) {
                    k--;
                    s = ((rowdistance[q] + q * q) - (rowdistance[v[k]] + v[k] * v[k])) / (2 * q - 2 * v[k]);
                    ++cyclecount;
                }
                k++;
                v[k] = q;
                z[k] = s;
                z[k + 1] = SHRT_MAX;
                ++cyclecount;
            }

            k = 0;
            for (int q = 0; q <= width - 1; q++)
            {
                while (z[k + 1] < q)
                    k++;
                distance[Y * width + q] = (q - v[k]) * (q - v[k]) + rowdistance[v[k]];
                ++cyclecount;
            }

            delete[] v;
            delete[] z;
        }
    }
    std::cout << cyclecount << std::endl;
}