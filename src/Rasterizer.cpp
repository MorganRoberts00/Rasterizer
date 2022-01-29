#include "Rasterizer.h"
#include <limits>
#include <cmath>

using namespace std;
Rasterizer::Rasterizer(vector<float> &posBuf, vector<float> &norBuf) : posBuf{posBuf}, norBuf{norBuf}
{
}

Rasterizer::~Rasterizer() {}

void Rasterizer::makePNG(const string &filename, int width, int height, int task, int angle = 0)
{
    this->width = width;
    this->height = height;
    this->task = min(max(task, 1), 8);

    this->angle = angle;

    cPosBuf = posBuf;
    cNorBuf = norBuf;

    rotateY();
    calcExtrema();
    calcScale();
    calcTranslate();
    transformVertices();

    Image *image = new Image(width, height);
    drawTriangles(image);
    image->writeToFile(filename);
    delete image;
}

void Rasterizer::rotateY()
{
    float theta = angle * 2 * acos(0.0) / 180;
    for (size_t i = 0; i < cPosBuf.size(); i += 3)
    {
        float x = cPosBuf.at(i);
        float z = cPosBuf.at(i + 2);
        cPosBuf.at(i) = x * cos(theta) + z * sin(theta);
        cPosBuf.at(i + 2) = z * cos(theta) - x * sin(theta);
        x = cNorBuf.at(i);
        z = cNorBuf.at(i + 2);
        cNorBuf.at(i) = x * cos(theta) + z * sin(theta);
        cNorBuf.at(i + 2) = z * cos(theta) - x * sin(theta);
    }
}

void Rasterizer::calcExtrema()
{
    gMinX = cPosBuf.at(0);
    gMaxX = cPosBuf.at(0);
    gMinY = cPosBuf.at(1);
    gMaxY = cPosBuf.at(1);
    gMinZ = cPosBuf.at(2);
    gMaxZ = cPosBuf.at(2);

    for (size_t i = 3; i < cPosBuf.size(); i += 3)
    {
        gMinX = min(gMinX, cPosBuf.at(i));
        gMaxX = max(gMaxX, cPosBuf.at(i));
        gMinY = min(gMinY, cPosBuf.at(i + 1));
        gMaxY = max(gMaxY, cPosBuf.at(i + 1));
        gMinZ = min(gMinZ, cPosBuf.at(i + 2));
        gMaxZ = max(gMaxZ, cPosBuf.at(i + 2));
    }
}

void Rasterizer::calcScale()
{
    float sx = width / (gMaxX - gMinX);
    float sy = height / (gMaxY - gMinY);
    scale = min(sx, sy);
}

void Rasterizer::calcTranslate()
{
    tX = (width - scale * (gMaxX + gMinX)) / 2;
    tY = (height - scale * (gMaxY + gMinY)) / 2;
}

void Rasterizer::transformVertices()
{
    for (size_t i = 0; i < cPosBuf.size(); i += 3)
    {
        cPosBuf.at(i) = scale * cPosBuf.at(i) + tX;
        cPosBuf.at(i + 1) = scale * cPosBuf.at(i + 1) + tY;
        // cPosBuf.at(i + 2) = scale * cPosBuf.at(i + 2); // tZ not needed
        // DO NOT SCALE Z! LEADS TO OVERFLOW FOR LARGE PNG!!!!!!!
    }

    // update global extrema
    gMinX = scale * gMinX + tX;
    gMaxX = scale * gMaxX + tX;
    gMinY = scale * gMinY + tY;
    gMaxY = scale * gMaxY + tY;
}

