// #include "OBJLoader.hpp"
// #include "Triangle.hpp"
// #include "../world/World.hpp"
// #include "../materials/Material.hpp"
// #include <fstream>
// #include <sstream>
// #include <vector>
// #include <iostream>
// #include <string>

// // parses just the vertex index from formats like:
// //   "3"  or  "3/2"  or  "3/2/1"  or  "3//1"
// static int parse_vertex_index(const std::string &token)
// {
//     // take everything before the first slash
//     std::string idx_str = token.substr(0, token.find('/'));
//     int idx = std::stoi(idx_str);
//     // obj indices are 1-based, convert to 0-based
//     if (idx > 0)
//         return idx - 1;
//     return idx; // negative = relative, rare, handle as-is
// }

// void load_obj(const std::string &filename, World &world, Material *mat)
// {
//     std::ifstream file(filename);
//     if (!file.is_open())
//     {
//         std::cerr << "error: could not open obj file: " << filename << "\n";
//         return;
//     }

//     std::vector<Point3D> vertices;
//     int triangle_count = 0;
//     std::string line;

//     while (std::getline(file, line))
//     {
//         // skip empty lines and comments
//         if (line.empty() || line[0] == '#')
//             continue;

//         std::istringstream iss(line);
//         std::string token;
//         iss >> token;

//         if (token == "v")
//         {
//             // vertex position line: v x y z
//             float x, y, z;
//             iss >> x >> y >> z;
//             vertices.push_back(Point3D(x, y, z));
//         }
//         else if (token == "f")
//         {
//             // face line - collect all vertex indices on this line
//             std::vector<int> face_indices;
//             std::string vtoken;
//             while (iss >> vtoken)
//             {
//                 face_indices.push_back(parse_vertex_index(vtoken));
//             }

//             // triangulate the face using fan triangulation
//             // triangle: v0, v1, v2 | v0, v2, v3 | v0, v3, v4 ...
//             // this handles triangles (3 verts) and quads (4 verts) and ngons
//             for (int i = 1; i + 1 < (int)face_indices.size(); i++)
//             {
//                 int i0 = face_indices[0];
//                 int i1 = face_indices[i];
//                 int i2 = face_indices[i + 1];

//                 // bounds check - skip degenerate faces
//                 if (i0 < 0 || i1 < 0 || i2 < 0)
//                     continue;
//                 if (i0 >= (int)vertices.size() ||
//                     i1 >= (int)vertices.size() ||
//                     i2 >= (int)vertices.size())
//                     continue;

//                 Triangle *tri = new Triangle(
//                     vertices[i0],
//                     vertices[i1],
//                     vertices[i2]);
//                 tri->set_material(mat);
//                 world.add_geometry(tri);
//                 triangle_count++;
//             }
//         }
//         // we skip: vt (texture coords), vn (normals), mtllib, usemtl, o, g, s
//         // for a basic render we only need positions
//     }

//     std::cout << "loaded " << filename << ": "
//               << vertices.size() << " vertices, "
//               << triangle_count << " triangles\n";
// }

#include "OBJLoader.hpp"
#include "Triangle.hpp"
#include "../world/World.hpp"
#include "../materials/Material.hpp"
#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>
#include <string>

// parses vertex index from "3" or "3/2" or "3/2/1" or "3//1"
// obj is 1-based so subtract 1
static int parse_vertex_index(const std::string &token)
{
    std::string idx = token.substr(0, token.find('/'));
    int i = std::stoi(idx);
    return (i > 0) ? i - 1 : i;
}

void load_obj(const std::string &filename,
              World &world,
              Material *mat,
              const OBJTransform &transform)
{

    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "error: could not open: " << filename << "\n";
        return;
    }

    std::vector<Point3D> vertices;
    int tri_count = 0;
    std::string line;

    while (std::getline(file, line))
    {
        if (line.empty() || line[0] == '#')
            continue;

        std::istringstream iss(line);
        std::string token;
        iss >> token;

        if (token == "v")
        {
            // read vertex and apply transform immediately
            float x, y, z;
            iss >> x >> y >> z;
            vertices.push_back(transform.apply(Point3D(x, y, z)));
        }
        else if (token == "f")
        {
            // collect all vertex indices for this face
            std::vector<int> idx;
            std::string vt;
            while (iss >> vt)
            {
                idx.push_back(parse_vertex_index(vt));
            }

            // fan triangulation: v0-v1-v2, v0-v2-v3, v0-v3-v4 ...
            for (int i = 1; i + 1 < (int)idx.size(); i++)
            {
                int i0 = idx[0], i1 = idx[i], i2 = idx[i + 1];
                int n = (int)vertices.size();

                // skip degenerate or out-of-range faces
                if (i0 < 0 || i1 < 0 || i2 < 0)
                    continue;
                if (i0 >= n || i1 >= n || i2 >= n)
                    continue;

                Triangle *tri = new Triangle(
                    vertices[i0], vertices[i1], vertices[i2]);
                tri->set_material(mat);
                world.add_geometry(tri);
                tri_count++;
            }
        }
        // skip: vt vn mtllib usemtl o g s
    }

    std::cout << "loaded " << filename << ": "
              << vertices.size() << " verts, "
              << tri_count << " triangles\n";
}