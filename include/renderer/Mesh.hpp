#ifndef __MESH_H__
#define __MESH_H__

#include "util/glutil.h"
#include <vector>


struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	glm::vec3 Tangent;
	glm::vec3 Bitangent;
};

struct Texture {
	ui32         id;
	std::string type;
	std::string path;
};

class Mesh {

public:
	std::vector<Vertex>  vertices;
	std::vector<ui32>     indices;
	std::vector<Texture> textures;
    glm::vec3 CoordColor;

	ui32 Vao;

	Mesh(std::vector<Vertex> vertices, std::vector<ui32> indices,
			std::vector<Texture> textures,
            const f32& r, const f32& g, const f32& b)
			: vertices(vertices), indices(indices), textures(textures), 
              CoordColor({g,r,b}){
		setupMesh();
	}

	void Draw(Shader* shader) {
		ui32 diffuseNr  = 1;
		ui32 specularNr = 1;
		ui32 normalNr   = 1;
		ui32 heightNr   = 1;

		for (ui32 i = 0; i < textures.size(); ++i) {
			glActiveTexture(GL_TEXTURE0 + i);
			std::string number;
			std::string name = textures[i].type;
			if (name == "texture_diffuse") {
				number = std::to_string(diffuseNr++);
			} else if (name == "texture_specular") {
				number = std::to_string(specularNr++);
			} else if (name == "texture_normal") {
				number = std::to_string(normalNr++);
			} else if (name == "texture_height") {
				number = std::to_string(heightNr++);
			}
			glUniform1i(glGetUniformLocation(shader->getProgram(), (name + number).c_str()), i);
			glBindTexture(GL_TEXTURE_2D, textures[i].id);
		}
		
		glBindVertexArray(Vao);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glActiveTexture(GL_TEXTURE0);
	}

private:

	ui32 vbo, ebo;

	void setupMesh() {
		glGenVertexArrays(1, &Vao);
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);

		glBindVertexArray(Vao);

		const ui32 sovs = vertices.size()*sizeof(Vertex);
		const ui32 sois = indices.size()*sizeof(ui32);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sovs, &vertices[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sois, &indices[0], GL_STATIC_DRAW);

		const ui32 sov = sizeof(Vertex);
		const void* of1 = (void*)offsetof(Vertex, Normal);
		const void* of2 = (void*)offsetof(Vertex, TexCoords);
		const void* of3 = (void*)offsetof(Vertex, Tangent);
		const void* of4 = (void*)offsetof(Vertex, Bitangent);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sov, (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sov, of1);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sov, of2);
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sov, of3);
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sov, of4);

		glBindVertexArray(0);
	}
};

#endif

