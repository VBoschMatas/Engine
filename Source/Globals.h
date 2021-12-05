#pragma once
#include <windows.h>
#include <stdio.h>
#include "Timer.h"

#define DEBUG(format, ...) log(__FILE__, __LINE__, format, __VA_ARGS__);

void log(const char file[], int line, const char* format, ...);

enum update_status
{
	UPDATE_CONTINUE = 1,
	UPDATE_STOP,
	UPDATE_ERROR
};

// Configuration -----------
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define FULLSCREEN false
#define RESIZABLE true
#define VSYNC true
#define TITLE "Super Awesome Engine"