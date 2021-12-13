#pragma once
#define NOMINMAX
#include <windows.h>
#include <stdio.h>
#include "Timer.h"
#include "Console.h"
#include "Math/float2.h"
#include "Math/float3.h"

#define DEBUG(format, ...) log(__FILE__, __LINE__, format, __VA_ARGS__);

void log(const char file[], int line, const char* format, ...);

enum update_status
{
	UPDATE_CONTINUE = 1,
	UPDATE_STOP,
	UPDATE_ERROR
};

struct Vertex
{
	float3 position;
	float3 normal;
	float2 uv;
};

// Constants ----------
#define LIBRARIES "SDL 2.0.16\nGLEW 2.1.0\nDear ImGui 1.86\nDevIL 1.8.0"
#define REPOSITORY_LINK "https://github.com/VBoschMatas/Engine"
#define TEXTURE_DATA_FORMAT "{start_%lu}\Path: %s\nFormat: %s\nWidth: %i\nHeight: %i\nDepth: %i\n{end_%lu}\n"

// Configuration -----------
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define FULLSCREEN false
#define MAXIMIZED true
#define RESIZABLE true
#define VSYNC true
#define TITLE "Super Ultra Mega Awesome Engine"