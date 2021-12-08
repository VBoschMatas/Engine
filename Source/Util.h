#ifndef _UTIL_H
#define _UTIL_H

#include "GL/glew.h"
#include "windows.h"
#include "string.h"
#include <direct.h> // _getcwd
#include <iostream>
#include <fstream>
#include <stdlib.h>

namespace util
{
	// User is responsible for deallocation.
	inline void SubstrBeforeCharFromEnd(char** source, char before)
	{
		size_t position = 0;
		size_t size = strlen(*source);
		size_t buffer_start = 0;
		bool start_counting = false;

		for (size_t i = size - 1; i > 0; --i)
		{
			if (start_counting == false && (*source)[i] == before)
			{
				start_counting = true;
			}

			if (!start_counting)
			{
				continue;
			}

			++buffer_start;
		}

		buffer_start = min(buffer_start + 1, size - 1);

		if (!start_counting)
		{
			buffer_start = 0;
		}

		size_t buffer_size = size - buffer_start;
		char* buffer = (char*)malloc(buffer_size + 1);

		memcpy(buffer, &((*source)[buffer_start]), buffer_size);

		buffer[buffer_size] = '\0';

		free(*source);

		*source = buffer;
	}

	inline void SubstrAfterCharFromEnd(char** source, char after)
	{
		size_t position = 0;
		size_t size = strlen(*source);
		size_t buffer_size = 0;
		bool start_counting = false;

		for (size_t i = size - 1; i > 0; --i)
		{
			if (start_counting == false && (*source)[i] == after)
			{
				start_counting = true;
			}

			if (!start_counting)
			{
				continue;
			}

			++buffer_size;
		}

		++buffer_size;

		if (!start_counting)
		{
			buffer_size = size;
		}

		char* buffer = (char*)malloc(buffer_size + 1);

		memcpy(buffer, &((*source)[0]), buffer_size);

		buffer[buffer_size] = '\0';

		free(*source);

		*source = buffer;
	}

	inline void GetWorkingDirectory(char** buffer)
	{
		*buffer = (char*)malloc(MAX_PATH);
		_getcwd(*buffer, MAX_PATH);
	}

	inline char* ConcatCStrings(const char* first, const char* second)
	{
		size_t first_size = strlen(first);
		size_t second_size = strlen(second);
		size_t concat_size = first_size + second_size;

		char* concat_string = (char*)malloc(concat_size + 1);

		memcpy(concat_string, first, first_size);
		memcpy(&(concat_string[first_size]), second, second_size);

		concat_string[concat_size] = '\0';

		return concat_string;
	}

	inline void OpenLink(const char* link)
	{
		ShellExecute(NULL, "open", link, NULL, NULL, SW_SHOWNORMAL);
	}
}

#endif