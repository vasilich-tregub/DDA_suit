#pragma once

#include "resource.h"
#include <cmath>
#include <limits>
#include <format>
#include <vector>

constexpr int WIDTH = 16;
constexpr int HEIGHT = 16;

double fld[WIDTH * HEIGHT]{};
uint8_t fldtrait[WIDTH * HEIGHT]{};

constexpr double pxlnan = std::numeric_limits<double>::quiet_NaN();

void setPixel(int x, int y, double Y = 0, uint8_t trait = 0)
{
    if (std::isnan(fld[x + y * WIDTH]) || (trait < fldtrait[x + y * WIDTH]) ||
        ((trait == fldtrait[x + y * WIDTH]) && (abs(Y) < abs(fld[x + y * WIDTH]))))
    {
        fld[x + y * WIDTH] = Y;
        fldtrait[x + y * WIDTH] = trait;
    }
}

enum celledge { LEFT, BOTTOM, RIGHT, TOP };
struct inode { int x; int y; celledge entry; }; // left/bottom coords + the border where the line enters the cell
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
double cross(vec v1, vec v2) // z-component of cross product, I use it to compute signed distance
{
    return (v1.x * v2.y - v1.y * v2.x);
}
vec tricross(vec v1, vec v2, vec v3) // triple cross product, I use it to find projection point, to verify if it falls inside line segment
{
    return { (v2.x * v3.y - v2.y * v3.x) * v1.y, (v2.y * v3.x - v2.x * v3.y) * v1.x };
}

extern bool inprocess;
extern double x0;
extern double yy0;
extern double x1;
extern double yy1;
extern double seglen, seglen2;
extern double yintersect;
extern double xintersect;
extern int ix0, iy0, ix1, iy1;
extern inode nextnode;
extern vec linevec;
extern bool intersectfound;
extern std::vector<POINT> vertices;

