#pragma once

#include <random>
#include <chrono>

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace Walnut {

	class Random
	{
	public:
		static void Init()
		{
			uint32_t seed = static_cast<uint32_t>(std::chrono::system_clock::now().time_since_epoch().count());
			//s_RandomEngine.seed(std::random_device()());
			s_RandomEngine.seed(seed);
		}

		static uint32_t UInt()
		{
			return s_Distribution(s_RandomEngine);
		}

		static uint32_t UInt(uint32_t min, uint32_t max)
		{
			return min + (s_Distribution(s_RandomEngine) % (max - min + 1));
		}

		static float Float()
		{
			return s_FloatDistribution(s_RandomEngine);
		}

		static float Float(float min, float max)
		{
			return Float() * (max - min) + min;
		}

		static glm::vec3 Vec3()
		{
			return glm::vec3(Float(), Float(), Float());
		}

		// https://corysimon.github.io/articles/uniformdistn-on-sphere/
		static glm::vec3 Vec3InSphere()
		{
			float theta = 2.0f * glm::pi<float>() * s_FloatDistribution(s_RandomEngine);
			float phi = std::acosf(1.0f - 2.0f * s_FloatDistribution(s_RandomEngine));
			float x = std::sinf(phi) * std::cosf(theta);
			float y = std::sinf(phi) * std::sinf(theta);
			float z = std::cosf(phi);

			return { x, y, z };
		}

		static glm::vec3 Vec3(float min, float max)
		{
			return glm::vec3(Float() * (max - min) + min, Float() * (max - min) + min, Float() * (max - min) + min);
		}

		static glm::vec3 InUnitSphere()
		{
			return glm::normalize(Vec3(-1.0f, 1.0f));
		}
	private:
		static std::mt19937 s_RandomEngine;
		static std::uniform_int_distribution<std::mt19937::result_type> s_Distribution;
		static std::uniform_real_distribution<float> s_FloatDistribution;
	};

}


