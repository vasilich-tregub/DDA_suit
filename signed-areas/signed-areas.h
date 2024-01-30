#pragma once

void signed_areas(int WIDTH, int HEIGHT, int* distance)
{
    int width = WIDTH;
    int height = HEIGHT - 1;

    int* rowdistance = new int[width] {};

    int cyclecount = 0;

    // vertical run: calculate vertical distances
    for (int X = 0; X < width; ++X)
    {
        int step = 1;
        for (int Y = 1; Y < height; ++Y)
        {
            if (distance[Y * width + X] > distance[(Y - 1) * width + X])
            {
                distance[Y * width + X] = distance[(Y - 1) * width + X] + step;
                step += 2;
                ++cyclecount;
            }
            else
            {
                step = 1;
            }
        }
        step = 1;
        for (int Y = height - 2; Y > 0; --Y)
        {
            if (distance[Y * width + X] > distance[(Y + 1) * width + X])
            {
                distance[Y * width + X] = distance[(Y + 1) * width + X] + step;
                step += 2;
                ++cyclecount;
            }
            else
            {
                step = 1;
            }
        }
    }
    // horizontal run: process distances row-wise
    int* js = new int[width + 1];
    for (int X = 0; X < width + 1; ++X) js[X] = 0;
    int* ks = new int[width];
    for (int X = 0; X < width; ++X) ks[X] = 0;

    for (int Y = 0; Y < height; ++Y)
    {
        for (int X = 0; X < width; ++X)
        {
            rowdistance[X] = distance[Y * width + X];
            ++cyclecount;
        }
        int idx = 0; // current parabola index
        js[0] = -(width * width + height * height);
        ks[0] = 0;
        int m = 0; // 'global' (i.e., outside loops) column index
        while (m < width - 1)
        {
            ++m;
            if (rowdistance[m] < (width * width + height * height))
            {
                // compute js with prev parabola
                int k = ks[idx];
                int j =
                    (int)ceil((rowdistance[m] - rowdistance[k] - k * k + m * m) /
                        (2.0 * (m - k)));
                while (j <= js[idx])
                {
                    idx--;
                    k = ks[idx];
                    j =
                        (int)ceil((rowdistance[m] - rowdistance[k] - k * k + m * m) /
                            (2.0 * (m - k)));
                }
                if (j < width)
                {
                    idx++;
                    js[idx] = std::max(0, j);
                    ks[idx] = m;
                }
            }
        }
        js[0] = 0;
        js[idx + 1] = width + 1;
        for (int n = 0; n < idx; ++n)
        {
            int k = ks[n];
            for (int xX = js[n]; xX < js[n + 1] - 1; ++xX)
            {
                distance[Y * width + xX] = rowdistance[xX] + (xX - k) * (xX - k);
                ++cyclecount;
            }
        }
    }
    delete js;
    delete ks;
    delete rowdistance;

    std::cout << cyclecount << std::endl;
}