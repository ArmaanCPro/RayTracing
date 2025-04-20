#pragma once

#include <glm/glm.hpp>
#include <vector>

struct Sphere
{
    glm::vec3 Position {0};
    float Radius = 0.5f;

    glm::vec3 Albedo {1};
};

struct Scene
{
    std::vector<Sphere> Spheres;
};
