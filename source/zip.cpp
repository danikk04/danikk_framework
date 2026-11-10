#include <cstring>
#include <danikk_framework/danikk_framework.h>
#include <danikk_framework/string.h>
#include <danikk_framework/number.h>
#include <danikk_framework/filesystem.h>
#include <danikk_framework/zip.h>
#include <filesystem>
#include <danikk_framework/array.h>

using namespace std;
using namespace std::filesystem;

#define RETURN_WITH_ERROR(ERROR)\
*errorp = ERROR;\
zip_close(zip);\
remove(zipPath);\
return NULL;\

namespace danikk_framework
{
	zip_t* zip_create_from_directory(const char* zipPath, const char* directory, int* errorp)
	{
		zip_int64_t error;
		zip_t *zip = zip_open(zipPath, ZIP_CREATE /*+ ZIP_EXCL*/ + ZIP_TRUNCATE, (int*)&error);

		if (error != 0)
		{
			RETURN_WITH_ERROR(error)
		}

		size_t toLocalOffset = strlen(directory);

		for (const auto& dirEntry : std::filesystem::recursive_directory_iterator(directory))
		{
			const char* path = dirEntry.path().c_str();//Путь к текущему файлу или папке.
			const char* localPath = dirEntry.path().c_str() + toLocalOffset;//Путь относительно добавляемой директории.

			if(isDirectory(path))
			{
				error = zip_dir_add(zip, localPath, ZIP_FL_ENC_UTF_8);

				if(error != 0)
				{
					RETURN_WITH_ERROR(error)
				}
			}
			else
			{
				zip_source_t *source = zip_source_file(zip, path, 0, 0);
				if (source == NULL)
				{
					RETURN_WITH_ERROR(ZIP_ER_READ)
				}

				error = zip_file_add(zip, localPath, source, ZIP_FL_ENC_UTF_8);
				if(error != 0)
				{
					RETURN_WITH_ERROR(error)
				}
			}
		}
		return zip;
	}

    const char* zip_error_code_tostr(int error)
    {
        zip_error_t zip_error;
        zip_error_init_with_code(&zip_error, error);
        const char* result = zip_error_strerror(&zip_error);
        zip_error_fini(&zip_error);
        return result;
    }
}