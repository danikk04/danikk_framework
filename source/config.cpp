#include <danikk_framework/config.h>
#include <danikk_framework/cataloged_dictionary.h>
#include <danikk_framework/filesystem.h>
#include <danikk_framework/misc/line_getter.h>
#include <danikk_framework/cstring_functions.h>
#include <danikk_framework/localization.h>
#include <danikk_framework/log.h>
#include <danikk_framework/cstring_iterator.h>

namespace danikk_framework
{
	void Config::openData(String& data)
	{
		openData(data.c_string());
	}

	void Config::openData(const char* str)
	{
		String key;
		String value;
		const char* end = strchr(str, '\0');
		while(true)
		{
			char* key = str;
			str = strchr(str, '=') + 1;
			char* value = str;
			str = strchr(str, '\n');
			str = str ? str + 1 : end;

			m_data.insert(key, value);

			if(curstr >= end)
			{
				break;
			}
		}
		m_opened = true;
	}

	void Config::openFile(const char* filename)
	{
		String data;
		FileReader reader;
		reader.open(filename);
		if(!reader.isOpen())
		{
			formatLogInfo("!reader.isOpen() : %", filename);
		}
		data.resize(reader.size());
		reader.read(data.data(), data.size());
		data.cutChars('\r');
		openData(data);

	}
}
