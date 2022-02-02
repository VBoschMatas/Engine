#pragma once
#include "Globals.h"

class CubeMap;

class Skybox
{
public:
    Skybox();
    ~Skybox() = default;

    void Update();

    std::vector<std::string> faces;
private:
    CubeMap* skymap;
};

