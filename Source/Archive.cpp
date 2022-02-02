#include "Archive.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>  
#include <sstream>  

using json = nlohmann::json;

void Archive::ToFile(bool is_scene)
{

	std::stringstream buffer;
	if (is_scene)
	{
		buffer << "Library\\Scene";
	}
	else
	{
		auto it_name = json.find("UUID");
		buffer << "Library\\" << *it_name;
	}
	std::ofstream ofs(buffer.str().c_str(), std::ofstream::out);
	ofs << json;

	ofs.close();
}