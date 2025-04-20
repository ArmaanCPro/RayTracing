#include "Renderer.h"

#include "Camera.h"
#include "Camera.h"
#include "Walnut/Random.h"

namespace Utils
{
    static uint32_t ConvertToRGBA(const glm::vec4& color)
    {
        uint8_t r = uint8_t(color.r * 255.0f);
        uint8_t g = uint8_t(color.g * 255.0f);
        uint8_t b = uint8_t(color.b * 255.0f);
        uint8_t a = uint8_t(color.a * 255.0f);

        return (a << 24) | (b << 16) | (g << 8) | r;
    }
}

void Renderer::OnResize(uint32_t width, uint32_t height)
{
    if (m_FinalImage)
    {
        // no resize necessary
        if (m_FinalImage->GetWidth() == width && m_FinalImage->GetHeight() == height)
            return;
        
        m_FinalImage->Resize(width, height);
    }
    else
    {
        m_FinalImage = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);   
    }
    
    delete[] m_ImageData;
    m_ImageData = new uint32_t[width * height];
}

void Renderer::Render(const Scene& scene, const Camera& camera)
{
    const glm::vec3& rayOrigin = camera.GetPosition();

    Ray ray;
    ray.Origin = rayOrigin;
    
    for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++)
    {
        for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++)
        {
            const glm::vec3& rayDirection = camera.GetRayDirections()[x + y * m_FinalImage->GetWidth()];
            ray.Direction = rayDirection;
            
            glm::vec4 color = TraceRay(scene, ray);
            color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));
            m_ImageData[x + y * m_FinalImage->GetWidth()] = Utils::ConvertToRGBA(color);
        }
    }

    m_FinalImage->SetData(m_ImageData);
}

glm::vec4 Renderer::TraceRay(const Scene& scene, const Ray& ray)
{
    // (bx^2 + by^2)t^2 + (2(axbx + ayby))t + (ax^2 + ay^2 - r^2) = 0
    // where
    // a = ray origin
    // b = ray direction
    // r = radius
    // t = hit distance

    if (scene.Spheres.empty())
        return glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

    const Sphere* closestSphere = nullptr;
    float hitDistance = std::numeric_limits<float>::max();
    
    for (const Sphere& sphere : scene.Spheres)
    {
        glm::vec3 origin = ray.Origin - sphere.Position;

        float a = glm::dot(ray.Direction, ray.Direction);
        float b = 2.0f * glm::dot(origin, ray.Direction);
        float c = glm::dot(origin, origin) - sphere.Radius * sphere.Radius;

        // Discriminant: b^2 - 4ac
        float discriminant = b * b - 4.0f * a * c;

        // miss - return black
        if (discriminant < 0.0f)
            continue;
    
        float tClosest = (-b - glm::sqrt(discriminant)) / (2.0f * a);
        if (tClosest < hitDistance)
        {
            closestSphere = &sphere;
            hitDistance = tClosest;
        }
    }

    if (closestSphere == nullptr)
        return { 0.0f, 0.0f, 0.0f, 1.0f };


    glm::vec3 sphereColor = closestSphere->Albedo;

    glm::vec3 origin = ray.Origin - closestSphere->Position;
    glm::vec3 hitPoint = origin + ray.Direction * hitDistance;
    glm::vec3 hitNormal = glm::normalize(hitPoint);
    
    const glm::vec3 lightDirection = glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f));
    float lightIntensity = glm::max(glm::dot(hitNormal, -lightDirection), 0.1f);

    sphereColor *= lightIntensity;
    return { sphereColor, 1.0f };
}
