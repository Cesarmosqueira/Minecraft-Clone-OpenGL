#ifndef NPC_HPP
#define NPC_HPP
#include "util/glutil.h"
#include "renderer/Model.hpp"
#include "util/Files.hpp"

class NPC {
public:
    glm::vec3 pos = {0,0,0};
    f32 scale_value = 1.0f;
    ui32 tilemap;

    NPC() {
        files_info = new Files("shaders/mob", "assets/textures", "assets/objects");
        main_shader = new Shader("shaders/mob/", "assets/objects/goodModel/");
        object_model = new Model(files_info, "goodModel/GodzillaLegendaryRigged.obj");
        
		this->model_matrix = glm::mat4(1.0f);
		this->model_matrix = glm::translate(this->model_matrix, pos);
        tilemap = main_shader->loadTexture("GodzillaTexture.png");
    }
    ~NPC() {
        glDeleteTextures(1, &tilemap);
        delete main_shader;
        delete object_model;
        delete files_info;
    }

    float u_starting_ambient = 1;
    float u_starting_specular = 1;
    float u_minimum_ambient = 1;
    void on_update(const glm::vec3& pp, const glm::vec3& lightColor){
        this->model_matrix = glm::translate(glm::mat4(1.0f), pos);
        this->model_matrix = glm::scale(this->model_matrix, glm::vec3(scale_value));

        main_shader->useProgram();
        main_shader->setVec3("xyz", pos);
		main_shader->setVec3("xyzColor", lightColor);
		main_shader->setVec3("xyzView", pp);

        main_shader->setVec3("worldPos", pos);
        main_shader->setFloat("u_starting_ambient", u_starting_ambient);
        main_shader->setFloat("u_starting_specular", u_starting_specular);
        main_shader->setFloat("u_minimum_ambient", u_minimum_ambient);

		main_shader->setMat4("model", this->model_matrix);
        
        glBindTexture(GL_TEXTURE_2D, tilemap);
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
