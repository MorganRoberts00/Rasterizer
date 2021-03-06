#include <iostream>
// #include "Point.h"
#include "Triangle.h"

using namespace std;

int main(int argc, char const *argv[])
{

    vector<float> i = {1.0, 0.0, 0.0};
    vector<float> j = {0.0, 1.0, 0.0};
    vector<float> k = {0.0, 0.0, 1.0};
    Triangle *tri = new Triangle(i, j, k);

    for (float r = 0.0; r < 1.0; r += 0.1)
    {
        for (float s = 0.0; s < 1.0; s += 0.1)
        {
            vector<float> l = {s, r, 0.0}; 
            tri->calcBarys(l);
            cout << tri->inTriangle() << endl;
        }
    }

    delete tri;

    return 0;
}
