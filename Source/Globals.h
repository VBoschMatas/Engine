#pragma once
#define NOMINMAX
#include <windows.h>
#include <stdio.h>
#include "Timer.h"
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
	// add normals in the future
	float2 uv;
};

// Configuration -----------
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define FULLSCREEN false
#define RESIZABLE true
#define VSYNC true
#define TITLE "Super Awesome Engine"