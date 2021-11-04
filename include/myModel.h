#ifndef MYMODEL_H
#define MYMODEL_H

#include <map>
#include <vector>
#include <string>
#include <cstring>
#include <sstream>
#include <fstream>
#include <iostream>
using namespace std;

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stb_image.h>

#include <shader.h>
#include <myMesh.h>


class Model
{
private:
    struct vtx
    {
        float x;
        float y;
        float z;
        vtx(float x_ = 0.0, float y_ = 0.0, float z_ = 0.0) : x(x_), y(y_), z(z_) {}
    };
    struct tex
    {
        float x;
        float y;
        tex(float x_ = 0.0, float y_ = 0.0) : x(x_), y(y_) {}
    };
    struct nrm
    {
        float x;
        float y;
        float z;
        nrm(float x_ = 0.0, float y_ = 0.0, float z_ = 0.0) : x(x_), y(y_), z(z_) {}
    };
    struct fce
    {
        int vtx_idx;
        int tex_idx;
        int nrm_idx;
        fce(int v = 0, int t = 0, int n = 0) : vtx_idx(v), tex_idx(t), nrm_idx(n) {}
    };
    struct mtl
    {
        string mtlName;
        string kd_map;
        string ks_map;
        glm::vec3 ka; // 环境光分量
        glm::vec3 kd; // 漫反射分量
        glm::vec3 ks; // 镜面光分量
        float ns; // 反射强度

        mtl(string mN, glm::vec3 a, glm::vec3 d, glm::vec3 s, float shininess)
            : mtlName(mN), ka(a), kd(d), ks(s), ns(shininess) {}
    };
private:
    vector<Mesh*> meshes;
    map<string, mtl> materials;

    string dirPath;
    string objName;

private:
    vector<string> stringSplit(const string &s, const char c)
    {
        int l = 0, r = 0;
        vector<string> clips;
        while (s[r] != '\0')
        {
            if (s[r] == c)
            {
                clips.push_back(s.substr(l, r - l));
                l = r + 1;
            }
            r++;
        }
        clips.push_back(s.substr(l, r - l));
        return clips;
    }
    void loadObjFile(const string& path)
    {
        vector<vtx> vtxs;
        vector<tex> texs;
        vector<nrm> nrms;
        vector<fce> fces;

        ifstream inFile(path.c_str());
        if (inFile.is_open())
            cout << "obj file has been found.\n";
        else
            cout << "ERROR::OPENNING::FILE\n";
        string line;

        float x, y, z;
        int vtx_i, tex_i, nrm_i;
        vector<string> temp_i;
        // load all vertices data
        while (getline(inFile, line))
        {
            if (line.substr(0, 2) == "vt")
            {
                // resolve texture coordinates
                std::istringstream s(line.substr(2));
                s >> x;
                s >> y;
                texs.push_back(tex(x, y));
            }
            else if (line.substr(0, 2) == "vn")
            {
                // resolve normal coordinates
                std::istringstream s(line.substr(2));
                s >> x;
                s >> y;
                s >> z;
                nrms.push_back(nrm(x, y, z));
            }
            else if (line.substr(0, 1) == "v")
            {
                // resolve position coordinates
                std::istringstream s(line.substr(2));
                s >> x;
                s >> y;
                s >> z;
                vtxs.push_back(vtx(x, y, z));
            }
            else if (line.substr(0, 1) == "f")
            {
                std::istringstream s(line.substr(2));
                string s1, s2, s3, s4;
                s >> s1 >> s2 >> s3 >> s4;
                if (s4.empty())
                {
                    // the surface is already a triangle
                    temp_i = stringSplit(s1, '/');
                    fces.push_back(fce(stoi(temp_i[0]) - 1, stoi(temp_i[1]) - 1, stoi(temp_i[2]) - 1));
                    temp_i = stringSplit(s2, '/');
                    fces.push_back(fce(stoi(temp_i[0]) - 1, stoi(temp_i[1]) - 1, stoi(temp_i[2]) - 1));
                    temp_i = stringSplit(s3, '/');
                    fces.push_back(fce(stoi(temp_i[0]) - 1, stoi(temp_i[1]) - 1, stoi(temp_i[2]) - 1));
                }
                else
                {
                    // the surface need to be split into two triangles
                    temp_i = stringSplit(s1, '/');
                    fces.push_back(fce(stoi(temp_i[0]) - 1, stoi(temp_i[1]) - 1, stoi(temp_i[2]) - 1));
                    temp_i = stringSplit(s2, '/');
                    fces.push_back(fce(stoi(temp_i[0]) - 1, stoi(temp_i[1]) - 1, stoi(temp_i[2]) - 1));
                    temp_i = stringSplit(s3, '/');
                    fces.push_back(fce(stoi(temp_i[0]) - 1, stoi(temp_i[1]) - 1, stoi(temp_i[2]) - 1));

                    temp_i = stringSplit(s1, '/');
                    fces.push_back(fce(stoi(temp_i[0]) - 1, stoi(temp_i[1]) - 1, stoi(temp_i[2]) - 1));
                    temp_i = stringSplit(s3, '/');
                    fces.push_back(fce(stoi(temp_i[0]) - 1, stoi(temp_i[1]) - 1, stoi(temp_i[2]) - 1));
                    temp_i = stringSplit(s4, '/');
                    fces.push_back(fce(stoi(temp_i[0]) - 1, stoi(temp_i[1]) - 1, stoi(temp_i[2]) - 1));
                }
            }
            else
            {
            }
        }
        // concatenate vertices
        for (auto &fce : fces)
        {
            //!Attention: the sequence you insert the vertices must
            //!correspond to the attribPointers you set.
            // insert position coords
            Vertex temp_vtx;
            temp_vtx.Position.x = vtxs[fce.vtx_idx].x;
            temp_vtx.Position.y = vtxs[fce.vtx_idx].y;
            temp_vtx.Position.z = vtxs[fce.vtx_idx].z;
            // insert normal coords
            temp_vtx.Normal.x = nrms[fce.nrm_idx].x;
            temp_vtx.Normal.y = nrms[fce.nrm_idx].y;
            temp_vtx.Normal.z = nrms[fce.nrm_idx].z;
            // insert texture coords
            temp_vtx.TexCoords.x = texs[fce.tex_idx].x;
            temp_vtx.TexCoords.y = texs[fce.tex_idx].y;

            vertices.push_back(temp_vtx);
        }
    }
    void loadMtlFile(const string& path)
    {
        ifstream inFile(path.c_str());
        if (inFile.is_open())
            cout << "obj file has been found.\n";
        else
            cout << "ERROR::OPENNING::FILE\n";
        string line;

        string mtlName;
        glm::vec3 temp_a;
        glm::vec3 temp_d;
        glm::vec3 temp_s;
        float temp_n;

        while (getline(inFile, line))
        {
            if (line.substr(0, 6) == "newmtl")
            {
                std::istringstream s(line.substr(2));
                s >> mtlName;
                
            }
        }
    }
public:
    Model(const string dP, const string oN)
    {
        dirPath = dP;
        objName = oN;

        loadMtlFile(dirPath + '/' + objName + ".mtl");
        loadObjFile(dirPath + '/' + objName + ".obj");
    }
};

#endif