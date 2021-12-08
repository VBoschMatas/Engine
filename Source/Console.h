#pragma once
#include <string>
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#define CONSOLE_MAX_LINES 256

struct Console
{
public:
	Console()
	{

	};



	~Console()
	{

	};

	inline void AddLog(const char* format, ...)
	{
		static char tmp_string[4096];
		static va_list  ap;

		// Construct the string from variable arguments
		va_start(ap, format);
		vsprintf_s(tmp_string, 4096, format, ap);
		va_end(ap);

		AddLine(tmp_string);
	}

	inline void ImGui()
	{
		int counter = 0;
		int current_pos = start - 1;

		while (counter < CONSOLE_MAX_LINES - 1)
		{
			counter++;
			current_pos = (current_pos + 1) % CONSOLE_MAX_LINES;

			// Don't print empty lines
			if (lines_array[current_pos].empty())
			{
				continue;
			}

			ImGui::TextWrapped((lines_array[current_pos]).c_str());
			ImGui::Separator();
		}
	}

private:
	int counter = 0;
	int start = 0;
	int end = -1;
	std::string lines_array[CONSOLE_MAX_LINES];

	inline void AddLine(const char* new_line)
	{
		// We reuse the array once we are close to the end
		counter = std::min(++counter, CONSOLE_MAX_LINES - 1);

		end = (end + 1) % CONSOLE_MAX_LINES;

		lines_array[end].clear();
		lines_array[end] = std::string(new_line);

		if (counter == CONSOLE_MAX_LINES - 1 && end == start)
		{
			start = (start + 1) % CONSOLE_MAX_LINES;
		}
	}
};

extern Console* console;