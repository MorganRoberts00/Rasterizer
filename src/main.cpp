#include <iostream>
#include <string>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include "Image.h"
#include "Rasterizer.cpp"

// This allows you to skip the `` in front of C++ standard library
// functions. You can also say `using cout` to be more selective.
// You should never do this in a header file.
using namespace std;

int main(int argc, char **argv)
{
	if (argc < 6 || argc > 7)
	{
		cout << "Usage: A1 meshfile outputfile width height task [angle]" << endl;
		return 0;
	}
	string meshName(argv[1]);

	// Load geometry
	vector<float> posBuf; // list of vertex positions
	vector<float> norBuf; // list of vertex normals
	vector<float> texBuf; // list of vertex texture coords
	tinyobj::attrib_t attrib;
	vector<tinyobj::shape_t> shapes;
	vector<tinyobj::material_t> materials;
	string errStr;
	bool rc = tinyobj::LoadObj(&attrib, &shapes, &materials, &errStr, meshName.c_str());
	if (!rc)
	{
		cerr << errStr << endl;
	}
	else
	{
		// Some OBJ files have different indices for vertex positions, normals,
		// and texture coordinates. For example, a cube corner vertex may have
		// three different normals. Here, we are going to duplicate all such
		// vertices.
		// Loop over shapes
		for (size_t s = 0; s < shapes.size(); s++)
		{
			// Loop over faces (polygons)
			size_t index_offset = 0;
			for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
			{
				size_t fv = shapes[s].mesh.num_face_vertices[f];
				// Loop over vertices in the face.
				for (size_t v = 0; v < fv; v++)
				{
					// access to vertex
					tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
					posBuf.push_back(attrib.vertices[3 * idx.vertex_index + 0]);
					posBuf.push_back(attrib.vertices[3 * idx.vertex_index + 1]);
					posBuf.push_back(attrib.vertices[3 * idx.vertex_index + 2]);
					if (!attrib.normals.empty())
					{
						norBuf.push_back(attrib.normals[3 * idx.normal_index + 0]);
						norBuf.push_back(attrib.normals[3 * idx.normal_index + 1]);
						norBuf.push_back(attrib.normals[3 * idx.normal_index + 2]);
					}
					if (!attrib.texcoords.empty())
					{
						texBuf.push_back(attrib.texcoords[2 * idx.texcoord_index + 0]);
						texBuf.push_back(attrib.texcoords[2 * idx.texcoord_index + 1]);
					}
				}
				index_offset += fv;
				// per-face material (IGNORE)
				shapes[s].mesh.material_ids[f];
			}
		}
	}
	cout << "Number of vertices: " << posBuf.size() / 3 << endl;

	/* my code start */
	string outputFile(argv[2]);
	size_t width(stoi(argv[3]));
	size_t height(stoi(argv[4]));
	size_t task(stoi(argv[5]));
	float angle = 0.0;
	try
	{
		angle = stof(argv[6]);
	}
	catch (const std::exception &e)
	{
	}
	Rasterizer *rasterizer = new Rasterizer(posBuf, norBuf);
	rasterizer->makePNG(outputFile, width, height, task, angle);
	delete rasterizer;
	/* end */

	return 0;
}
