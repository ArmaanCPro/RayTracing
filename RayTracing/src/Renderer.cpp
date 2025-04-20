#include "Renderer.h"

#include "Walnut/Random.h"

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
            m_ImageData[x + y * m_FinalImage->GetWidth()] = PerPixel(coord);
        }
    }

    m_FinalImage->SetData(m_ImageData);
}

uint32_t Renderer::PerPixel(glm::vec2 coord)
{
    glm::vec3 rayOrigin = glm::vec3(0.0f, 0.0f, 2.0f);
    glm::vec3 rayDirection = glm::normalize(glm::vec3(coord.x, coord.y, -1.0f));
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
        return 0xff000000;
    
    // tMinus should be the closest hit
    float tMinus = (-b - glm::sqrt(discriminant)) / (2.0f * a);
    float tPlus = (-b + glm::sqrt(discriminant)) / (2.0f * a);

    if (tMinus > 0.0f)
    {
        glm::vec3 hitPoint = rayOrigin + tMinus * rayDirection;
        glm::vec3 hitNormal = glm::normalize(hitPoint);
        
        const glm::vec3 lightDirection = glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f));
        float lightIntensity = glm::dot(hitNormal, -lightDirection);
        lightIntensity = glm::max(0.1f, lightIntensity);
        
        uint8_t red = 255;
        uint8_t green = 0;
        uint8_t blue = 255;
        
        // scaling by intensity
        red = (uint8_t)(red * lightIntensity);
        green = (uint8_t)(green * lightIntensity);
        blue = (uint8_t)(blue * lightIntensity);
        
        return 0xff000000 | (red << 16) | (green << 8) | blue;
    }
    
    // back face
    else if (tPlus > 0.0f)
    {
        return 0xffff00ff;
    }

    return 0xff000000;
}
