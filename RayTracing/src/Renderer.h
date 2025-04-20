#pragma once

#include "Camera.h"
#include "Ray.h"
#include "Walnut/Image.h"
#include <memory>
#include <glm/glm.hpp>

class Renderer
{
public:
    Renderer() = default;
    
    void OnResize(uint32_t width, uint32_t height);
    void Render(const Camera& camera);
    
    std::shared_ptr<Walnut::Image> GetFinalImage() const { return m_FinalImage; }
    glm::vec3& GetSphereColor() { return m_SphereColor; }

private:
    glm::vec4 TraceRay(const Ray& ray);
private:
    std::shared_ptr<Walnut::Image> m_FinalImage;
    uint32_t* m_ImageData = nullptr;
    glm::vec3 m_SphereColor = { 1, 0, 1 };
};
