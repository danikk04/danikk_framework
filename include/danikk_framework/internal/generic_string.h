#pragma once

#include <danikk_framework/danikk_framework.h>
#include <danikk_framework/string.h>
#include <danikk_framework/type_traits.h>
#include <danikk_framework/number_base.h>

#include <string>
#include <cstring>
#include <cstdlib>

namespace danikk_framework
{
	class GenericString
	{
		virtual const char* data() const = 0;
		virtual size_t size() const = 0;
		virtual size_t capacity() const = 0;

		virtual void clear() = 0;
		virtual void resize(size_t new_size) = 0;
		virtual void push(char c) = 0;
		virtual void push(const char* data, size_t len) = 0;
		virtual void eraseCharacters(const GenericString& targets) = 0;

		virtual char* c_string() = 0;
		virtual const char* c_string() const = 0;

		// Доступ по индексу
		virtual char& operator[](size_t index) = 0;
		virtual const char& operator[](size_t index) const = 0;

		// Поиск
		virtual const char* find(char c) const = 0;
		virtual char* find(char c) = 0;
		virtual bool contains(char c) const = 0;

		// Сравнение
		virtual bool operator==(const GenericString& other) const = 0;
		virtual bool operator==(const char* str) const = 0;
	};
}
