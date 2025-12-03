#pragma once

#include "Common.h"

enum class ImageFormat
{
    RGBA
};

class Image
{
public:
    Image(SDL_GPUDevice* gpu, uint32_t width, uint32_t height, ImageFormat format, const void* data = nullptr);
    ~Image();

    void Resize(uint32_t newWidth, uint32_t newHeight);
    void SetData(const void* data);
    SDL_GPUTexture* GetTexture() const { return m_Texture; }

    uint32_t GetWidth() const { return m_Width; }
    uint32_t GetHeight() const { return m_Height; }

    void Cleanup();

private:
    void Allocate();
    void Release();

private:
    SDL_GPUDevice* m_GPU = nullptr; // non-owning. probably shouldn't store this as a member and just pass it around to functions freely, or use a global Application::GetGPU()
    SDL_GPUTexture* m_Texture = nullptr;
    uint32_t m_Width = 0, m_Height = 0;
    ImageFormat m_Format;

    std::vector<std::function<void()>> m_CleanupTasks;
    bool m_MarkedForResize = false;
};