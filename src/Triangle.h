#pragma once
#ifndef _TRIANGLE_H_
#define _TRIANGLE_H_

#include "Point.h"

class Triangle
{
    Point *p;
    Point *q;
    Point *r;
    Point *s;
    // Point *colors;
    float alpha;
    float beta;
    float gamma;

public:
    Triangle(Point *, Point *, Point *);
    ~Triangle();
    void calcBarys(Point *);
    bool inTriangle();

private:
    float calcArea(Point *, Point *, Point *);
};

Triangle::Triangle(Point *p, Point *q, Point *r)
{
    this->p = p;
    this->q = q;
    this->r = r;
}

Triangle::~Triangle()
{
    if (p)
    {
        delete p;
    }

    if (q)
    {
        delete q;
    }

    if (r)
    {
        delete r;
    }

    if (s)
    {
        delete s;
    }
}

void Triangle::calcBarys(Point *s)
{
    this->s = s;

    float area = calcArea(p, q, r);
    alpha = calcArea(s, q, r) / area;
    beta = calcArea(p, s, r) / area;
    gamma = calcArea(p, q, s) / area;

    this->s->z = alpha * p->z + beta * q->z + gamma * r->z;
}

// void Triangle::setZ() {}

// signed area
float Triangle::calcArea(Point *a, Point *b, Point *c)
{
    return (a->x * (b->y - c->y) + b->x * (c->y - a->y) + c->x * (a->y - b->y)) / 2.0;
}

bool Triangle::inTriangle()
{
    return (alpha >= 0 && beta >= 0 && gamma >= 0);
}

#endif
