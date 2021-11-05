#ifndef MYMODEL_H
#define MYMODEL_H

#include <map>
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
            : mtlName(mN), kd_map("empty"), ks_map("empty"),
              ka(a), kd(d), ks(s), ns(shininess) {}
        mtl(string mN)
            : mtlName(mN), kd_map("empty"), ks_map("empty"),
              ka(glm::vec3(0.0f, 0.0f, 0.0f)),
              kd(glm::vec3(0.0f, 0.0f, 0.0f)),
              ks(glm::vec3(0.0f, 0.0f, 0.0f)),
              ns(0.0f) {}
        mtl() : mtlName("empty"),
            kd_map("empty"),
            ks_map("empty"),
            ka(glm::vec3(0.0f, 0.0f, 0.0f)),
            kd(glm::vec3(0.0f, 0.0f, 0.0f)),
            ks(glm::vec3(0.0f, 0.0f, 0.0f)),
            ns(0.0f) {}
    };
private:
    map<string, Mesh> meshes;
    map<string, mtl>  materials;
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
        string prtName;
        string mtlName;

        float x, y, z;
        int vtx_i, tex_i, nrm_i;
        vector<string> temp_i;
        // load all vertices data
        while (getline(inFile, line))
        {
            if (line.substr(0, 1) == "o")
            {
                // remember to set up VAO of the last part!
                meshes[prtName].setUpVertx();

                // construct a new part of object
                std::istringstream s(line.substr(1));
                s >> prtName;
                meshes[prtName] = Mesh(prtName, true);
                cout << "Loading part [" << prtName << "]\t of the obj file\n";

                // vtxs.clear();
                // texs.clear();
                // nrms.clear();
                // fces.clear();
            }
            else if (line.substr(0, 2) == "vt")
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

                    meshes[prtName].vertices.push_back(temp_vtx);
                }
            }
            else if (line.substr(0, 6) == "usemtl")
            {
                std::istringstream s(line.substr(6));
                s >> mtlName;
                meshes[prtName].setMateria(
                    materials[mtlName].ka,
                    materials[mtlName].kd,
                    materials[mtlName].ks,
                    materials[mtlName].ns
                );
                if (materials[mtlName].kd_map != "empty")
                    meshes[prtName].addMapping(materials[mtlName].kd_map, Texture::TEXType::DIFF);
                if (materials[mtlName].ks_map != "empty")
                    meshes[prtName].addMapping(materials[mtlName].ks_map, Texture::TEXType::SPEC);
            }
            else
            {
            }
        }
        vtxs.clear();
        texs.clear();
        nrms.clear();
        fces.clear();
    }
    void loadMtlFile(const string& path)
    {
        ifstream inFile(path.c_str());
        if (inFile.is_open())
            cout << "mtl file has been found.\n";
        else
            cout << "ERROR::OPENNING::FILE\n";

        string line;
        string mtlName;

        while (getline(inFile, line))
        {
            if (line.substr(0, 6) == "newmtl")
            {
                std::istringstream s(line.substr(6));
                s >> mtlName;
                materials[mtlName] = mtl(mtlName);
            }
            else if (line.substr(0, 2) == "Ka")
            {
                std::istringstream s(line.substr(2));
                s >> materials[mtlName].ka.x;
                s >> materials[mtlName].ka.y;
                s >> materials[mtlName].ka.z;
            }
            else if (line.substr(0, 2) == "Kd")
            {
                std::istringstream s(line.substr(2));
                s >> materials[mtlName].kd.x;
                s >> materials[mtlName].kd.y;
                s >> materials[mtlName].kd.z;
            }
            else if (line.substr(0, 2) == "Ks")
            {
                std::istringstream s(line.substr(2));
                s >> materials[mtlName].ks.x;
                s >> materials[mtlName].ks.y;
                s >> materials[mtlName].ks.z;
            }
            else if (line.substr(0, 2) == "Ns")
            {
                std::istringstream s(line.substr(2));
                s >> materials[mtlName].ns;
                materials[mtlName].ns /= 2.0f;
            }
            else if (line.substr(0, 6) == "map_Kd")
            {
                std::istringstream s(line.substr(6));
                s >> materials[mtlName].kd_map;
                materials[mtlName].kd_map = dirPath + '/' + materials[mtlName].kd_map;
            }
            else if (line.substr(0, 6) == "map_Ks")
            {
                std::istringstream s(line.substr(6));
                s >> materials[mtlName].ks_map;
                materials[mtlName].ks_map = dirPath + '/' + materials[mtlName].ks_map;
            }
        }
    }

public:
    /**
     * @brief Construct a new Model object
     * 
     * @param dP directory path name
     * @param oN objfile name
     */
    Model(const string dP, const string oN)
    {
        dirPath = dP;
        objName = oN;

        loadMtlFile(dirPath + '/' + objName + ".mtl");
        // printMtl();
        loadObjFile(dirPath + '/' + objName + ".obj");
        // printObj();
    }
    void Draw(Shader& shader)
    {
        for (auto iter = meshes.begin(); iter != meshes.end(); ++iter)
        {
            iter->second.Draw(shader);
        }
    }

    // some utilities
    void printObj()
    {
        cout << "this obj file contains " << meshes.size() << " parts\n";
        for (map<string, Mesh>::reverse_iterator iter = meshes.rbegin(); iter != meshes.rend(); ++iter)
        {
            cout << iter->second.partName << " has vertices: ";
            cout << iter->second.vertices.size() << endl;
        }
    }
    void printMtl()
    {
        for (map<string, mtl>::reverse_iterator iter = materials.rbegin(); iter != materials.rend(); ++iter)
        {
            cout << "newmtl " << iter->second.mtlName << endl;
            cout << "Ka " << iter->second.ka.x << ' '
                          << iter->second.ka.y << ' '
                          << iter->second.ka.z << endl;
            cout << "Kd " << iter->second.kd.x << ' '
                          << iter->second.kd.y << ' '
                          << iter->second.kd.z << endl;
            cout << "Ks " << iter->second.ks.x << ' '
                          << iter->second.ks.y << ' '
                          << iter->second.ks.z << endl;
            cout << "Ns " << iter->second.ns << endl;
            cout << "map_Kd " << iter->second.kd_map << endl;
            cout << "map_Ks " << iter->second.ks_map << endl;
            cout << endl;
        }
    }
};

#endif