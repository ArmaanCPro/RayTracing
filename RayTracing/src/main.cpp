#include "Common.h"
#include <iostream>
#include "SphereLayer.h"

static constexpr uint32_t SCREEN_WIDTH = 1280;
static constexpr uint32_t SCREEN_HEIGHT = 720;

int main()
{
    if (SDL_Init(SDL_INIT_VIDEO) == false)
    {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    float mainScale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
    SDL_Window* window = SDL_CreateWindow("RayTracing", SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    if (window == nullptr)
    {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_ShowWindow(window);

    SDL_GPUDevice* gpuDevice = SDL_CreateGPUDevice(
        SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_DXIL |
        SDL_GPU_SHADERFORMAT_MSL | SDL_GPU_SHADERFORMAT_METALLIB, true, nullptr);
    if (gpuDevice == nullptr)
    {
        std::cerr << "GPU device could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    if (!SDL_ClaimWindowForGPUDevice(gpuDevice, window))
    {
        std::cerr << "GPU device could not claim window! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }
    SDL_SetGPUSwapchainParameters(gpuDevice, window, SDL_GPU_SWAPCHAINCOMPOSITION_SDR,
        SDL_GPU_PRESENTMODE_VSYNC);


    // Imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::StyleColorsDark();

    // scaling
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(mainScale);
    style.FontScaleDpi = mainScale;

    ImGui_ImplSDL3_InitForSDLGPU(window);
    ImGui_ImplSDLGPU3_InitInfo init_info = {};
    init_info.Device = gpuDevice;
    init_info.ColorTargetFormat = SDL_GetGPUSwapchainTextureFormat(gpuDevice, window);
    init_info.MSAASamples = SDL_GPU_SAMPLECOUNT_1;
    init_info.SwapchainComposition = SDL_GPU_SWAPCHAINCOMPOSITION_SDR;
    init_info.PresentMode = SDL_GPU_PRESENTMODE_VSYNC;
    ImGui_ImplSDLGPU3_Init(&init_info);

    SphereLayer layer{gpuDevice};


    bool running = true;
    SDL_Event event{};
    Timer timestep{};
    const bool* keyboardState = nullptr;
    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL3_ProcessEvent(&event);
            if (event.type == SDL_EVENT_QUIT)
            {
                running = false;
            }
            if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED &&
                event.window.windowID == SDL_GetWindowID(window))
            {
                running = false;
            }
        }

        if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED)
        {
            SDL_Delay(10);
            continue;
        }

        float ts = static_cast<float>(timestep.ElapsedMillis());
        timestep.Reset();
        keyboardState = SDL_GetKeyboardState(nullptr);
        layer.OnUpdate(ts, window, keyboardState);

        ImGui_ImplSDLGPU3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        layer.Render();
        layer.OnUIRender();

        ImGui::Render();
        ImDrawData* drawData = ImGui::GetDrawData();
        const bool isMinimized = (drawData->DisplaySize.x <= 0.0f || drawData->DisplaySize.y <= 0.0f);

        SDL_GPUCommandBuffer* commandBuf = SDL_AcquireGPUCommandBuffer(gpuDevice);

        SDL_GPUTexture* swapchainTexture = nullptr;
        SDL_WaitAndAcquireGPUSwapchainTexture(commandBuf, window, &swapchainTexture, nullptr, nullptr);

        if (swapchainTexture != nullptr && !isMinimized)
        {
            // uploads the vertex/index buffer
            ImGui_ImplSDLGPU3_PrepareDrawData(drawData, commandBuf);

            // setup and start a render pass
            SDL_GPUColorTargetInfo targetInfo{};
            targetInfo.texture = swapchainTexture;
            targetInfo.clear_color = SDL_FColor{ 0.0f, 0.0f, 0.0f, 1.0f };
            targetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
            targetInfo.store_op = SDL_GPU_STOREOP_STORE;
            targetInfo.mip_level = 0;
            targetInfo.layer_or_depth_plane = 0;
            targetInfo.cycle = false;
            SDL_GPURenderPass* renderPass = SDL_BeginGPURenderPass(commandBuf, &targetInfo, 1, nullptr);

            // Render ImGui
            ImGui_ImplSDLGPU3_RenderDrawData(drawData, commandBuf, renderPass);

            SDL_EndGPURenderPass(renderPass);
        }

        SDL_SubmitGPUCommandBuffer(commandBuf);
    }

    // Cleanup
    SDL_WaitForGPUIdle(gpuDevice);
    ImGui_ImplSDLGPU3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    SDL_ReleaseWindowFromGPUDevice(gpuDevice, window);
    SDL_DestroyGPUDevice(gpuDevice);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}