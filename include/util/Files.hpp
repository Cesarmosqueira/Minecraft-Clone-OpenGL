#ifndef __FILES_H__
#define __FILES_H__

#include <string>

class Files {
	std::string shadersPath;
	std::string texturesPath;
	std::string objectsPath;
public:
	Files(std::string shadersPath="bin",
		   std::string texturesPath="resources/textures",
		   std::string objectsPath="resources/objects")
		: shadersPath(shadersPath),
			texturesPath(texturesPath),
			objectsPath(objectsPath) {}

	std::string shaderFile(const std::string& name) {
		return shadersPath + "/" + name;
	}

	std::string textureFile(const std::string& name) {
		return texturesPath + "/" + name;
	}

	std::string objectFile(const std::string& name) {
		return objectsPath + "/" + name;
	}
};

#endif

