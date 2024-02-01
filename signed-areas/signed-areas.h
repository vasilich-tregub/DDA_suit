#pragma once

void signed_areas(int WIDTH, int HEIGHT, int* distance)
{
    int* rowdistance = new int[WIDTH] {};
    //int cyclecount = 0;

    // vertical run: calculate vertical distances
    for (int X = 0; X < WIDTH; ++X)
    {
        int step = 1;
        for (int Y = 1; Y < HEIGHT; ++Y)
        {
            if (distance[Y * WIDTH + X] > distance[(Y - 1) * WIDTH + X])
            {
                distance[Y * WIDTH + X] = distance[(Y - 1) * WIDTH + X] + step;
                step += 2;
                //++cyclecount;
            }
            else
            {
                step = 1;
            }
        }
        step = 1;
        for (int Y = HEIGHT - 2; Y >= 0; --Y)
        {
            if (distance[Y * WIDTH + X] > distance[(Y + 1) * WIDTH + X])
            {
                distance[Y * WIDTH + X] = distance[(Y + 1) * WIDTH + X] + step;
                step += 2;
                //++cyclecount;
            }
            else
            {
                step = 1;
            }
        }
    }

    // horizontal run: process distances row-wise
    int* ixsect = new int[WIDTH + 1];
    for (int X = 0; X < WIDTH + 1; ++X) ixsect[X] = 0;
    int* parapos = new int[WIDTH];
    for (int X = 0; X < WIDTH; ++X) parapos[X] = 0;
    for (int Y = 0; Y < HEIGHT; ++Y)
    {
        for (int X = 0; X < WIDTH; ++X)
        {
            rowdistance[X] = distance[Y * WIDTH + X];
            //++cyclecount;
        }
        int paraix = 0; // current parabola index
        ixsect[0] = -(WIDTH * WIDTH + HEIGHT * HEIGHT);
        parapos[0] = 0;
        int m = 0; // 'global' (i.e., outside loops) column index
        while (m < WIDTH - 1)
        {
            ++m;
            if (rowdistance[m] < (WIDTH * WIDTH + HEIGHT * HEIGHT))
            {
                // compute ixsect with prev parabola
                int k = parapos[paraix];
                int j =
                    (int)ceil((rowdistance[m] - rowdistance[k] - k * k + m * m) /
                        (2.0 * (m - k)));
                while (j <= ixsect[paraix])
                {
                    paraix--;
                    k = parapos[paraix];
                    j =
                        (int)ceil((rowdistance[m] - rowdistance[k] - k * k + m * m) /
                            (2.0 * (m - k)));
                }
                if (j <= WIDTH - 1)
                {
                    paraix++;
                    ixsect[paraix] = std::max(0, j);
                    parapos[paraix] = m;
                }
            }
        }
        ixsect[0] = 0;
        ixsect[paraix + 1] = WIDTH;
        for (int n = 0; n <= paraix; ++n)
        {
            int k = parapos[n];
            for (int xX = ixsect[n]; xX < ixsect[n + 1]; ++xX)
            {
                distance[Y * WIDTH + xX] = rowdistance[k] + (xX - k) * (xX - k);
                //++cyclecount;
            }
        }
    }
    delete[] ixsect;
    delete[] parapos;
    delete[] rowdistance;

    //std::cout << cyclecount << std::endl;
}