void Rasterizer::drawTriangles(Image *image)
{
    double RANDOM_COLORS[7][3] = {
        {0.0000, 0.4470, 0.7410},
        {0.8500, 0.3250, 0.0980},
        {0.9290, 0.6940, 0.1250},
        {0.4940, 0.1840, 0.5560},
        {0.4660, 0.6740, 0.1880},
        {0.3010, 0.7450, 0.9330},
        {0.6350, 0.0780, 0.1840},
    };

    vector<vector<float>> zBuf;
    for (int i = 0; i < width; i++)
    {
        zBuf.push_back(vector<float>(height, gMinZ));
    }

    for (size_t i = 0; i < cPosBuf.size(); i += 9)
    {
        // get coordinates from buffer
        float x1 = cPosBuf.at(i);
        float y1 = cPosBuf.at(i + 1);
        float z1 = cPosBuf.at(i + 2);
        float x2 = cPosBuf.at(i + 3);
        float y2 = cPosBuf.at(i + 4);
        float z2 = cPosBuf.at(i + 5);
        float x3 = cPosBuf.at(i + 6);
        float y3 = cPosBuf.at(i + 7);
        float z3 = cPosBuf.at(i + 8);

        // get local min and max
        float minX = round(min(x1, min(x2, x3)));
        float minY = round(min(y1, min(y2, y3)));
        float maxX = round(max(x1, max(x2, x3)));
        float maxY = round(max(y1, max(y2, y3)));

        int color = (i) % 7;
        int color2 = (i + 1) % 7;
        int color3 = (i + 2) % 7;

        // get rgb values
        float r = RANDOM_COLORS[color][0];
        float g = RANDOM_COLORS[color][1];
        float b = RANDOM_COLORS[color][2];
        float r2 = RANDOM_COLORS[color2][0];
        float g2 = RANDOM_COLORS[color2][1];
        float b2 = RANDOM_COLORS[color2][2];
        float r3 = RANDOM_COLORS[color3][0];
        float g3 = RANDOM_COLORS[color3][1];
        float b3 = RANDOM_COLORS[color3][2];

        for (float j = minX; j < maxX; j++)
        {
            for (float k = minY; k < maxY; k++)
            {
                float alpha = 0, beta = 0, gamma = 0, area = 0;
                calcBarycentrics(j, k, x1, y1, x2, y2, x3, y3, alpha, beta, gamma, area);
                if (shouldDraw(alpha, beta, gamma, area) || (task == 1))
                {
                    float contested_z = (alpha * z1 + beta * z2 + gamma * z3) / area;
                    if (zBuf.at(j).at(k) < contested_z || task < 5)
                    {
                        zBuf.at(j).at(k) = contested_z;
                        unsigned char rT = 255 * r;
                        unsigned char gT = 255 * g;
                        unsigned char bT = 255 * b;

                        switch (task)
                        {
                        case 8:
                        {
                            rT = (contested_z - gMinZ) / (gMaxZ - gMinZ) * 255;
                            gT = (j - gMinX) / (gMaxX - gMinX) * 255;
                            bT = (k - gMinY) / (gMaxY - gMinY) * 255;
                            break;
                        }
                        case 7:
                        {
                            float a = (alpha * cNorBuf.at(i) + beta * cNorBuf.at(i + 3) + gamma * cNorBuf.at(i + 6)) / area;
                            float b = (alpha * cNorBuf.at(i + 1) + beta * cNorBuf.at(i + 4) + gamma * cNorBuf.at(i + 7)) / area;
                            float c = (alpha * cNorBuf.at(i + 2) + beta * cNorBuf.at(i + 5) + gamma * cNorBuf.at(i + 8)) / area;
                            float d = max((a + b + c) * sqrt(3) / 3 * 255, 0.0);
                            rT = d;
                            gT = d;
                            bT = d;
                            break;
                        }
                        case 6:
                        {
                            float a = (alpha * cNorBuf.at(i) + beta * cNorBuf.at(i + 3) + gamma * cNorBuf.at(i + 6)) / area;
                            float b = (alpha * cNorBuf.at(i + 1) + beta * cNorBuf.at(i + 4) + gamma * cNorBuf.at(i + 7)) / area;
                            float c = (alpha * cNorBuf.at(i + 2) + beta * cNorBuf.at(i + 5) + gamma * cNorBuf.at(i + 8)) / area;
                            rT = (a + 1) / 2 * 255;
                            gT = (b + 1) / 2 * 255;
                            bT = (c + 1) / 2 * 255;
                            break;
                        }
                        case 5:
                        {
                            rT = (contested_z - gMinZ) / (gMaxZ - gMinZ) * 255;
                            gT = 0;
                            bT = 0;
                            break;
                        }
                        case 4:
                        {
                            rT = (k - gMinY) / (gMaxY - gMinY) * 255;
                            gT = 0;
                            bT = (gMaxY - k) / (gMaxY - gMinY) * 255;
                            break;
                        }
                        case 3:
                        {
                            rT = (alpha * r + beta * r2 + gamma * r3) / area * 255;
                            gT = (alpha * g + beta * g2 + gamma * b3) / area * 255;
                            bT = (alpha * b + beta * b2 + gamma * g3) / area * 255;
                            break;
                        }
                        default:
                            break;
                        }
                        image->setPixel(j, k, rT, gT, bT);
                    }
                }
            }
        }
    }
}

// TODO: change?
float Rasterizer::calcArea(int x1, int y1, int x2, int y2, int x3, int y3)
{
    return abs((x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2)) / 2.0);
}

void Rasterizer::calcBarycentrics(int x, int y, int x1, int y1, int x2, int y2, int x3, int y3, float &alpha, float &beta, float &gamma, float &area)
{
    area = calcArea(x1, y1, x2, y2, x3, y3); // have to keep this separate because approximation errors?
    alpha = calcArea(x, y, x2, y2, x3, y3);
    beta = calcArea(x1, y1, x, y, x3, y3);
    gamma = calcArea(x1, y1, x2, y2, x, y);
}

bool Rasterizer::shouldDraw(float alpha, float beta, float gamma, float area)
{
    return (alpha + beta + gamma == area);
}