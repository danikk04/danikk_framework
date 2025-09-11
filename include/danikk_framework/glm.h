#pragma once
#include <danikk_framework/danikk_framework.h>
#include <danikk_framework/log.h>
#include <danikk_framework/number.h>
#include <danikk_framework/random.h>
#include <danikk_framework/math.h>
#include <assert.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>

namespace danikk_framework
{
	using glm::uvec2;
	using glm::vec2;
	using glm::uvec3;
	using glm::ivec3;
	using glm::vec3;

#define pi glm::pi<float>()

	float constexpr pi_half = 0.5f * pi;

	template<class number_t> number_t clamp(number_t arg, number_t max, number_t min)
	{
		if(arg > max)
		{
			return arg;
		}
		else if(arg < min)
		{
			return min;
		}
		else min;
	}

	LOG_OPERATOR(glm::vec2 data)
	{
		formatWrite(out, "%/%", data.x, data.y);
		return out;
	}

	LOG_OPERATOR(glm::uvec2 data)
	{
		formatWrite(out, "%/%", data.x, data.y);
		return out;
	}

	LOG_OPERATOR(glm::ivec2 data)
	{
		formatWrite(out, "%/%", data.x, data.y);
		return out;
	}

	LOG_OPERATOR(glm::vec3 data)
	{
		formatWrite(out, "%/%/%", data.x, data.y, data.z);
		return out;
	}

	LOG_OPERATOR(glm::ivec3 data)
	{
		formatWrite(out, "%/%/%", data.x, data.y, data.z);
		return out;
	}

	LOG_OPERATOR(glm::uvec3 data)
	{
		formatWrite(out, "%/%/%", data.x, data.y, data.z);
		return out;
	}

	LOG_OPERATOR(glm::mat4& data)
	{
		if(startLogInfo())
		{
			for(index_t x = 0; x < 4; x++)
			{
				for(index_t y = 0; y < 4; y++)
				{
					log_buffer << data[y][x] << ',';
				}
				log_buffer << ' ';
			}
			endLog();
		}
		return out;
	}

	inline glm::ivec2 ivec2_from_string(const String& data)
	{
		String first;
		String second;
		const char* space_ptr = data.find(' ');
		if(space_ptr == NULL)
		{
			return glm::ivec2(0, 0);
		}
		data.split(space_ptr, first, second);
		return glm::ivec2(parseNumber<int>(first), parseNumber<int>(second));
	}

	template<class vec_type> inline bool betweenByAxis(vec_type value, vec_type p1, vec_type p2)
	{
		bool x_axis = value.x == p1.x && value.x == p2.x;
		bool y_axis = value.y == p1.y && value.y == p2.y;
		if(x_axis)
		{
			return (value.x > p1.x && value.x < p2.x) ||
				(value.x > p2.x && value.x < p1.x);
		}
		else if(y_axis)
		{
			return (value.y > p1.y && value.y < p2.y) ||
				(value.y > p2.y && value.y < p1.y);
		}
		else
		{
			return false;
		}
	}

	template<class vec_type> inline bool betweenByDiag(vec_type value, vec_type p1, vec_type p2)
	{
		typedef int32 tmp_type;
		vec_type abs_delta1 =
				vec_type(::abs((tmp_type)value.x - (tmp_type)p1.x),
						::abs((tmp_type)value.y - (tmp_type)p1.y));
		vec_type abs_delta2 =
				vec_type(::abs((tmp_type)value.x - (tmp_type)p2.x),
						::abs((tmp_type)value.y - (tmp_type)p2.y));
		if((abs_delta1.x != abs_delta1.y) || (abs_delta2.x != abs_delta2.y))
		{
			return false;
		}
		bool by_x = (value.x > p1.x && value.x < p2.x) ||
					(value.x > p2.x && value.x < p1.x);
		bool by_y = (value.y > p1.y && value.y < p2.y) ||
					(value.y > p2.y && value.y < p1.y);
		return by_x && by_y;
	}

	template<class vec_type> vec_type::value_type squareLen(vec_type v1, vec_type v2)
	{
		return abs(v1.x - v2.x) + abs(v1.y - v2.y);
	}

	template<class vec2, class val> bool inRange(const vec2& range, const val& value)
	{
		return (value > range.x) && (value < range.y);
	}

	template<class vec2> vec2 index2Pos(int index, size_t size)
	{
		return vec2(index % size, index / size);
	}

	inline vec3 parseVec3(char** data)
	{
		return vec3(
				parseNumber<float>(data[0]),
				parseNumber<float>(data[1]),
				parseNumber<float>(data[2]));
	}

	inline vec2 parseVec2(char** data)
	{
		return vec2(
				parseNumber<float>(data[0]),
				parseNumber<float>(data[1]));
	}

	inline uvec3 parseUvec3(char** data)
	{
		return uvec3(
				parseNumber<uint32>(data[0]),
				parseNumber<uint32>(data[1]),
				parseNumber<uint32>(data[0]));
	}

	inline vec2 randVec2()
	{
		return vec2(default_random.nextFloat(), default_random.nextFloat());
	}

	inline vec3 randVec3()
	{
		return vec3(default_random.nextFloat(), default_random.nextFloat(), default_random.nextFloat());
	}

	inline vec3 truncateTo100(vec3 vector)
	{
		return vec3(truncateTo100(vector.x), truncateTo100(vector.y), truncateTo100(vector.z));
	}

	inline vec3 spreadDirection(vec3 direction, float random_factor)
	{
		return glm::normalize(randVec3() * random_factor + direction);
	}

	inline ivec3 mod(ivec3 x, ivec3 y)
	{
		return ivec3(mod(x.x, y.x), mod(x.y, y.y), mod(x.z, y.z));
	}
}
