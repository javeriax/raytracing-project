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

    float bmin_x = 1e30f, bmin_y = 1e30f, bmin_z = 1e30f;
    float bmax_x = -1e30f, bmax_y = -1e30f, bmax_z = -1e30f;

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
            Point3D tp = transform.apply(Point3D(x, y, z));
            vertices.push_back(tp);
            if (tp.x < bmin_x) bmin_x = tp.x;
            if (tp.y < bmin_y) bmin_y = tp.y;
            if (tp.z < bmin_z) bmin_z = tp.z;
            if (tp.x > bmax_x) bmax_x = tp.x;
            if (tp.y > bmax_y) bmax_y = tp.y;
            if (tp.z > bmax_z) bmax_z = tp.z;
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

    if (!vertices.empty())
    {
        std::cout << "  bounds min: (" << bmin_x << ", " << bmin_y << ", " << bmin_z << ")\n";
        std::cout << "  bounds max: (" << bmax_x << ", " << bmax_y << ", " << bmax_z << ")\n";
        std::cout << "  center:     (" << (bmin_x + bmax_x) * 0.5f << ", "
                  << (bmin_y + bmax_y) * 0.5f << ", "
                  << (bmin_z + bmax_z) * 0.5f << ")\n";
        std::cout << "  size:       (" << (bmax_x - bmin_x) << ", "
                  << (bmax_y - bmin_y) << ", "
                  << (bmax_z - bmin_z) << ")\n";
    }
}

#include <map>

void load_obj_mtl(const std::string &filename,
                  World &world,
                  const std::map<std::string, Material *> &materials,
                  Material *default_mat,
                  const OBJTransform &transform)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "error: could not open: " << filename << "\n";
        return;
    }

    std::vector<Point3D> vertices;
    Material *current_mat = default_mat;
    int tri_count = 0;
    int mat_switches = 0;
    std::string line;

    float bmin_x = 1e30f, bmin_y = 1e30f, bmin_z = 1e30f;
    float bmax_x = -1e30f, bmax_y = -1e30f, bmax_z = -1e30f;

    while (std::getline(file, line))
    {
        if (line.empty() || line[0] == '#')
            continue;

        std::istringstream iss(line);
        std::string token;
        iss >> token;

        if (token == "v")
        {
            float x, y, z;
            iss >> x >> y >> z;
            Point3D tp = transform.apply(Point3D(x, y, z));
            vertices.push_back(tp);
            if (tp.x < bmin_x) bmin_x = tp.x;
            if (tp.y < bmin_y) bmin_y = tp.y;
            if (tp.z < bmin_z) bmin_z = tp.z;
            if (tp.x > bmax_x) bmax_x = tp.x;
            if (tp.y > bmax_y) bmax_y = tp.y;
            if (tp.z > bmax_z) bmax_z = tp.z;
        }
        else if (token == "usemtl")
        {
            std::string mat_name;
            iss >> mat_name;
            auto it = materials.find(mat_name);
            if (it != materials.end())
                current_mat = it->second;
            else
                current_mat = default_mat;
            mat_switches++;
        }
        else if (token == "f")
        {
            std::vector<int> idx;
            std::string vt;
            while (iss >> vt)
                idx.push_back(parse_vertex_index(vt));

            for (int i = 1; i + 1 < (int)idx.size(); i++)
            {
                int i0 = idx[0], i1 = idx[i], i2 = idx[i + 1];
                int n = (int)vertices.size();
                if (i0 < 0 || i1 < 0 || i2 < 0) continue;
                if (i0 >= n || i1 >= n || i2 >= n) continue;

                Triangle *tri = new Triangle(
                    vertices[i0], vertices[i1], vertices[i2]);
                tri->set_material(current_mat);
                world.add_geometry(tri);
                tri_count++;
            }
        }
    }

    std::cout << "loaded " << filename << ": "
              << vertices.size() << " verts, "
              << tri_count << " triangles, "
              << mat_switches << " material switches\n";

    if (!vertices.empty())
    {
        std::cout << "  bounds min: (" << bmin_x << ", " << bmin_y << ", " << bmin_z << ")\n";
        std::cout << "  bounds max: (" << bmax_x << ", " << bmax_y << ", " << bmax_z << ")\n";
        std::cout << "  center:     (" << (bmin_x + bmax_x) * 0.5f << ", "
                  << (bmin_y + bmax_y) * 0.5f << ", "
                  << (bmin_z + bmax_z) * 0.5f << ")\n";
        std::cout << "  size:       (" << (bmax_x - bmin_x) << ", "
                  << (bmax_y - bmin_y) << ", "
                  << (bmax_z - bmin_z) << ")\n";
    }
}