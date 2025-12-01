#include "Image.h"

#include <iostream>

static constexpr SDL_GPUTextureFormat ConvertToGPUFormat(ImageFormat format)
{
    switch (format)
    {
        case ImageFormat::RGBA:
            return SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
        default:
            std::cerr << "Unsupported image format!" << std::endl;
            return SDL_GPU_TEXTUREFORMAT_INVALID;
    }
}

Image::Image(SDL_GPUDevice* gpu, uint32_t width, uint32_t height, ImageFormat format, const void* data)
    :
    m_GPU(gpu),
    m_Width(width),
    m_Height(height),
    m_Format(format)
{
    Allocate();

    if (data)
    {
        SetData(data);
    }
}

Image::~Image()
{
    Release();
}

void Image::Resize(uint32_t newWidth, uint32_t newHeight)
{
    m_Width = newWidth;
    m_Height = newHeight;

    Release();

    Allocate();
}

void Image::SetData(const void *data)
{
    uint32_t uploadSize = m_Width * m_Height * 4; // 4 BPP for RGBA

    SDL_GPUTransferBufferCreateInfo tbci{};
    tbci.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    tbci.size = uploadSize;

    SDL_GPUTransferBuffer* transferBuffer = SDL_CreateGPUTransferBuffer(m_GPU, &tbci);

    if (!transferBuffer)
    {
        std::cerr << "GPU transfer buffer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return;
    }

    Uint8* map = static_cast<Uint8*>(SDL_MapGPUTransferBuffer(m_GPU, transferBuffer, false));
    if (map)
    {
        std::memcpy(map, data, uploadSize);
        SDL_UnmapGPUTransferBuffer(m_GPU, transferBuffer);
    }

    SDL_GPUCommandBuffer* cmdBuf = SDL_AcquireGPUCommandBuffer(m_GPU);
    SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(cmdBuf);

    SDL_GPUTextureTransferInfo source{};
    source.transfer_buffer = transferBuffer;
    source.offset = 0;
    source.pixels_per_row = m_Width;
    source.rows_per_layer = m_Height;

    SDL_GPUTextureRegion dest{};
    dest.texture = m_Texture;
    dest.x = 0;
    dest.y = 0;
    dest.w = m_Width;
    dest.h = m_Height;
    dest.d = 1;

    SDL_UploadToGPUTexture(copyPass, &source, &dest, false);

    SDL_EndGPUCopyPass(copyPass);
    SDL_SubmitGPUCommandBuffer(cmdBuf);

    // we can release immediately, SDL internally tracks the resource usage until cmdBuf completes
    SDL_ReleaseGPUTransferBuffer(m_GPU, transferBuffer);
}

void Image::Allocate()
{
    if (m_Width == 0 || m_Height == 0)
    {
        return;
    }

    SDL_GPUTextureCreateInfo ci{};
    ci.format = ConvertToGPUFormat(m_Format);
    ci.width = m_Width;
    ci.height = m_Height;
    ci.layer_count_or_depth = 1;
    ci.num_levels = 1;
    ci.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER;
    ci.type = SDL_GPU_TEXTURETYPE_2D;
    m_Texture = SDL_CreateGPUTexture(m_GPU, &ci);
    if (m_Texture == nullptr)
    {
        std::cerr << "GPU texture could not be created! SDL_Error: " << SDL_GetError() << std::endl;
    }
}

void Image::Release()
{
    SDL_ReleaseGPUTexture(m_GPU, m_Texture);
    m_Texture = nullptr;
}
