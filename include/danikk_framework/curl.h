#pragma once
#include <curl/curl.h>
#include <danikk_framework/string.h>

namespace danikk_framework
{
	size_t write_data(void* buffer, size_t size, size_t nmemb, void* userp)
	{
		size_t real_size = size * nmemb;
		danikk_framework::String* str = (String*)userp;
		str->append((const char*)buffer, real_size);
		return real_size;
	}

	bool wget_download_append(String& output, const char* url)
	{
		CURL* curl = curl_easy_init();
		if (!curl) return false;

		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &output);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);

		CURLcode res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);

		bool success = (res == CURLE_OK);
		return success;
	}

	bool wget_download(String& output, const char* url)
	{
		output.clear();
		wget_download_append(output, url);
	}
}
