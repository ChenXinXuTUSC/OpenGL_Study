#ifndef MYMESH_H
#define MYMESH_H
/**
 * @file myMesh.h
 * @author your name (you@domain.com)
 * @brief This class is a test demo for loading obj file
 * it must load diffuse texture and specular texture...
 * @version 0.1
 * @date 2021-10-06
 * 
 * @copyright Copyright (c) 2021
 * 
 */
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

struct Vertex
{
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 TexCoords;
};

struct Texture
{
    enum TEXType{DIFF, SPEC, EVRM, RFLC};
    unsigned int id;
    TEXType type;
    string path;
};

class Mesh
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
        glm::vec3 ka; // 环境光分量
        glm::vec3 kd; // 漫反射分量
        glm::vec3 ks; // 镜面光分量
        float ns; // 反射强度
    };

    unsigned int VBO;
    unsigned int EBO;

    bool useEvrmTex;
    bool useRflcTex;

    void setupMesh()
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        // A great thing about structs is that their memory layout is sequential for all its items.
        // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
        // again translates to 3/2 floats which translates to a byte array.
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // set the vertex attribute pointers
        // vertex Positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
        // vertex normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, Normal));
        // vertex texture coords
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, TexCoords));

        glBindVertexArray(0);
    }
    
    unsigned int loadMapping(const char* path)
    {
        unsigned int textureID;
        glGenTextures(1, &textureID);

        int width, height, nrComponents;
        unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
        if (data)
        {
            GLenum format;
            if (nrComponents == 1)
                format = GL_RED;
            else if (nrComponents == 3)
                format = GL_RGB;
            else if (nrComponents == 4)
                format = GL_RGBA;

            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            stbi_image_free(data);
        }
        else
        {
            std::cout << "Texture failed to load at path: " << path << std::endl;
            stbi_image_free(data);
        }

        return textureID;
    }
    unsigned int loadCubeMap(const std::vector<string>& faces)
    {
        unsigned int textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

        int width, height, nrComponents;
        for (unsigned int i = 0; i < faces.size(); i++)
        {
            unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);
            if (data)
            {
                GLenum format;
                if (nrComponents == 1)
                    format = GL_RED;
                else if (nrComponents == 3)
                    format = GL_RGB;
                else if (nrComponents == 4)
                    format = GL_RGBA;
                
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                             0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
                stbi_image_free(data);
            }
            else
            {
                std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
                stbi_image_free(data);
            }
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        return textureID;
    }
    
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
    void loadObjFile(const string &path)
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
            //! Attention: the sequence you insert the vertices must
            //! correspond to the attribPointers you set.
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

public: 
    vector<Vertex> vertices;

    // indices would not be used this time...
    vector<unsigned int> indices;
    vector<Texture> textures;

    unsigned int VAO;

    string partName;

    // constructor
    Mesh(const string& path)
    {
        loadObjFile(path);
        setupMesh();

        useEvrmTex = false;
        useRflcTex = false;

        partName = "empty";
    }
    
    void setVertice(const vector<Vertex>& v)
    {
        vertices = v;
    }
    void setUpVertx()
    {
        setupMesh();
    }
    
    void addMapping(string texPath, Texture::TEXType texType)
    {
        Texture newTex;
        for (Texture& t : textures)
        {
            if (std::strcmp(t.path.c_str(), texPath.c_str()) == 0)
            {
                newTex.id = t.id;
                newTex.type = t.type;
                newTex.path = t.path;
                textures.push_back(newTex);
                return;
            }
        }
        newTex.id = loadMapping(texPath.c_str());
        newTex.type = texType;
        newTex.path = texPath;
        textures.push_back(newTex);
    }
    void addCubeMap(const std::vector<string> faces, Texture::TEXType texType)
    {
        if (texType == Texture::TEXType::EVRM)
            useEvrmTex = true;
        if (texType == Texture::TEXType::RFLC)
            useRflcTex = true;

        Texture newTex;
        newTex.id = loadCubeMap(faces);
        textures.push_back(newTex);
    }
    
    void Draw(Shader& shader)
    {
        if (useEvrmTex)
        {
            glDepthFunc(GL_LEQUAL);
            glBindVertexArray(VAO);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, textures[0].id);

            glDrawArrays(GL_TRIANGLES, 0, vertices.size());

            glBindVertexArray(0);
            glDepthFunc(GL_LESS); // set depth function back to default

            return;
        }
        if (useRflcTex)
        {
            glBindVertexArray(VAO);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, textures[0].id);
            glDrawArrays(GL_TRIANGLES, 0, vertices.size());
            glBindVertexArray(0);
            return;
        }
        // bind appropriate textures
        unsigned int diffuseNr = 1;
        unsigned int specularNr = 1;

        for (unsigned int i = 0; i < textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
            // retrieve texture number (the N in diffuse_textureN)
            string number;
            string name;
            if (textures[i].type == Texture::TEXType::DIFF)
            {
                number = std::to_string(diffuseNr++);
                name = "material.texture_diffuse";
            }
            else if (textures[i].type == Texture::TEXType::SPEC)
            {
                number = std::to_string(specularNr++);
                name = "material.texture_specular";
            }
            // now set the sampler to the correct texture unit
            glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
            // and finally bind the texture
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }

        // draw mesh
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, vertices.size());
        
        glBindVertexArray(0);

        // always good practice to set everything back to defaults once configured.
        glActiveTexture(GL_TEXTURE0);
    }
};

#endif