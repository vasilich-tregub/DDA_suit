#pragma once
#include <iostream>

constexpr int WIDTH = 32;
constexpr int HEIGHT = 32;

struct vec
{
    double x; double y;
    vec operator+(vec other)
    {
        return { x + other.x, y + other.y };
    }
    vec operator-(vec other)
    {
        return { x - other.x, y - other.y };
    }
    vec operator*(double z)
    {
        return { x * z, y * z };
    }
    vec operator/(double z)
    {
        return { x / z, y / z };
    }
    bool operator!=(vec other)
    {
        return (x != other.x || y != other.y);
    }
    bool isnan()
    {
        return (std::isnan(x) || std::isnan(y));
    }
};

double fldY[WIDTH * HEIGHT]{};

vec fld[WIDTH * HEIGHT]{};
uint8_t fldtrait[WIDTH * HEIGHT]{};

void setPixel(int x, int y, vec displacement)
{
    if ((fld[x + y * WIDTH]).isnan())
    {
        fld[x + y * WIDTH] = displacement;
    }
}

vec getPixel(int x, int y)
{
    return fld[x + y * WIDTH];
}

void iterateStencil(int ix, int iy)
{
    vec mindist = { sqrt((double)(WIDTH * WIDTH + HEIGHT * HEIGHT)), sqrt((double)(WIDTH * WIDTH + HEIGHT * HEIGHT)) };
    vec mindist1 = { sqrt((double)(WIDTH * WIDTH + HEIGHT * HEIGHT)), sqrt((double)(WIDTH * WIDTH + HEIGHT * HEIGHT)) };
    if (!(getPixel(ix - 1, iy)).isnan())
    {
        mindist = getPixel(ix - 1, iy);
    }
    else if (!(getPixel(ix + 1, iy)).isnan())
    {
        mindist = getPixel(ix + 1, iy);
    }
    else if (!(getPixel(ix, iy - 1)).isnan())
    {
        mindist = getPixel(ix, iy - 1);
    }
    else if (!(getPixel(ix, iy + 1)).isnan())
    {
        mindist = getPixel(ix, iy + 1);
    }

    if (mindist != mindist1)
        setPixel(ix, iy, mindist);
}

void init(int WIDTH, int HEIGHT, vec* fld)
{
    for (int iy = 1; iy < HEIGHT; ++iy)
    {
        for (int ix = 1; ix < WIDTH; ++ix)
        {
            if (iy == HEIGHT / 4 || iy == 3 * HEIGHT / 4)
                if (ix >= WIDTH / 4 && ix <= 3 * WIDTH / 4)
                {
                    fld[ix + iy * WIDTH] = { 1.0, 0.0 };
                }
            if (iy == HEIGHT / 4 + 1 || iy == 3 * HEIGHT / 4 - 1)
                if (ix >= WIDTH / 4 + 1 && ix <= 3 * WIDTH / 4 - 1)
                {
                    fld[ix + iy * WIDTH] = { -1.0, 0.0 };
                }
            if (ix == WIDTH / 4 || ix == 3 * WIDTH / 4)
                if (iy >= HEIGHT / 4 && iy <= 3 * HEIGHT / 4)
                {
                    fld[ix + iy * WIDTH] = { 0.0, 1.0 };
                }
            if (ix == WIDTH / 4 + 1 || ix == 3 * WIDTH / 4 - 1)
                if (iy >= HEIGHT / 4 + 1 && iy <= 3 * HEIGHT / 4 - 1)
                {
                    fld[ix + iy * WIDTH] = { 0.0, -1.0 };
                }
        }
    }
}
