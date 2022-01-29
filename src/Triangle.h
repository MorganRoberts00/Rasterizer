#pragma once
#ifndef _TRIANGLE_H_
#define _TRIANGLE_H_

#include <vector>

class Triangle
{
    std::vector<float> p;
    std::vector<float> q;
    std::vector<float> r;
    std::vector<float> s;
    std::vector<float> gMin;
    std::vector<float> gMax;
    // std::vector<float> colors;
    float alpha;
    float beta;
    float gamma;

public:
    Triangle(const std::vector<float> &, const std::vector<float> &, const std::vector<float> &);
    ~Triangle();
    void calcBarys(const std::vector<float> &);
    bool inTriangle();

private:
    float calcArea(const std::vector<float> &, const std::vector<float> &, const std::vector<float> &);
};

Triangle::Triangle(const std::vector<float> &p, const std::vector<float> &q, const std::vector<float> &r)
{
    this->p = p;
    this->q = q;
    this->r = r;
}

Triangle::~Triangle()
{
}

void Triangle::calcBarys(const std::vector<float> &s)
{
    this->s = s;

    float area = calcArea(p, q, r);
    alpha = calcArea(s, q, r) / area;
    beta = calcArea(p, s, r) / area;
    gamma = calcArea(p, q, s) / area;

    this->s.at(2) = alpha * p.at(2) + beta * q.at(2) + gamma * r.at(2);
}

// void Triangle::setZ() {}

// signed area
float Triangle::calcArea(const std::vector<float> &a, const std::vector<float> &b, const std::vector<float> &c)
{
    return (a.at(0) * (b.at(1) - c.at(1)) + b.at(0) * (c.at(1) - a.at(1)) + c.at(0) * (a.at(1) - b.at(1))) / 2.0;
}

bool Triangle::inTriangle()
{
    return (alpha >= 0 && beta >= 0 && gamma >= 0);
}

#endif
