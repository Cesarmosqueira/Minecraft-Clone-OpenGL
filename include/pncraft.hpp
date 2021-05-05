#include "glutil.h"
const ui32 FSIZE = sizeof(float);
const f32  ASPECT = 16.0f / 9.0f;

f32 vertices[] = {
	 // posiciones        colores          texturas
	-0.5f,  0.5f,  0.5f,   1.0f, 0.0, 0.0,   0.0f, 1.0f,  // 0
	 0.5f,  0.5f,  0.5f,   1.0f, 0.0, 0.0,   1.0f, 1.0f,  // 1
	-0.5f, -0.5f,  0.5f,   1.0f, 0.0, 0.0,   0.0f, 0.0f,  // 2
	 0.5f, -0.5f,  0.5f,   1.0f, 0.0, 0.0,   1.0f, 0.0f,  // 3
	-0.5f,  0.5f, -0.5f,   1.0f, 0.0, 0.0,   1.0f, 1.0f,  // 4
	 0.5f,  0.5f, -0.5f,   1.0f, 0.0, 0.0,   0.0f, 1.0f,  // 5
	-0.5f, -0.5f, -0.5f,   1.0f, 0.0, 0.0,   1.0f, 0.0f,  // 6
	 0.5f, -0.5f, -0.5f,   1.0f, 0.0, 0.0,   0.0f, 0.0f,  // 7

	-0.5f,  0.5f,  0.5f,   1.0f, 0.0, 0.0,   0.0f, 0.0f,  // 8
	 0.5f,  0.5f,  0.5f,   1.0f, 0.0, 0.0,   1.0f, 0.0f,  // 9
	-0.5f, -0.5f,  0.5f,   1.0f, 0.0, 0.0,   0.0f, 0.0f,  // 10
	 0.5f, -0.5f,  0.5f,   1.0f, 0.0, 0.0,   1.0f, 0.0f,  // 11
	-0.5f,  0.5f, -0.5f,   1.0f, 0.0, 0.0,   0.0f, 1.0f,  // 12
	 0.5f,  0.5f, -0.5f,   1.0f, 0.0, 0.0,   1.0f, 1.0f,  // 13
	-0.5f, -0.5f, -0.5f,   1.0f, 0.0, 0.0,   0.0f, 1.0f,  // 14
	 0.5f, -0.5f, -0.5f,   1.0f, 0.0, 0.0,   1.0f, 1.0f}; // 15
ui32 idxs[] = {
	 0,  1,  2,   1,  2,  3,
	 8,  9, 12,   9, 12, 13,
	 1,  5,  3,   3,  5,  7,
	11, 14, 15,  10, 11, 14,
	 0,  4,  6,   0,  2,  6,
	 4,  5,  6,   5,  6,  7};

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
    ui32 dcon;
    ui32 textureID; 
    ui32 SCR_WIDTH, SCR_HEIGHT;
	ui32 vbo, vao, ebo;
    ui32 W, H, D;
public:
    BlockMesh(const int& code) : program(new Shader("shaders/coord/")) {
        this->wireframe = false;
        this->vbo = 0;
        this->vao = 0;
        this->ebo = 0;
        dcon = 150;
        
        W = H = 20; /* 16x16 1 dim mesh */
        D = 2;
        switch(code){
        case 1: 
            textureID = program->loadTexture("blocks/dirt.jpg");
            std::cout << textureID << " [LOADED] \n";
            break;
        }
        for(ui32 i = 0; i < H; i++){
            for(ui32 j = 0; j < W; j++){
                for(ui32 k = 0; k < D; k++){
                    blocks.push_back(Block(j, rand() % 10, rand () % W, 40, 40));
                }
            }
        }
        this->mesh_init();
    }
    ~BlockMesh(){
        /* clean object */
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ebo);
        if(program) delete program;
    }
    void update_width_height(const int& w, const int& h) {
        this->SCR_WIDTH = w;
        this->SCR_HEIGHT = h;
    }
    void toggle_wireframe() { this->wireframe = !this->wireframe;};
    void mesh_init(){
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);
        glBindVertexArray(vao);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(idxs), idxs, GL_STATIC_DRAW);

        // posiciones
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*FSIZE, (void*)0);
        glEnableVertexAttribArray(0);
        // colores
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*FSIZE, (void*)(3*FSIZE));
        glEnableVertexAttribArray(1);
        // coordenadas de textura
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*FSIZE, (void*)(6*FSIZE));
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);
    }
    void on_update() {
        glBindTexture(GL_TEXTURE_2D, textureID);
        /* Enable shader */
        program->useProgram();
        if(dcon < 2) {
            blocks.erase(blocks.begin() + rand()% blocks.size());
            dcon = 50;
        }


        /* commpute matrices to send as uniforms */
        glPolygonMode( GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
        for(Block b : blocks) {
            b.on_update(SCR_WIDTH, SCR_HEIGHT);
            program->setMat4("model", b.model);
            program->setMat4("proj", b.projection);

            /* Draw call */ 
            glBindVertexArray(vao);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        }
        dcon--;
        

        /* Send block matrices to shaders */
    }
    Shader* s() { return program; }

};

