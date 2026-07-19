#pragma once

#include </usr/include/libzip/zip.h>



namespace danikk_framework
{
	zip_t* zip_create_from_directory(const char* zipPath, const char* directory, int* errorp);

    const char* zip_error_code_tostr(int error);
}

#include <danikk_framework/internal/zip.inc>
