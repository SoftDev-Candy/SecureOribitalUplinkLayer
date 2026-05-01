//
// Created by Candy on 4/30/2026.
//

#include "ObjLoader.hpp"
#include <fstream>
#include <iostream>
#include<sstream>
#include<glm/vec2.hpp>
#include<glm/vec3.hpp>
static int ResolveObjIndex(int idx , int listSize)
{
    //OBJ indices are 1-based.
    //Also if idx is negative it means "count from the end" (OBJ weirdness).
    //ie. if idx == 0 THOU ARE 'INVALIDDDDDDDDDDDDDD ! ' BEGONE SATAN

    if(idx > 0)
    {
        return idx - 1; //convert to 0-based
    }
    if (idx < 0)
    {
        return listSize + idx; //relative to end
    }

    return -1;
}

static bool ParseFaceToken(const std::string& tok, int& vIdx, int& vtIdx, int& vnIdx)
{
    // Face tokens can be:
    //  "v"
    //  "v/vt"
    //  "v//vn"
    //  "v/vt/vn"
    // We parse all of them so later we can support any OBJ file.

    vIdx = 0;
    vtIdx = 0;
    vnIdx = 0;

    size_t s1 = tok.find('/');
    if (s1 == std::string::npos)
    {
        // just "v"
        vIdx = std::stoi(tok);
        return true;
    }

    size_t s2 = tok.find('/', s1 + 1);

    // first chunk is always v
    vIdx = std::stoi(tok.substr(0, s1));

    if (s2 == std::string::npos)
    {
        // "v/vt"
        std::string a = tok.substr(s1 + 1);
        if (!a.empty())
            vtIdx = std::stoi(a);
        return true;
    }

    if (s2 == s1 + 1)
    {
        // "v//vn" (empty vt)
        std::string c = tok.substr(s2 + 1);
        if (!c.empty())
            vnIdx = std::stoi(c);
        return true;
    }

    // "v/vt/vn"
    std::string b = tok.substr(s1 + 1, s2 - (s1 + 1));
    std::string c = tok.substr(s2 + 1);

    if (!b.empty()) vtIdx = std::stoi(b);
    if (!c.empty()) vnIdx = std::stoi(c);

    return true;
}

struct VertexKey
{
    int v;
    int vt;
    int vn;

    bool operator==(const VertexKey& other) const
    {
        return v == other.v && vt == other.vt && vn == other.vn;
    }
};

struct VertexKeyHash
{
    size_t operator()(const VertexKey& k) const noexcept
    {
        // cheap-ish hash combine
        size_t h1 = std::hash<int>{}(k.v);
        size_t h2 = std::hash<int>{}(k.vt);
        size_t h3 = std::hash<int>{}(k.vn);
        return (h1 * 73856093u) ^ (h2 * 19349663u) ^ (h3 * 83492791u);
    }
};

bool LoadObjFromPos(const std::string &path, MeshData &Data)
{

    int vCount = 0;
    int vnCount = 0;
    int vtCount = 0;
    int fCount = 0;
    // map (v,vt,vn) -> packed vertex index (so we don't duplicate vertices constantly)
    std::unordered_map<VertexKey, unsigned int, VertexKeyHash> vertexMap;
    vertexMap.reserve(10000);

    std::ifstream objFile(path);

    if (!objFile.is_open())
    {
    std::cerr<<"[ObjLoader]Failed to open obj: "<<path<<std::endl;
    return false;
    }

std::string line;

//Vector and UV's Storage here
    std::vector<glm::vec3>positions;
    std::vector<glm::vec2>uvs;
    std::vector<glm::vec3>normals;

    while (std::getline(objFile,line))
    {
        if (line.empty() || line[0] == '#')
        {
            continue;
        }

        std::istringstream iss(line);
        std::string type;
        iss>>type;

        if (type =="v")
        {
            float x , y, z;
            glm::vec3 pos;
            iss>>type>>pos.x>>pos.y>>pos.z;
            positions.push_back(pos);
        }else if(line.rfind("f " , 0) == 0)
        {
            fCount++;

            // Parse face line tokens
            std::stringstream ss(line);
            std::string prefix;
            ss >> prefix;

            std::vector<unsigned int> faceVertexIndices;
            std::string tok;

            // Read tokens like: "12/3/7"
            while (ss >> tok)
            {
                int vI = 0, vtI = 0, vnI = 0;
                if (!ParseFaceToken(tok, vI, vtI, vnI))
                    continue;

                int v0  = ResolveObjIndex(vI, (int)positions.size());
                int vt0 = (vtI != 0) ? ResolveObjIndex(vtI, (int)uvs.size()) : -1;
                int vn0 = (vnI != 0) ? ResolveObjIndex(vnI, (int)normals.size()) : -1;

                if (v0 < 0 || v0 >= (int)positions.size())
                    continue;

                VertexKey key{ v0, vt0, vn0 };

                auto it = vertexMap.find(key);
                if (it == vertexMap.end())
                {
                    // Upgraded to // Create a new packed vertex (x y z u v nx ny nz)

                    const glm::vec3& p = positions[v0];

                    glm::vec2 uv(0.0f, 0.0f);
                    if (vt0 >= 0 && vt0 < (int)uvs.size())
                        uv = uvs[vt0]; // if OBJ has UVs we use them, if not its (0,0)

                    glm::vec3 n(0.0f, 0.0f, 1.0f);
                    if (vn0 >= 0 && vn0 < (int)normals.size())
                        n = normals[vn0];

                    unsigned int newIndex = (unsigned int)(Data.vertices.size() / 8);

                    Data.vertices.push_back(p.x);
                    Data.vertices.push_back(p.y);
                    Data.vertices.push_back(p.z);
                    Data.vertices.push_back(uv.x);
                    Data.vertices.push_back(uv.y);
                    Data.vertices.push_back(n.x);
                    Data.vertices.push_back(n.y);
                    Data.vertices.push_back(n.z);


                    vertexMap.emplace(key, newIndex);
                    faceVertexIndices.push_back(newIndex);
                }
                else
                {
                    // Already made this exact vertex combo before
                    faceVertexIndices.push_back(it->second);
                }
            }

        }
    }
}

