#pragma once

void signed_areas(int WIDTH, int HEIGHT, int* distance)
{
    int width = WIDTH;
    int height = HEIGHT - 1;

    int* rowdistance = new int[width] {};

    int cyclecount = 0;

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
                ++cyclecount;
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
                ++cyclecount;
            }
            else
            {
                step = 1;
            }
        }
    }
    // horizontal run: process distances row-wise

    for (int Y = 0; Y <= height; ++Y)
    {
        for (int X = 0; X < width; ++X)
        {
            rowdistance[X] = distance[Y * width + X];
            ++cyclecount;
        }

        for (int X = 0; X < width; ++X)
        {
            int distMin = rowdistance[X];
            for (int k = 0; k < width; ++k)
            {
                int dist = rowdistance[k] + (k - X) * (k - X);
                if (distMin > dist)
                {
                    distMin = dist;
                }
                distance[Y * width + X] = distMin;
                ++cyclecount;
            }
        }

    }
    std::cout << cyclecount << std::endl;
}