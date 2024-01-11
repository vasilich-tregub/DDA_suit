#pragma once
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
    vec operator=(vec other)
    {
        return { other.x, other.y };
    }
    bool isnan()
    {
        return (std::isnan(x) || std::isnan(y));
    }
};

void init(int WIDTH, int HEIGHT, vec* fld)
{
    for (int iy = 1; iy < HEIGHT; ++iy)
    {
        for (int ix = 1; ix < WIDTH; ++ix)
        {
            if (iy == HEIGHT / 4 || iy == 3 * HEIGHT / 4)
                if (ix >= WIDTH / 4 && ix <= 3 * WIDTH / 4)
                    fld[ix + iy * WIDTH] = { 1.0, 0.0 };
            if (iy == HEIGHT / 4 + 1 || iy == 3 * HEIGHT / 4 - 1)
                if (ix >= WIDTH / 4 + 1 && ix <= 3 * WIDTH / 4 - 1)
                    fld[ix + iy * WIDTH] = { -1.0, 0.0 };
            if (ix == WIDTH / 4 || ix == 3 * WIDTH / 4)
                if (iy >= HEIGHT / 4 && iy <= 3 * HEIGHT / 4)
                    fld[ix + iy * WIDTH] = { 0.0, 1.0 };
            if (ix == WIDTH / 4 + 1 || ix == 3 * WIDTH / 4 - 1)
                if (iy >= HEIGHT / 4 + 1 && iy <= 3 * HEIGHT / 4 - 1)
                    fld[ix + iy * WIDTH] = { 0.0, -1.0 };
        }
    }
}