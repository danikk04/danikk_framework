#pragma once

#include<danikk_framework/danikk_framework.h>
#include <danikk_framework/dictionary.h>
#include<danikk_framework/string.h>
#include<danikk_framework/cstring_functions.h>
#include<initializer_list>

namespace danikk_framework
{
	#define set_localization(key, eng, rus)\
	if(strequal(getLocale(), "ru_RU"))\
	{\
		localization.set(key, eng);\
	}\
	else\
	{\
		localization.set(key, rus);\
	}\

	class Localization
	{
	private:
		DynamicDictionary<String, String> m_data;
	public:
		struct LocalizationPair
		{
			const char* key;
			const char* value;
		};

		Localization() = default;

		void set(const char* key, const char* value);

		void parsekvlist(const char* str);

		const String& operator()(const char* key);
	};

    //То есть Localization это класс локализации,
    //localization это объект со всеми строками фреймворка.
	//В каждом пространстве имён пусть будет свой объект
	//Localization с именем localization.
	//Что бы везде можно было бы просто ввести localization("ключ").
	extern Localization localization;

	void initLocalization();

	void setLocale(const char* name);

	const char* getLocale();
}
