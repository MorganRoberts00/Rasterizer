#pragma once
#ifndef _POINT_H_
#define _POINT_H_

#include <cmath>

struct Point
{
    float x;
    float y;
    float z;
    Point(float, float, float);
    ~Point();
    float getMagnitude();
};

Point::Point(float x, float y, float z) : x{x}, y{y}, z{z} {}

Point::~Point() {}

float Point::getMagnitude()
{
    return sqrt(x * x + y * y + z * z);
}

#endif