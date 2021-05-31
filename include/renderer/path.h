#ifndef __PATH_H__
#define __PATH_H__

#include <string>

class Path {
	std::string shadersPath;
	std::string texturesPath;
public:
	Path(std::string shaders_folder, std::string texture_folder)
		: shadersPath(shaders_folder), 
          texturesPath(texture_folder) {}
	std::string sp(const std::string& name) {
		std::string str = shadersPath + name;
		return str;
	}
	std::string tp(const std::string& name) {
		std::string str = texturesPath + name;
		return str;
	}
};

#endif
