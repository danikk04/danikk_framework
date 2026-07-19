#pragma once

#include <danikk_framework/danikk_framework.h>
#include <danikk_framework/dictionary.h>

namespace danikk_framework
{
	template<class container> void readConfigDictionary(container& output, const char filename)
	{

	}

	void inlineParseConfig(char* data)
	{
		m_data = str;
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

			addPair(key, value);

			if(curstr >= end)
			{
				break;
			}
		}
	}
}


/*void Config::openData(String& data)
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

	}*/