int continueBoundaryLine()
{
    // compute pixels between endpoints
    // 
    if ((linevec.x > 0 && nextnode.x < ix1) || (linevec.x < 0 && nextnode.x > ix1) ||
        (linevec.y > 0 && nextnode.y < iy1) || (linevec.y < 0 && nextnode.y > iy1))
    {
        int ix = nextnode.x, iy = nextnode.y;
        celledge edge = nextnode.entry;

        auto nodepair = std::pair<inode, inode>{ nextnode, nextnode };
        switch (edge)
        {
        case LEFT:
            nodepair.second = { ix, iy + 1 };
            break;
        case BOTTOM:
            nodepair.second = { ix + 1, iy };
            break;
        case RIGHT:
            nodepair = { {ix + 1, iy}, {ix + 1, iy + 1} };
            break;
        case TOP:
            nodepair = { {ix, iy + 1}, {ix + 1, iy + 1} };
            break;
        }

        for (inode sidenode : {nodepair.first, nodepair.second})
        {
            //TODO: skip the node of a pair that's been already visited in the preceding step
            vec nv{ (sidenode.x - x0), (sidenode.y - yy0) }; // 2D vector from {ix + i, iy + j} to {x0,y0}
            // cross product, the (signed) area and the distance to the line (linevec/seglen is unit vector)
            double d2l = cross(nv, linevec) / seglen;
            setPixel(sidenode.x, sidenode.y, d2l);
        }
        intersectfound = false;
        if (linevec.x != 0)
        {
            if ((edge != LEFT) && ((x0 < ix && ix <= x1)))
            {
                yintersect = linevec.y / linevec.x * (ix - x0) + yy0;
                xintersect = ix;
                if (yintersect > iy && yintersect < iy + 1)
                {
                    intersectfound = true;
                    nextnode = { ix - 1, iy, RIGHT };
                }
            }
            if ((edge != LEFT) && ((x1 <= ix && ix <= x0)))
            {
                yintersect = linevec.y / linevec.x * (ix - x0) + yy0;
                xintersect = ix;
                if (yintersect >= iy && yintersect <= iy + 1)
                {
                    intersectfound = true;
                    nextnode = { ix - 1, iy, RIGHT };
                }
            }
            // check if right boundary is crossed
            if (!intersectfound && ((edge != RIGHT) && ((x0 <= ix + 1 && ix + 1 <= x1))))
            {
                yintersect = linevec.y / linevec.x * (ix + 1 - x0) + yy0;
                xintersect = ix + 1;
                if (yintersect >= iy && yintersect <= iy + 1)
                {
                    intersectfound = true;
                    nextnode = { ix + 1, iy, LEFT };
                }
            }
            if (!intersectfound && ((edge != RIGHT) && ((x1 <= ix + 1 && ix + 1 <= x0))))
            {
                yintersect = linevec.y / linevec.x * (ix + 1 - x0) + yy0;
                xintersect = ix + 1;
                if (yintersect > iy && yintersect < iy + 1)
                {
                    intersectfound = true;
                    nextnode = { ix + 1, iy, LEFT };
                }
            }
            if (!intersectfound)
            {
                xintersect = WIDTH;
                yintersect = HEIGHT;
                nextnode = { WIDTH, HEIGHT };
            }
        }
        if (linevec.y != 0)
        {
            // check if bottom boundary is crossed EXCEPTIONAL CONDITION STRICTLY LESS !!!!!!!!!!!
            if ((edge != BOTTOM) && ((yy0 < iy && iy <= yy1)))
            {
                xintersect = linevec.x / linevec.y * (iy - yy0) + x0;
                yintersect = iy;
                if (xintersect > ix && xintersect < ix + 1)
                {
                    intersectfound = true;
                    nextnode = { ix, iy - 1, TOP };
                }
            }
            if ((edge != BOTTOM) && ((yy1 <= iy && iy <= yy0)))
            {
                xintersect = linevec.x / linevec.y * (iy - yy0) + x0;
                yintersect = iy;
                if (xintersect >= ix && xintersect <= ix + 1)
                {
                    intersectfound = true;
                    nextnode = { ix, iy - 1, TOP };
                }
            }
            // check if top boundary is crossed
            if (!intersectfound && ((edge != TOP) && ((yy0 <= iy + 1 && iy + 1 <= yy1))))
            {
                xintersect = linevec.x / linevec.y * (iy + 1 - yy0) + x0;
                yintersect = iy + 1;
                if (xintersect >= ix && xintersect <= ix + 1)
                {
                    intersectfound = true;
                    nextnode = { ix, iy + 1, BOTTOM };
                }
            }
            if (!intersectfound && ((edge != TOP) && ((yy1 <= iy + 1 && iy + 1 <= yy0))))
            {
                xintersect = linevec.x / linevec.y * (iy + 1 - yy0) + x0;
                yintersect = iy + 1;
                if (xintersect > ix && xintersect < ix + 1)
                {
                    intersectfound = true;
                    nextnode = { ix, iy + 1, BOTTOM };
                }
            }
            if (!intersectfound)
            {
                xintersect = WIDTH;
                yintersect = HEIGHT;
                nextnode = { WIDTH, HEIGHT };
            }
        }
        //std::cout << "Next node: " << nextnode.x << " " << nextnode.y << std::endl;
        return 0;
    }
    return 1;
}
void startBoundaryLine(double argx0, double argy0, double argx1, double argy1, bool clearFld=true)
{
    if (argx0 == argx1 && argy0 == argy1)
        return;
    if (clearFld)
    {
        for (int ix = 0; ix < WIDTH * HEIGHT; ++ix)
        {
            fld[ix] = pxlnan;
            fldtrait[ix] = 0;
        }
    }
    x0 = argx0;
    ix0 = (int)std::floor(x0);
    yy0 = argy0;
    iy0 = (int)std::floor(yy0);
    x1 = argx1;
    yy1 = argy1;
    ix1 = (int)std::floor(x1);
    iy1 = (int)std::floor(yy1);
    linevec = { (x1 - x0), (yy1 - yy0) };
    seglen2 = linevec.x * linevec.x + linevec.y * linevec.y;// dx* dx + dy * dy;
    seglen = sqrt(seglen2);
    // 1st endpoint
    for (int j = 0; j < 2; ++j)
        for (int i = 0; i < 2; ++i)
        {
            uint8_t trait = 0;
            //std::cout << i << j << std::endl;
            vec nv0{ (ix0 + i - x0), (iy0 + j - yy0) }; // 2D vector from {ix0 + i, iy0 + j} to {x0,y0}
            // cross product, the (signed) area and the distance to the line (linevec/seglen is unit vector)
            double d2l = cross(nv0, linevec) / seglen;
            //std::cout << "d2l: " << d2l << std::endl;
            // triple cross product and the vector to nv0 proj to linevec wrt {ix0+i,iy0+j}
            vec proj = tricross(linevec, linevec, nv0) / seglen2;
            //std::cout << proj.x << ", " << proj.y << std::endl;
            if (((proj.x < (x0 - ix0 - i) && proj.x < (x1 - ix0 - i)) || (proj.x > (x0 - ix0 - i) && proj.x > (x1 - ix0 - i))) ||
                ((proj.y < (yy0 - iy0 - j) && proj.y < (yy1 - iy0 - j)) || (proj.y > (yy0 - iy0 - j) && proj.y > (yy1 - iy0 - j))))
            {
                d2l = copysign(
                    sqrt((x0 - ix0 - i) * (x0 - ix0 - i) + (yy0 - iy0 - j) * (yy0 - iy0 - j)) * ((d2l >= 0) ? 1.0 : -1.0), 
                    d2l);
                trait = 1;
            }
            //std::cout << "d2ep0: " << d2l << std::endl;
            setPixel(ix0 + i, iy0 + j, d2l, trait);
        }
    // 2nd endpoint
    for (int j = 0; j < 2; ++j)
        for (int i = 0; i < 2; ++i)
        {
            uint8_t trait = 0;
            //std::cout << i << j << std::endl;
            vec nv1{ (ix1 + i - x1), (iy1 + j - yy1) }; // 2D vector from {ix1 + i, iy1 + j} to {x1,y1}
            // cross product, the (signed) area and the distance to the line (linevec/seglen is unit vector)
            double d2l = cross(nv1, linevec) / seglen;
            //std::cout << "d2l: " << d2l << std::endl;
            // triple cross product and the vector to nv0 proj to linevec wrt {ix0+i,iy0+j}
            vec proj = tricross(linevec, linevec, nv1) / seglen2;
            //std::cout << proj.x << ", " << proj.y << std::endl;
            if (((proj.x < (x1 - ix1 - i) && proj.x < (x0 - ix1 - i)) || (proj.x > (x1 - ix1 - i) && proj.x > (x0 - ix1 - i))) ||
                ((proj.y < (yy1 - iy1 - j) && proj.y < (yy0 - iy1 - j)) || (proj.y > (yy1 - iy1 - j) && proj.y > (yy0 - iy1 - j))))
            {
                d2l = copysign(
                    sqrt((x0 - ix0 - i) * (x0 - ix0 - i) + (yy0 - iy0 - j) * (yy0 - iy0 - j)) * ((d2l >= 0) ? 1.0 : -1.0),
                    d2l);
                trait = 1;
            }
            //std::cout << "d2ep0: " << d2l << std::endl;
            setPixel(ix1 + i, iy1 + j, d2l);
        }

    if (ix0 == ix1 && iy0 == iy1)
        return;
    // connecting line
    // line intersection with grid
    yintersect = HEIGHT;
    xintersect = WIDTH;
    intersectfound = false;
    if (linevec.x != 0)
    {
        // check if left boundary is crossed EXCEPTIONAL CONDITION STRICTLY LESS !!!!!!!!!!!
        if ((x1 <= ix0 && ix0 <= x0))
        {
            yintersect = linevec.y / linevec.x * (ix0 - x0) + yy0;
            xintersect = ix0;
            if (yintersect >= iy0 && yintersect <= iy0 + 1)
            {
                intersectfound = true;
                nextnode = { ix0 - 1, iy0, RIGHT };
            }
        }
        // check if right boundary is crossed
        if (!intersectfound && (x0 <= ix0 + 1 && ix0 + 1 <= x1))
        {
            yintersect = linevec.y / linevec.x * (ix0 + 1 - x0) + yy0;
            xintersect = ix0 + 1;
            if (yintersect >= iy0 && yintersect <= iy0 + 1)
            {
                intersectfound = true;
                nextnode = { ix0 + 1, iy0, LEFT };
            }
        }
        if (!intersectfound)
        {
            xintersect = WIDTH;
            yintersect = HEIGHT;
            nextnode = { WIDTH, HEIGHT };
        }
    }
    if (linevec.y != 0)
    {
        // check if bottom boundary is crossed EXCEPTIONAL CONDITION STRICTLY LESS !!!!!!!!!!!
        if ((yy1 <= iy0 && iy0 <= yy0))
        {
            xintersect = linevec.x / linevec.y * (iy0 - yy0) + x0;
            yintersect = iy0;
            if (xintersect >= ix0 && xintersect < ix0 + 1)
            {
                intersectfound = true;
                nextnode = { ix0, iy0 - 1, TOP };
            }
        }
        // check if top boundary is crossed
        if (!intersectfound && ((yy0 <= iy0 + 1 && iy0 + 1 <= yy1)))
        {
            xintersect = linevec.x / linevec.y * (iy0 + 1 - yy0) + x0;
            yintersect = iy0 + 1;
            if (xintersect >= ix0 && xintersect <= ix0 + 1)
            {
                intersectfound = true;
                nextnode = { ix0, iy0 + 1, BOTTOM };
            }
        }
        if (!intersectfound)
        {
            xintersect = WIDTH;
            yintersect = HEIGHT;
            nextnode = { WIDTH, HEIGHT };
        }
    }
    inprocess = true;
    /*while (inprocess)
    {
        inprocess = (0 == pixelsInBetween());
    }*/
}

void startPolyline()
{

}