#include <danikk_framework/string.h>
#include <danikk_framework/localization.h>
#include <danikk_framework/array.h>
#include <danikk_framework/cstring_functions.h>
#include <danikk_framework/number.h>
#include <danikk_framework/assert.h>
#include <danikk_framework/number.h>
#include <stdlib.h>
#include <string>
#include <limits>

using namespace std;

namespace danikk_framework
{
	bool stringToBool(const String& str)
	{
		if (str == "1")
		{
			return true;
		}
		else if(str == "0")
		{
			return false;
		}
		else if(str == "True")
		{
			return true;
		}
		else if(str == "False")
		{
			return false;
		}
		else if(str == "true")
		{
			return true;
		}
		else if(str == "false")
		{
			return false;
		}
		else
		{
			return false;
		}
	}

	const char* boolToNumberString(bool value)
	{
		return value ? "1" : "0";
	}

	//static const char* hexdict = "123456789ABCDEF";

	/*String hex(const String& data)
	{
		String result;
		byte* cur = (byte*)data.begin();
		byte* end = (byte*)data.end();
		while(cur < end)
		{
			byte chr = *cur;
			cur++;
			result << hexdict[chr & 0xF0] << hexdict[chr & 0x0F];
		}
		return move(result);
	}

	String unhex(const String& data)
	{
		String result;
		byte* cur = (byte*)data.begin();
		byte* end = (byte*)data.end();
		while(cur < end)
		{
			byte hex1 = *cur;
			cur++;
			byte hex2 = *cur;
			cur++;
			byte chr =
		}
		return move(result);
	}*/

	String toCodeList(const String& data)
	{
		String result;
		const byte* cur = (const byte*)data.data();
		const byte* end = cur + data.size();
		while(cur < end)
		{
			byte chr = *cur;
			cur++;
			result << (uint64)chr << ';';
		}
		return move(result);
	}

	String unCodeList(const String& data)
	{
		String result;
		const char* cur = data.data();
		const char* end = cur + data.size();
		String numbuf;
		while(cur < end)
		{
			char chr = *cur;
			if(chr == ';')
			{
				byte value = parseNumber<byte>(numbuf);
				result << *((char*)&value);
				numbuf.clear();
			}
			else
			{
				numbuf << chr;
			}
			cur++;
		}
		return move(result);
	}

	void toCodeList(String& data)
	{
		byte* cdata = (byte*)alloca(data.size());
		const byte* cur = cdata;
		const byte* end = cdata + data.size();
		memcpy(cdata, data.data(), data.size());
		data.resize(0);
		while(cur < end)
		{
			byte chr = *cur;
			cur++;
			data << (uint64)chr << ';';
		}
	}

	void unCodeList(String& data)
	{
		String source(data);
		const char* cur = source.data();
		const char* end = cur + source.size();
		data.resize(0);
		String numbuf;
		while(cur < end)
		{
			char chr = *cur;
			if(chr == ';')
			{
				byte value = parseNumber<byte>(numbuf);
				data << *((char*)&value);
				numbuf.clear();
			}
			else
			{
				numbuf << chr;
			}
			cur++;
		}
	}

}
