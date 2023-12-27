// flattenham.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "display.h"

constexpr int WIDTH = 32;
constexpr int HEIGHT = 32;

double fld[WIDTH * HEIGHT]{};

void setPixel(int x, int y, double Y = 0)
{
    if (std::isnan(fld[x + y * WIDTH]) || (abs(Y) < abs(fld[x + y * WIDTH])))
    {
        fld[x + y * WIDTH] = Y;
    }
}

void plotLine(double x0, double y0, double x1, double y1)
{
    int ix0 = std::floor(x0);
    int iy0 = std::floor(y0);
    int ix1 = std::ceil(x1);
    int iy1 = std::floor(y1);
    int idx = abs(ix1 - ix0), sx = ix0 < ix1 ? 1 : -1;
    int idy = -abs(iy1 - iy0), sy = iy0 < iy1 ? 1 : -1;
    int err = idx + idy, e2; // error value e_xy 

    double dy = (y1 - y0);
    double dx = (x1 - x0);

    double dist = (dx * (y0 - iy0) - dy * (x0 - ix0)) / sqrt(dx * dx + dy * dy);
    for (;;) {
        setPixel(ix0, iy0, dist);
        e2 = 2 * err;
        if (e2 >= idy) {
            if (ix0 == ix1) break;
            err += idy; ix0 += sx;
        }
        if (e2 <= idx) {
            if (iy0 == iy1) break;
            err += idx; iy0 += sy;
        }
        dist = (dx * (y0 - iy0) - dy * (x0 - ix0)) / sqrt(dx * dx + dy * dy);
        if (dist > 0)
        {
            double dist0 = (dx * (y0 - iy0 - 1) - dy * (x0 - ix0)) / sqrt(dx * dx + dy * dy);
            if (dist0 > 0) dist0 = +1; else if (dist0 < 0) dist0 = -1;
            setPixel(ix0, iy0 + 1, dist0);
            dist = +1;
        }
        else if (dist < 0)
        {
            double dist0 = (dx * (y0 - iy0 + 1) - dy * (x0 - ix0)) / sqrt(dx * dx + dy * dy);
            if (dist0 > 0) dist0 = +1; else if (dist0 < 0) dist0 = -1;
            setPixel(ix0, iy0 - 1, dist0);
            dist = -1;
        }
    }
}

