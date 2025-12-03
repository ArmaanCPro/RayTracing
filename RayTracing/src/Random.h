#pragma once

#include "Common.h"
#include <random>
#include <limits>

namespace Random
{
    using RandomEngineT = std::mt19937;
    using DistributionT = std::uniform_int_distribution<std::mt19937::result_type>;

    inline static thread_local std::mt19937 s_RandomEngine;
    inline static thread_local std::uniform_int_distribution<std::mt19937::result_type> s_Distribution;

    inline static void Init()
    {
        s_RandomEngine.seed(std::random_device()());
    }

    inline static uint32_t Uint()
    {
        return s_Distribution(s_RandomEngine);
    }

    inline static uint32_t Uint(uint32_t min, uint32_t max)
    {
        return min + (s_Distribution(s_RandomEngine) % (max - min + 1));
    }

    inline static float Float()
    {
        return static_cast<float>(s_Distribution(s_RandomEngine)) / static_cast<float>(std::numeric_limits<uint32_t>::max());
    }

    inline static glm::vec3 Vec3()
    {
        return glm::vec3(Float(), Float(), Float());
    }

    inline static glm::vec3 Vec3(float min, float max)
    {
#define RAND_VALUE      Float() * (max - min) + min
        return glm::vec3(RAND_VALUE, RAND_VALUE, RAND_VALUE);
#undef RAND_VALUE
    }

    inline static glm::vec3 InUnitSphere()
    {
        return glm::normalize(Vec3(-1.0f, 1.0f));
    }
}
