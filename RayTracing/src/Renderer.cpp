#include "Renderer.h"

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

void Renderer::Render()
{
    for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++)
    {
        for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++)
        {
            glm::vec2 coord = { (float)x / (float)m_FinalImage->GetWidth(), (float)y / (float)m_FinalImage->GetHeight() };
            coord = coord * 2.0f - 1.0f; // -1 -> 1
            glm::vec4 color = PerPixel(coord);
            color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));
            m_ImageData[x + y * m_FinalImage->GetWidth()] = Utils::ConvertToRGBA(color);
        }
    }

    m_FinalImage->SetData(m_ImageData);
}

glm::vec4 Renderer::PerPixel(glm::vec2 coord)
{
    glm::vec3 rayOrigin = glm::vec3(0.0f, 0.0f, 2.0f);
    glm::vec3 rayDirection = glm::vec3(coord.x, coord.y, -1.0f); // normalizing this tanks performance and isn't strictly necessary
    float radius = 0.5f;
    
    // (bx^2 + by^2)t^2 + (2(axbx + ayby))t + (ax^2 + ay^2 - r^2) = 0
    // where
    // a = ray origin
    // b = ray direction
    // r = radius
    // t = hit distance

    float a = glm::dot(rayDirection, rayDirection);
    float b = 2.0f * glm::dot(rayOrigin, rayDirection);
    float c = glm::dot(rayOrigin, rayOrigin) - radius * radius;

    // Discriminant: b^2 - 4ac
    float discriminant = b * b - 4.0f * a * c;

    // miss - return black
    if (discriminant < 0.0f)
        return { 0, 0, 0, 1 };
    
    float tClosest = (-b - glm::sqrt(discriminant)) / (2.0f * a);
    float tFar = (-b + glm::sqrt(discriminant)) / (2.0f * a);
    
    if (tClosest > 0.0f)
    {
        glm::vec3 hitPoint = rayOrigin + rayDirection * tClosest;
        glm::vec3 hitNormal = glm::normalize(hitPoint);
        
        const glm::vec3 lightDirection = glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f));
        float lightIntensity = glm::dot(hitNormal, -lightDirection);
        lightIntensity = glm::max(0.0f, lightIntensity);
        
        return { m_SphereColor * lightIntensity, 1.0f };
    }
    
    // back face
    else if (tFar > 0.0f)
    {
        return { m_SphereColor, 1 };
    }

    return { 0, 0, 0, 1 }; // 0xff000000
}
