#ifndef _RASTERIZER_H_
#define _RASTERIZER_H_

#include <string>
#include <vector>

// buffers, args -> png file
class Rasterizer
{
    std::vector<float> posBuf; // list of vertex positions
    std::vector<float> norBuf; // list of vertex normals
    std::vector<float> cPosBuf;
    std::vector<float> cNorBuf;
    // std::vector<float> texBuf; // list of vertex texture coords
    int width{0}, height{0}, task{0};
    float angle{0};
    float scale{0}, tX{0}, tY{0};
    float gMinX{0}, gMaxX{0}; // global X's
    float gMinY{0}, gMaxY{0}; // global Y's
    float gMinZ{0}, gMaxZ{0}; // global Z's (unused)
    // std::vector<float> zBuf; // list of z values

public:
    Rasterizer(std::vector<float> &, std::vector<float> &);
    ~Rasterizer();
    void makePNG(const std::string &, int, int, int, int);

private:
    // task 1
    void rotateY();
    void calcExtrema();
    void calcScale();
    void calcTranslate();
    void transformVertices();
    void drawTriangles(Image *);

    // task 2
    float calcArea(int, int, int, int, int, int);
    void calcBarycentrics(int, int, int, int, int, int, int, int, float &, float &, float &, float &);
    bool shouldDraw(float, float, float, float);
};

#endif