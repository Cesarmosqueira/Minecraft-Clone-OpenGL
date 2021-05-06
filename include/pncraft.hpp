#include "block.hpp"

class Block {
public:
    glm::mat4 model, projection;
private:
    f32 x, y, z;
    i32 W, H;
public:
    Block(const f32&x, const f32&y, const f32& z, const i32&W, const i32&H):
        model(glm::mat4(1.0f)), projection(glm::mat4(1.0f)) 
    {
        this->x = x; 
        this->y = y; 
        this->z = z; 
        this->W = W; 
        this->H = H;

        model = glm::translate(model, glm::vec3(x, y, z));
        
    }
    ~Block() {
    }
    void on_update(const int& SCR_WIDTH, const int& SCR_HEIGHT){
        /* muito obrigado manito voce e um amigo */
		//model = rotate(model, theta, glm::vec3(0.3, 0.6, 1.0));
        //
		projection = glm::perspective(glm::radians(45.0f), /* size btw [1, 180] */
            (float)SCR_WIDTH/SCR_HEIGHT * W/H, 0.1f, 100.0f);
    }
};

class BlockMesh {
public:
    bool wireframe;
private:
    Shader* program;   
    std::vector<Block> blocks; 
    ui32 textureID; 
    ui32 SCR_WIDTH, SCR_HEIGHT;
    BlockUtil* block_util;
    ui32 W, H, D;
public:
    BlockMesh(const int& code) : program(new Shader("shaders/coord/")) {
        this->wireframe = false;
        block_util = new BlockUtil();
        
        W = H = 1; /* 16x16 1 dim mesh */
        D = 1;
        switch(code){
        case 1: 
            textureID = program->loadTexture("blocks/dirt.jpg");
            std::cout << textureID << " [LOADED] \n";
            break;
        }
        for(ui32 i = 0; i < H; i++){
            for(ui32 j = 0; j < W; j++){
                for(ui32 k = 0; k < D; k++){
                    blocks.push_back(Block(j, i, k, 40, 40));
                }
            }
        }
    }
    ~BlockMesh(){
        /* clean object */
        delete block_util;
        if(program) delete program;
    }
    void update_width_height(const int& w, const int& h) {
        this->SCR_WIDTH = w;
        this->SCR_HEIGHT = h;
    }
    void toggle_wireframe() { this->wireframe = !this->wireframe;};
    void on_update() {
        glBindTexture(GL_TEXTURE_2D, textureID);
        /* Enable shader */
        program->useProgram();

        /* commpute matrices to send as uniforms */
        glPolygonMode( GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
        for(Block b : blocks) {
            b.on_update(SCR_WIDTH, SCR_HEIGHT);
            program->setMat4("model", b.model);
            program->setMat4("proj", b.projection);

            /* Draw call */ 
            /* UP*/
            glBindVertexArray(block_util->DOWN.vao);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }

        /* Send block matrices to shaders */
    }
    Shader* s() { return program; }
};

