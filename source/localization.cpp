#include<danikk_framework/localization.h>
#include<danikk_framework/memory.h>
#include<danikk_framework/static_string.h>
#include<danikk_framework/log.h>

#include <locale.h>
#include <locale>
#include<mutex>

using namespace std;

#define _ set_localization

namespace danikk_framework
{
	void Localization::set(const char* key, const char* value)
	{
		m_data.insert(key, value);
	}

	void Localization::parsekvlist(const char* str)
	{
		StringBuffer<64> keyBuffer;
		String valueBuffer;

		bool is_value_reading = true;

		while(str != NULL)
		{
			char chr = *str;
			if(is_value_reading)
			{
				if(chr == '\n')
				{
					is_value_reading = false;
					m_data.insert_move(keyBuffer.data(), keyBuffer.size(), valueBuffer);//valueBuffer должен "сбросится" до NULL значения.
					keyBuffer.clear();
				}
				else
				{
					valueBuffer << chr;
				}
			}
			else
			{
				if(chr == ' ')
				{
					is_value_reading = true;
				}
				else
				{
					keyBuffer << chr;
				}
			}
			str++;
		}
	}

	static String localization_key_string;


	const String& Localization::operator()(const char* key)
	{
		const String* result = m_data.get(key);

		if(result == NULL)
		{
			formatLogWarning("% %.", localization("cant_find_localization"), key);
			localization_key_string.clear();
			localization_key_string << key;
			return localization_key_string;
		}
		else
		{
			return *result;
		}

	}

	Localization localization;

	void initLocalization()
	{
		_("debug_max_time",				"max time: ",							"Наибольшее время")
		_("debug_min_time",				"min time: ",							"Наименьшее время")
		_("debug_average_time",			"average time: ",						"Среднее время")
		_("call_argv_init", 			"Call danikk_framework::argv::init.",	"Вызовите danikk_framework::argv::init.")
		_("call_random_init", 			"Call danikk_framework::random::init.",	"Вызовите danikk_framework::random::init.")
		_("cant_find_localization", 	"Cant find localization",				"Не может найти локализацию")
		_("cant_get_config_value", 		"Cant get config value",				"Не может получить значение из конфига")
		_("config_already_opened", 		"Config already opened.",				"Конфиг уже открыт.")
		_("exec_cmd",					"Executing console command",			"Выполнение консольной команды")
		_("fail_to_open_log_file",		"Failed to open log file",				"Не удалось открыть лог файл")
	}

	void setLocale(const char* name)
	{
		std::setlocale(LC_ALL, name);
		std::setlocale(LC_CTYPE, name);
	}

	const char* getLocale()
	{
		return std::setlocale(LC_CTYPE, NULL);
	}
}
