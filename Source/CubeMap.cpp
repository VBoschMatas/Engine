#include "Application.h"
#include "CubeMap.h"
#include "ModuleProgram.h"
#include "ModuleEditorCamera.h"
#include "IL/il.h"
#include "IL/ilu.h"
#include <vector>
#include "GL/glew.h"

CubeMap::CubeMap(const float* vertices_list, std::vector<std::string> textures_path)
{
    initCubeMap(vertices_list, textures_path);
}

void CubeMap::initCubeMap(const float* vertices_list, std::vector<std::string> textures_path)
{
    glGenVertexArrays(1, &cubemapVAO);
    glGenBuffers(1, &cubemapVBO);
    glBindVertexArray(cubemapVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubemapVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_list), &vertices_list, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    cubemapTexture = loadCubemap(textures_path);
}

unsigned int CubeMap::loadCubemap(std::vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned int img_id;
        ilGenImages(1, &img_id);
        ilBindImage(img_id);
        if (ilLoadImage(faces[i].c_str()))
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH),
                ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT),
                GL_UNSIGNED_BYTE, ilGetData()
            );
            ilDeleteImages(1, &img_id);
        }
        else
        {
            console->AddLog("Couldn't load cubemap texture");
            ilDeleteImages(1, &img_id);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

void CubeMap::Update()
{
    const float4x4 view = App->editorcamera->getView();
    const float4x4 proj = App->editorcamera->getProjection();

    glDepthFunc(GL_ALWAYS);

    glUseProgram(App->program->skybox_program);
    glUniform1i(glGetUniformLocation(App->program->skybox_program, "skybox"), 0);
    glUniformMatrix4fv(glGetUniformLocation(App->program->skybox_program, "view"), 1, GL_TRUE, (const float*)&view);
    glUniformMatrix4fv(glGetUniformLocation(App->program->skybox_program, "proj"), 1, GL_TRUE, (const float*)&proj);

    glBindVertexArray(cubemapVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);
}