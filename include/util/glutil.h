#ifndef __GLUTIL_H__
#define __GLUTIL_H__

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
//#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <renderer/path.h>
#include <math.h>

#include <fstream>
#include <iostream>
#include <string>

#include <time.h>
#include <unordered_map>

typedef float  f32;
typedef double f64;

typedef char      i8;
typedef short int i16;
typedef int       i32;
typedef long long i64;
typedef std::unordered_map<std::string, i32> U_Tree;

typedef unsigned char      ui8;
typedef unsigned short int ui16;
typedef unsigned int       ui32;
typedef unsigned long long ui64;


GLFWwindow* glutilInit(i32 major, i32 minor,
		i32 width, i32 height,
		const i8* title) {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(width, height, title, nullptr, nullptr);
	if (window == nullptr) {
		std::cerr << "Failed to create GLFW Window\n";
		glfwTerminate();
		return nullptr;
	}



	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "Not GLAD at all!\n";
		return nullptr;
	}

	return window;
}

class Shader {
	ui32 pid;
	Path* path;

	i32 ok;             // check for error status
	i8 infoLog[512];  // get error status info
    mutable U_Tree uniforms_cache;

public:
	Shader(const std::string& shader_folder, std::string texture_folder="assets/textures/")
			: path(new Path(shader_folder, texture_folder)) {
		std::ifstream vertexFile(path->sp("shader.vert"));
		std::string vertexSrc;
        std::cout << "[VS] Compiling from " << path->sp("shader.vert") << "\n";
		std::getline(vertexFile, vertexSrc, '\0');
        std::cout << vertexSrc << "\n";
		std::ifstream fragmentFile(path->sp("shader.frag"));
		std::string fragmentSrc;
        std::cout << "[FS] Compiling from " << path->sp("shader.frag") << "\n";
		std::getline(fragmentFile, fragmentSrc, '\0');
        std::cout << fragmentSrc << "\n";
		ui32 vertex = mkShader(vertexSrc.c_str(), GL_VERTEX_SHADER);
		ui32 fragment = mkShader(fragmentSrc.c_str(), GL_FRAGMENT_SHADER);

		pid = glCreateProgram();
		glAttachShader(pid, vertex);
		glAttachShader(pid, fragment);
		glLinkProgram(pid);
		glGetProgramiv(pid, GL_LINK_STATUS, &ok);
		if (!ok) {
			glGetProgramInfoLog(pid, 512, nullptr, infoLog);
			std::cout << "Error::shader::program::link_failed\n"
				<< infoLog << std::endl;
		}

		glDeleteShader(vertex);
		glDeleteShader(fragment);
	}
	~Shader() {
		glDeleteProgram(pid);
	}
	void useProgram() {
		glUseProgram(pid);
	}
	ui32 getProgram() { // might need to refactor this later ughhh
		return pid;
	}
    // retreive uniforms
    GLint selfGetUniformLocation(const i8* name) const {
        if(uniforms_cache.find(name) != uniforms_cache.end())
            return uniforms_cache[name];
        GLuint loc = glGetUniformLocation(pid, name);
        uniforms_cache[name] = loc; 
        return loc;
    }
	// Set uniforms
	void setMat4(const i8* name, const glm::mat4& mat) const {
		glUniformMatrix4fv(selfGetUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
	}

	void setFloat(const i8* name, const f32 f) const {
		glUniform1f(selfGetUniformLocation(name), f);
	}
    void setVec3(const i8* name, const glm::vec3& vec) const {
		glUniform3fv(selfGetUniformLocation(name), 1, &vec[0]);
	}
	void setVec3(const i8* name, f32 a, f32 b, f32 c) const {
		glUniform3f(selfGetUniformLocation(name), a, b, c);
	}
	// Texture loading
	ui32 loadTexture(const std::string& textureFile) {
		ui32 texture;
		std::string fileName = path->tp(textureFile);
        std::cout << " [LOADING TEXTURE FROM ] " << fileName << "\n";
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		
		i32 width, height, nrChannels;

		stbi_set_flip_vertically_on_load(true); // porque en opgl el eje Y invertio
		ui8* data = stbi_load(fileName.c_str(), &width, &height, &nrChannels, 0);
		if (data != nullptr) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
			             GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		} else {
			std::cerr << "Can't load texture\n";
		}
		stbi_image_free(data);
        
		return texture;
	}
    std::string get_texture_path(const std::string& fn){
        return path->tp(fn);
    }

private:
	ui32 mkShader(const i8* source, GLenum type) {
		ui32 shader = glCreateShader(type);
		glShaderSource(shader, 1, &source, nullptr);
		glCompileShader(shader);
		glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
		if (!ok) {
            glGetShaderInfoLog(shader, 512, nullptr, infoLog);
			std::cerr << "Error::shader::compilation_failed\n"
				<< infoLog << std::endl;
			return 0;
		}
		return shader;
	}
};

#endif

