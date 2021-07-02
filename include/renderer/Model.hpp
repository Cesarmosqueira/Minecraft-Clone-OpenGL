#ifndef __MODEL_H__
#define __MODEL_H__

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "renderer/Mesh.hpp"
#include "util/Files.hpp"

ui32 TextureFromFile(const std::string& fileName, i32 param = GL_LINEAR) {
	ui32 texture;

	glGenTextures(1, &texture);

	i32 w, h, nrChannels;

	//stbi_set_flip_vertically_on_load(true); // porque en opgl el eje Y invertio
    std::cout << "Loading from: " << fileName << "\n";
	ui8* data = stbi_load(fileName.c_str(), &w, &h, &nrChannels, 0);
	if (data == nullptr) {
		std::cerr << "Can't load texture\n";
		return -1;
	}
	GLenum fmt;

	if (nrChannels == 4) {
		fmt = GL_RGBA;
	} else if (nrChannels == 3) {
		fmt = GL_RGB;
	} else {
		fmt = GL_RED;
	}

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, fmt, w, h, 0, fmt, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, param);
	stbi_image_free(data);

	return texture;
}

class Model {
	public:
		Files* files;
		std::vector<Texture> textures_loaded;
		std::vector<Mesh> meshes;
		bool gammaCorrection;

		Model(Files* files, std::string const& fileName, bool gamma=false)
			: files(files), gammaCorrection(gamma) {
				loadModel(files->objectFile(fileName));
			}

		void Draw(Shader* shader) {
			for (ui32 i = 0; i < meshes.size(); ++i) {
                shader->setVec3("CoordColor", meshes[i].CoordColor);
				meshes[i].Draw(shader);
			}
		}
	private:
		void loadModel(std::string const& fileName) {
			Assimp::Importer importer;
            std::cout << "-> Ladoading model from '" << fileName << "' <-\n";
			const aiScene* scene = importer.ReadFile(fileName,
					aiProcess_Triangulate
					| aiProcess_GenSmoothNormals
					| aiProcess_FlipUVs
					| aiProcess_CalcTangentSpace);
			if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE
					|| !scene->mRootNode) {
				std::cerr << "ERROR::Assimp::"
					<< importer.GetErrorString() << std::endl;
			}
			processNode(scene->mRootNode, scene);
		}
		void processNode(aiNode* node, const aiScene* scene) {
			for (ui32 i = 0; i < node->mNumMeshes; ++i) {
				aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
				meshes.push_back(processMesh(mesh, scene));
			}
			for (ui32 i = 0; i < node->mNumChildren; ++i) {
				processNode(node->mChildren[i], scene);
			}
		}
		Mesh processMesh(aiMesh *mesh, const aiScene *scene) {
			std::vector<Vertex> vertices;
			std::vector<ui32> indices;
			std::vector<Texture> textures;

			for (ui32 i = 0; i < mesh->mNumVertices; ++i) {
				Vertex vertex;

				vertex.Position = {mesh->mVertices[i].x,
					mesh->mVertices[i].y,
					mesh->mVertices[i].z };
				if (mesh->HasNormals()) {
					vertex.Normal = {mesh->mNormals[i].x,
						mesh->mNormals[i].y,
						mesh->mNormals[i].z };
				}
				if (mesh->mTextureCoords[0]) {
					vertex.TexCoords = {mesh->mTextureCoords[0][i].x,
						mesh->mTextureCoords[0][i].y };
					vertex.Tangent = {mesh->mTangents[i].x,
						mesh->mTangents[i].y,
						mesh->mTangents[i].z };
					vertex.Bitangent = {mesh->mBitangents[i].x,
						mesh->mBitangents[i].y,
						mesh->mBitangents[i].z };
				} else {
					vertex.TexCoords = glm::vec2(0.0f);
				}
				vertices.push_back(vertex);
			}
			for (ui32 i = 0; i < mesh->mNumFaces; ++i) {
				aiFace face = mesh->mFaces[i];
				for (ui32 j = 0; j < face.mNumIndices; ++j) {
					indices.push_back(face.mIndices[j]);
				}
			}
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
            aiColor3D scene_color(0.f,0.f,0.f);
            material->Get(AI_MATKEY_COLOR_DIFFUSE, scene_color);
            std::cout << "Color: " << scene_color.r << ", " <<
                                      scene_color.g << ", " << 
                                      scene_color.b << "\n";
			std::vector<Texture> diffuseMaps = loadMaterialTextures(material,
					aiTextureType_DIFFUSE, "texture_diffuse");
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
			std::vector<Texture> specularMaps = loadMaterialTextures(material,
					aiTextureType_SPECULAR, "texture_specular");
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
			std::vector<Texture> normalMaps = loadMaterialTextures(material,
					aiTextureType_HEIGHT, "texture_normal");
			textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
            std::vector<Texture> heightMaps = loadMaterialTextures(material,
					aiTextureType_AMBIENT, "texture_height");
			textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

			return Mesh(vertices, indices, textures, 
                    scene_color.r, scene_color.b, scene_color.b);
		}

		std::vector<Texture> loadMaterialTextures(aiMaterial* mat,
				aiTextureType type, std::string typeName) {
			std::vector<Texture> textures;
			for (ui32 i = 0; i < mat->GetTextureCount(type); ++i) {
				aiString str;
				mat->GetTexture(type, i, &str);
				bool skip = false;
				for (ui32 j = 0; j < textures_loaded.size(); ++j) {
					if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0) {
						textures.push_back(textures_loaded[j]);
						skip = true;
						break;
					}
				}
				if (!skip) {
					Texture texture;
					std::string texFile = files->objectFile(std::string(str.C_Str()));
					texture.id = TextureFromFile(texFile);
					texture.type = typeName;
					texture.path = str.C_Str();
					textures.push_back(texture);
					textures_loaded.push_back(texture);
				}
			}
			return textures;
		}
};

#endif