enum driveaxis { X, Y };
struct inode { int x; int y; };
struct vec
{
    double x; double y;
    vec operator*(double z)
    {
        return { x * z, y * z };
    };
    vec operator/(double z)
    {
        return { x / z, y / z };
    };
};
double cross(vec v1, vec v2)
{
    return (v1.x * v2.y - v1.y * v2.x);
}
vec tricross(vec v1, vec v2, vec v3)
{
    return { (v2.x * v3.y - v2.y * v3.x) * v1.y, (v2.y * v3.x - v2.x * v3.y) * v1.x };
}
void boundaryLine(double x0, double y0, double x1, double y1)
{
    int ix0 = std::floor(x0);
    int iy0 = std::floor(y0);
    int ix1 = std::floor(x1);
    int iy1 = std::floor(y1);
    vec linevec{ (x1 - x0), (y1 - y0) };
    double seglen2 = linevec.x * linevec.x + linevec.y * linevec.y;// dx* dx + dy * dy;
    double seglen = sqrt(seglen2);
    std::cout << seglen << std::endl;
    // 1st endpoint
    std::cout << "1st enpoint\n";
    for (int j = 0; j < 2; ++j)
        for (int i = 0; i < 2; ++i)
        {
            std::cout << i << j << std::endl;
            vec nv0{ (x0 - ix0 - i), (y0 - iy0 - j) }; // 2D vector from {ix0 + i, iy0 + j} to {x0,y0}
            // cross product, the (signed) area and the distance to the line (linevec/seglen is unit vector)
            double d2l = cross(linevec, nv0) / seglen;
            std::cout << "d2l: " << d2l << std::endl;
            // triple cross product and the vector to nv0 proj to linevec wrt {ix0+i,iy0+j}
            vec proj = tricross(linevec, nv0, linevec) / seglen2;
            std::cout << proj.x << ", " << proj.y << std::endl;
            if (((proj.x < (x0 - ix0 - i) && proj.x < (x1 - ix0 - i)) || (proj.x > (x0 - ix0 - i) && proj.x > (x1 - ix0 - i))) ||
                ((proj.y < (y0 - iy0 - j) && proj.y < (y1 - ix0 - j)) || (proj.y > (y0 - iy0 - j) && proj.y > (y1 - iy0 - j))))
                d2l = sqrt((x0 - ix0 - i) * (x0 - ix0 - i) + (y0 - iy0 - j) * (y0 - iy0 - j)) * ((d2l >= 0) ? 1.0 : -1.0);
            std::cout << "d2ep0: " << d2l << std::endl;
            setPixel(ix0 + i, iy0 + j, d2l);
        }
    // 2nd endpoint
    std::cout << "2nt enpoint\n";
    for (int j = 0; j < 2; ++j)
        for (int i = 0; i < 2; ++i)
        {
            std::cout << i << j << std::endl;
            vec nv1{ (x1 - ix1 - i), (y1 - iy1 - j) }; // 2D vector from {ix1 + i, iy1 + j} to {x1,y1}
            // cross product, the (signed) area and the distance to the line (linevec/seglen is unit vector)
            double d2l = cross(linevec, nv1) / seglen;
            std::cout << "d2l: " << d2l << std::endl;
            // triple cross product and the vector to nv0 proj to linevec wrt {ix0+i,iy0+j}
            vec proj = tricross(linevec, nv1, linevec) / seglen2;
            std::cout << proj.x << ", " << proj.y << std::endl;
            if (((proj.x < (x1 - ix1 - i) && proj.x < (x0 - ix1 - i)) || (proj.x > (x1 - ix1 - i) && proj.x > (x0 - ix1 - i))) ||
                ((proj.y < (y1 - iy1 - j) && proj.y < (y0 - ix1 - j)) || (proj.y > (y1 - iy1 - j) && proj.y > (y0 - iy1 - j))))
                d2l = sqrt((x1 - ix1 - i) * (x1 - ix1 - i) + (y1 - iy1 - j) * (y1 - iy1 - j)) * ((d2l >= 0) ? 1.0 : -1.0);
            std::cout << "d2ep0: " << d2l << std::endl;
            setPixel(ix1 + i, iy1 + j, d2l);
        }
    // connecting line
    // exit from ep0
    // line intersection with grid
    double yintersect = HEIGHT;
    double xintersect = WIDTH;
    bool intersectfound = false;
    inode nextnode{ WIDTH, HEIGHT };
    if (linevec.x != 0)
    {
        if ((x1 <= ix0 && ix0 <= x0) || (x0 <= ix0 && ix0 <= x1))
        {
            yintersect = linevec.y / linevec.x * (ix0 - x0) + y0;
            xintersect = ix0;
            nextnode = { ix0 - 1, iy0 };
        }
        else if ((x1 <= ix0 + 1 && ix0 + 1 <= x0) || (x0 <= ix0 + 1 && ix0 + 1 <= x1))
        {
            yintersect = linevec.y / linevec.x * (ix0 + 1 - x0) + y0;
            xintersect = ix0 + 1;
            nextnode = { ix0 + 1, iy0 };
        }
        if (yintersect < iy0 || yintersect >= iy0 + 1)
        {
            xintersect = WIDTH;
            yintersect = HEIGHT;
            nextnode = { WIDTH, HEIGHT };
        }
        else
            intersectfound = true;
    }
    if (linevec.y != 0 && !intersectfound)
    {
        if ((y1 <= iy0 && iy0 <= y0) || (y0 <= iy0 && iy0 <= y1))
        {
            xintersect = linevec.x / linevec.y * (iy0 - y0) + x0;
            yintersect = iy0;
            nextnode = { ix0, iy0 - 1 };
        }
        else if ((y1 <= iy0 + 1 && iy0 + 1 <= y0) || (y0 <= iy0 + 1 && iy0 + 1 <= y1))
        {
            xintersect = linevec.x / linevec.y * (iy0 + 1 - y0) + x0;
            yintersect = iy0 + 1;
            nextnode = { ix0, iy0 + 1 };
        }
        if (xintersect < ix0 || xintersect >= ix0 + 1)
        {
            xintersect = WIDTH;
            yintersect = HEIGHT;
            nextnode = { WIDTH, HEIGHT };
        }
        else
            intersectfound = true;
    }
    std::cout << "Intersect: " << xintersect << " " << yintersect << std::endl;
    driveaxis drax = (abs(linevec.y) < abs(linevec.x)) ? X : Y;
    double grad = (drax == X) ? (linevec.y / linevec.x) : (linevec.x / linevec.y);
    std::cout << "Grad: " << grad << std::endl;
    if (drax == X)
        for (int x = std::floor(xintersect); x <= ix1; ++x)
        {
            //std::cout << "Yintersect: " << yintersect << std::endl;
            vec nv{ (x0 - x), (y0 - std::floor(yintersect) - 1) };
            //std::cout << "nv: " << x << " " << std::floor(yintersect) << std::endl;
            // cross product, the (signed) area and the distance to the line (linevec/seglen is unit vector)
            double d2l = cross(linevec, nv) / seglen;
            setPixel(x, std::floor(yintersect) + 1, d2l);
            vec nv1{ (x0 - x), (y0 - std::floor(yintersect)) };
            //std::cout << "nv1: " << x << " " << std::floor(yintersect) + 1 << std::endl;
            double d2l1 = cross(linevec, nv1) / seglen;
            setPixel(x, std::floor(yintersect), d2l1);
            std::cout << d2l << " " << d2l1 << std::endl;
            yintersect = yintersect + grad;
        }

}

int main()
{
    constexpr double nan = std::numeric_limits<double>::quiet_NaN();
    for (int ix = 0; ix < WIDTH * HEIGHT; ++ix) fld[ix] = nan;

    boundaryLine(0.75, 5.125, 6.625, 0.25);
    display(WIDTH, HEIGHT, fld);
}

