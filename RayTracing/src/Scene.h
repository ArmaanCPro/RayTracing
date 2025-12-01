#pragma once

#include "Common.h"
#include <vector>

struct Material
{
    glm::vec3 Albedo {1};
    float Roughness = 1.0f;
    float Metallic = 0.0f;
    glm::vec3 EmissionColor {0};
    float EmissionPower = 0.0f;

    glm::vec3 GetEmission() const { return EmissionColor * EmissionPower; }
};

struct Sphere
{
    glm::vec3 Position {0};
    float Radius = 0.5f;

    int MaterialIndex = 0;
};

struct Scene
{
    std::vector<Sphere> Spheres;
    std::vector<Material> Materials;
};
