#include <glm.h>

namespace danikk_framework
{
	float lerp(float a, float b, float x)
	{
		return a + x * (b - a);
	}

	float fade(float x)
	{
		return x * x * x * (x * (6 * x - 15) + 10);
	}

	float gradient(glm::vec2 point, unsigned char seed)
	{
		glm::vec2 randomVectors[] = {
			{ 0.0f, 1.0f },
			{ 0.0f, -1.0f },
			{ 1.0f, 0.0f },
			{ -1.0f, 0.0f }
		};

		return glm::dot(point, randomVectors[seed % 4]);
	}

	float perlinNoise(const std::vector<unsigned int>& permutations, glm::vec2 point)
	{
		glm::vec2 gridCoords = glm::ivec2(point);
		glm::vec2 pointCoords = point - gridCoords;
		glm::vec2 distanceCoords = { fade(pointCoords.x), fade(pointCoords.y) };

		return (lerp(
				(
				gradient(pointCoords, permutations[permutations[gridCoords.x] + gridCoords.y]),
				gradient({ pointCoords.x - 1, pointCoords.y }, permutations[permutations[gridCoords.x + 1] + gridCoords.y]),
				distanceCoords.x
			),
			lerp(
				gradient({ pointCoords.x, pointCoords.y - 1 }, permutations[permutations[gridCoords.x] + gridCoords.y + 1]),
				gradient({ pointCoords.x - 1, pointCoords.y - 1 }, permutations[permutations[gridCoords.x + 1] + gridCoords.y + 1]),
				distanceCoords.x
			),
			distanceCoords.y
		) + 1) / 2;
	}
}
