#ifndef OBJECT_HPP
#define OBJECT_HPP
#include "util/glutil.h"
#include "renderer/Model.hpp"
#include "util/Files.hpp"

class Object {
public:
    glm::vec3 pos = {0,0,0};
    f32 scale_value = 1.0f;
    ui32 tilemap;
    bool textured;
    Object(const std::string& shadersPath, 
           const std::string& modelPath, 
           const std::string& modelName, 
           const std::string& textureName) {
        
        this->textured = textureName != "None";
        files_info = new Files(shadersPath, "assets/textures", "assets/objects/");
        main_shader = new Shader(shadersPath, "assets/objects/"+modelPath);
        object_model = new Model(files_info, modelPath + modelName);
        
		this->model_matrix = glm::mat4(1.0f);
		this->model_matrix = glm::translate(this->model_matrix, pos);

        if(this->textured){
            tilemap = main_shader->loadTexture(textureName);
        }
    }
    ~Object() {
        glDeleteTextures(1, &tilemap);
        delete main_shader;
        delete object_model;
        delete files_info;
    }

    float u_starting_ambient = 64;
    float u_starting_specular = 202;
    float u_minimum_ambient = 144;

    void on_update(const glm::vec3& pp, const glm::vec3& lightColor, const glm::vec3& lightsrc){
        this->model_matrix = glm::translate(glm::mat4(1.0f), pos);
        this->model_matrix = glm::scale(this->model_matrix, glm::vec3(scale_value));

        main_shader->useProgram();
        main_shader->setVec3("xyz", lightsrc);
		main_shader->setVec3("xyzColor", lightColor);
		main_shader->setVec3("xyzView", pp);

        main_shader->setVec3("worldPos", pos);
        main_shader->setFloat("u_starting_ambient", u_starting_ambient);
        main_shader->setFloat("u_starting_specular", u_starting_specular);
        main_shader->setFloat("u_minimum_ambient", u_minimum_ambient);

        on_draw_call(false);
    }
    void on_multiple_update(const glm::vec3& pp, const glm::vec3& lightColor,
            const glm::vec3& lightsrc, const std::vector<glm::vec3>& positions){

        for(glm::vec3 p : positions) {
            pos = p;
            this->on_update(pp, lightColor, lightsrc);
        }

    }
    void on_draw_call(bool simple=true) {
        if(simple) {
            this->model_matrix = glm::translate(glm::mat4(1.0f), pos);
            this->model_matrix = glm::scale(this->model_matrix, glm::vec3(scale_value));
            main_shader->useProgram();
        }
		main_shader->setMat4("model", this->model_matrix);
        if(this->textured) {
            glBindTexture(GL_TEXTURE_2D, tilemap);
        }
		object_model->Draw(main_shader);

    }

    void send_proj(const glm::mat4& proj){
        main_shader->useProgram();
        main_shader->setMat4("proj", proj);
    }

    void send_view(const glm::mat4& view){
        main_shader->useProgram();
        main_shader->setMat4("view", view);
    }
private:
    Shader* main_shader;
    Model* object_model;
    Files* files_info;
    glm::mat4 model_matrix;
    glm::vec3 position;
};

#endif
