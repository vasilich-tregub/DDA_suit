// flattenham.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "display.h"

constexpr int WIDTH = 128;
constexpr int HEIGHT = 128;

double fld[WIDTH * HEIGHT]{};

void setPixel(int x, int y, double Y = 0)
{
    /*double Yfld = fld[x + y * WIDTH];
    if (Y != 0 && Y + Yfld == 0)
    {
        fld[x + y * WIDTH] = std::numeric_limits<double>::quiet_NaN();
        return;
    }*/
    if (std::isnan(fld[x + y * WIDTH]) || (abs(Y) < abs(fld[x + y * WIDTH])))
    {
        fld[x + y * WIDTH] = Y;
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

vec priorlinevec{ 0, 0 }; // TODO: replace 'priorlinevec' tinkering with a test on position of a foot of normal vector from a current node to the line
void boundaryLine(double x0, double y0, double x1, double y1)
{
    if (x0 == x1 && y0 == y1)
        return;
    int ix0 = (int)std::floor(x0);
    int iy0 = (int)std::floor(y0);
    int ix1 = (int)std::floor(x1);
    int iy1 = (int)std::floor(y1);
    vec linevec = { (x1 - x0), (y1 - y0) };
    double seglen2 = linevec.x * linevec.x + linevec.y * linevec.y;// dx* dx + dy * dy;
    double seglen = sqrt(seglen2);
    inode nextnode{};
    // 1st endpoint
    double convexconcave = copysign(1.0, cross(priorlinevec ,linevec));
    for (int j = 0; j < 2; ++j)
        for (int i = 0; i < 2; ++i)
        {
            //std::cout << i << j << std::endl;
            vec nv0{ (ix0 + i - x0), (iy0 + j - y0) }; // 2D vector from {ix0 + i, iy0 + j} to {x0,y0}
            // cross product, the (signed) area and the distance to the line (linevec/seglen is unit vector)
            double d2l = cross(nv0, linevec) / seglen;
            //std::cout << "d2l: " << d2l << std::endl;
            // triple cross product and the vector to nv0 proj to linevec wrt {ix0+i,iy0+j}
            vec proj = tricross(linevec, linevec, nv0) / seglen2;
            //std::cout << proj.x << ", " << proj.y << std::endl;
            if (((proj.x < (x0 - ix0 - i) && proj.x < (x1 - ix0 - i)) || (proj.x > (x0 - ix0 - i) && proj.x > (x1 - ix0 - i))) ||
                ((proj.y < (y0 - iy0 - j) && proj.y < (y1 - iy0 - j)) || (proj.y > (y0 - iy0 - j) && proj.y > (y1 - iy0 - j))))
            {
                
                double d2lsign = copysign(1.0, d2l);
                if (d2lsign != convexconcave)
                    continue; // is it skipped because this distance is calculated in the iteration over nodes STRICTLY BETWEEN endpoints?
                d2l = copysign(
                    sqrt((x0 - ix0 - i) * (x0 - ix0 - i) + (y0 - iy0 - j) * (y0 - iy0 - j)),
                    d2l);
            }
            //std::cout << "d2ep0: " << d2l << std::endl;
            setPixel(ix0 + i, iy0 + j, d2l);
        }
    // 2nd endpoint
    for (int j = 0; j < 2; ++j)
        for (int i = 0; i < 2; ++i)
        {
            //std::cout << i << j << std::endl;
            vec nv1{ (ix1 + i - x1), (iy1 + j - y1) }; // 2D vector from {ix1 + i, iy1 + j} to {x1,y1}
            // cross product, the (signed) area and the distance to the line (linevec/seglen is unit vector)
            double d2l = cross(nv1, linevec) / seglen;
            //std::cout << "d2l: " << d2l << std::endl;
            // triple cross product and the vector to nv0 proj to linevec wrt {ix0+i,iy0+j}
            vec proj = tricross(linevec, linevec, nv1) / seglen2;
            //std::cout << proj.x << ", " << proj.y << std::endl;
            if (((proj.x < (x1 - ix1 - i) && proj.x < (x0 - ix1 - i)) || (proj.x > (x1 - ix1 - i) && proj.x > (x0 - ix1 - i))) ||
                ((proj.y < (y1 - iy1 - j) && proj.y < (y0 - iy1 - j)) || (proj.y > (y1 - iy1 - j) && proj.y > (y0 - iy1 - j))))
            {
                double d2lsign = copysign(1.0, d2l);
                if (d2lsign != convexconcave)
                    continue;
                d2l = copysign(
                    sqrt((x0 - ix0 - i) * (x0 - ix0 - i) + (y0 - iy0 - j) * (y0 - iy0 - j)),
                    d2l);
            }
            //std::cout << "d2ep0: " << d2l << std::endl;
            setPixel(ix1 + i, iy1 + j, d2l);
        }

    if (ix0 == ix1 && iy0 == iy1)
        return;
    // connecting line
    // line intersection with grid
    double yintersect = HEIGHT;
    double xintersect = WIDTH;
    bool intersectfound = false;
    if (linevec.x != 0)
    {
        // check if left boundary is crossed EXCEPTIONAL CONDITION STRICTLY LESS !!!!!!!!!!!
        if ((x1 <= ix0 && ix0 <= x0))
        {
            yintersect = linevec.y / linevec.x * (ix0 - x0) + y0;
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
            yintersect = linevec.y / linevec.x * (ix0 + 1 - x0) + y0;
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
        if ((y1 <= iy0 && iy0 <= y0))
        {
            xintersect = linevec.x / linevec.y * (iy0 - y0) + x0;
            yintersect = iy0;
            if (xintersect >= ix0 && xintersect < ix0 + 1)
            {
                intersectfound = true;
                nextnode = { ix0, iy0 - 1, TOP };
            }
        }
        // check if top boundary is crossed
        if (!intersectfound && ((y0 <= iy0 + 1 && iy0 + 1 <= y1)))
        {
            xintersect = linevec.x / linevec.y * (iy0 + 1 - y0) + x0;
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
    //std::cout << "Next node: " << nextnode.x << " " << nextnode.y << std::endl;

    // compute pixels between endpoints
    // 
    while ((linevec.x > 0 && nextnode.x < ix1) || (linevec.x < 0 && nextnode.x > ix1) ||
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
            vec nv{ (sidenode.x - x0), (sidenode.y - y0) }; // 2D vector from {ix + i, iy + j} to {x0,y0}
            // cross product, the (signed) area and the distance to the line (linevec/seglen is unit vector)
            double d2l = cross(nv, linevec) / seglen;
            setPixel(sidenode.x, sidenode.y, d2l);
        }
        intersectfound = false;
        if (linevec.x != 0)
        {
            if ((edge != LEFT) && ((x0 < ix && ix <= x1)))
            {
                yintersect = linevec.y / linevec.x * (ix - x0) + y0;
                xintersect = ix;
                if (yintersect > iy && yintersect < iy + 1)
                {
                    intersectfound = true;
                    nextnode = { ix - 1, iy, RIGHT };
                }
            }
            if ((edge != LEFT) && ((x1 <= ix && ix <= x0)))
            {
                yintersect = linevec.y / linevec.x * (ix - x0) + y0;
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
                yintersect = linevec.y / linevec.x * (ix + 1 - x0) + y0;
                xintersect = ix + 1;
                if (yintersect >= iy && yintersect <= iy + 1)
                {
                    intersectfound = true;
                    nextnode = { ix + 1, iy, LEFT };
                }
            }
            if (!intersectfound && ((edge != RIGHT) && ((x1 <= ix + 1 && ix + 1 <= x0))))
            {
                yintersect = linevec.y / linevec.x * (ix + 1 - x0) + y0;
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
            if ((edge != BOTTOM) && ((y0 < iy && iy <= y1)))
            {
                xintersect = linevec.x / linevec.y * (iy - y0) + x0;
                yintersect = iy;
                if (xintersect > ix && xintersect < ix + 1)
                {
                    intersectfound = true;
                    nextnode = { ix, iy - 1, TOP };
                }
            }
            if ((edge != BOTTOM) && ((y1 <= iy && iy <= y0)))
            {
                xintersect = linevec.x / linevec.y * (iy - y0) + x0;
                yintersect = iy;
                if (xintersect >= ix && xintersect <= ix + 1)
                {
                    intersectfound = true;
                    nextnode = { ix, iy - 1, TOP };
                }
            }
            // check if top boundary is crossed
            if (!intersectfound && ((edge != TOP) && ((y0 <= iy + 1 && iy + 1 <= y1))))
            {
                xintersect = linevec.x / linevec.y * (iy + 1 - y0) + x0;
                yintersect = iy + 1;
                if (xintersect >= ix && xintersect <= ix + 1)
                {
                    intersectfound = true;
                    nextnode = { ix, iy + 1, BOTTOM };
                }
            }
            if (!intersectfound && ((edge != TOP) && ((y1 <= iy + 1 && iy + 1 <= y0))))
            {
                xintersect = linevec.x / linevec.y * (iy + 1 - y0) + x0;
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
    }
    priorlinevec = linevec;
}

int main()
{
    constexpr double nan = std::numeric_limits<double>::quiet_NaN();
    for (int ix = 0; ix < WIDTH * HEIGHT; ++ix) fld[ix] = nan;

    //boundaryLine(20.25, 10.25, 15.25, 15.25);
    //boundaryLine(5.25, 15.25, 10.25, 10.25);

    /*boundaryLine(15.25, 5.25, 5.25, 15.25);
    boundaryLine(5.25, 15.25, 15.25, 25.25);
    boundaryLine(15.25, 25.25, 25.25, 15.25);
    boundaryLine(25.25, 15.25, 15.25, 5.25);

    boundaryLine(15.25, 5.25, 5.25, 15.25);
    boundaryLine(5.25, 15.25, 15.75, 25.75);
    boundaryLine(15.75, 25.75, 25.5, 15.25);
    boundaryLine(25.5, 15.25, 15.25, 5.25);*/

    /*boundaryLine(1.2, 1.2, 30.2, 1.2);
    boundaryLine(30.2, 1.2, 30.2, 30.2);
    boundaryLine(30.2, 30.2, 1.2, 30.2);
    boundaryLine(1.2, 30.2, 1.2, 1.2);*/

    /*boundaryLine((int)(WIDTH / 2.), (int)(HEIGHT /9.), (int)(8 * WIDTH /9.), (int)(HEIGHT /2.));
    boundaryLine((int)(8 * WIDTH / 9.), (int)(HEIGHT / 2.), (int)(WIDTH / 2.), (int)(8 * HEIGHT / 9.));
    boundaryLine((int)(WIDTH / 2.), (int)(8 * HEIGHT / 9.), (int)(WIDTH / 9.), (int)(HEIGHT / 2.));
    boundaryLine((int)(WIDTH / 9.), (int)(HEIGHT / 2.), (int)(WIDTH / 2.), (int)(HEIGHT / 9.));

    boundaryLine(WIDTH / 2., HEIGHT / 9., 3 * WIDTH / 9., HEIGHT / 2.);
    boundaryLine(3 * WIDTH / 9., HEIGHT / 2., WIDTH / 2., 8 * HEIGHT / 9.);
    boundaryLine(WIDTH / 2., 8 * HEIGHT / 9., WIDTH / 9., HEIGHT / 2.);
    boundaryLine(WIDTH / 9., HEIGHT / 2., WIDTH / 2., HEIGHT / 9.);*/

    //boundaryLine(50.964843750000000, 64.992187500000000, 50.574218750000000, 64.089843750000000);

    double w = WIDTH, h = HEIGHT;
    boundaryLine(3 * w / 8, 6 * h / 8, 3 * w / 8, 2 * h / 8);
    boundaryLine(3 * w / 8, 2 * h / 8, 5 * w / 8, 2 * h / 8);
    boundaryLine(5 * w / 8, 2 * h / 8, 5 * w / 8, 6 * h / 8);
    boundaryLine(5 * w / 8, 6 * h / 8, 4 * w / 8, 3 * h / 8);
    boundaryLine(4 * w / 8, 3 * h / 8, 3 * w / 8, 6 * h / 8);

    /*boundaryLine(3 * w / 8, 2 * h / 8, 5 * w / 8, 2 * h / 8);
    boundaryLine(5 * w / 8, 2 * h / 8, 4 * w / 8/* - 0.00001* /, 6 * h / 8);
    boundaryLine(4 * w / 8/* - 0.00001* /, 6 * h / 8, 3 * w / 8, 2 * h / 8);*/

    display(WIDTH, HEIGHT, fld, L"flattenham.png");

    double fldvalsign = 1.0; // sqrt(2);
    for (int Y = 0; Y < HEIGHT; ++Y)
    {
        for (int X = 0; X < WIDTH; ++X)
        {
            int iX = Y * WIDTH + X;
            if (!std::isnan(fld[iX]) && fld[iX] * fldvalsign <= 0) // test for sign flip only
            {
                fldvalsign = -fldvalsign;
            }
            if (std::isnan(fld[iX]))
            {
                fld[iX] = fldvalsign;
            }
        }
        fldvalsign = 1.0; // sqrt(2);
    }
    display(WIDTH, HEIGHT, fld, L"flattenham-filled.png");
}
