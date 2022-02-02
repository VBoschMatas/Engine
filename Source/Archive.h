#pragma once
#include "Globals.h"
#include "json.hpp"

using json = nlohmann::json;

class Archive
{
public:
	Archive() { json = ""; };
	~Archive() = default;

	void ToFile(bool is_scene = false);

	json json;
};

