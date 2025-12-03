#pragma once

#include "Common.h"
#include "Camera.h"
#include "Ray.h"
#include "Scene.h"
#include "Image.h"

#include <memory>

class Renderer
{
public:
    struct Settings
    {
        bool Accumulate = true;
        bool SlowRandom = false;
        bool EnableSky = false;
    };
public:
    Renderer(SDL_GPUDevice* gpuDevice)
        : m_GPU(gpuDevice) {}

    void OnResize(uint32_t width, uint32_t height);
    void Render(const Scene& scene, const Camera& camera);
    void OnFrameEnd();
    
    std::shared_ptr<Image> GetFinalImage() const { return m_FinalImage; }

    void ResetFrameIndex() { m_FrameIndex = 1; }
    Settings& GetSettings() { return m_Settings; }
private:
    struct HitPayload
    {
        float HitDistance;
        glm::vec3 WorldPosition;
        glm::vec3 WorldNormal;
        
        int ObjectIndex;
    };
    glm::vec4 PerPixel(uint32_t x, uint32_t y); // RayGen
    
    HitPayload TraceRay(const Ray& ray);
    HitPayload ClosestHit(const Ray& ray, float hitDistance, int objectIndex);
    HitPayload Miss([[maybe_unused]] const Ray& ray);
private:
    SDL_GPUDevice* m_GPU; // non-owning, yet
    std::shared_ptr<Image> m_FinalImage;
    Settings m_Settings;

    std::vector<uint32_t> m_ImageHorizontalIter, m_ImageVerticalIter;

    const Scene* m_ActiveScene = nullptr;
    const Camera* m_ActiveCamera = nullptr;
    
    uint32_t* m_ImageData = nullptr;
    glm::vec4* m_AccumulationData = nullptr;

    uint32_t m_FrameIndex = 1;
};
