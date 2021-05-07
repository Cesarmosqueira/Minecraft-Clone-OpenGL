#include "block.hpp"
#include "IndexBuffer.hpp"
#include "glutil.h"
class Block {
public:
    glm::mat4 model, projection;
private:
    f32 x, y, z;
    bool solid;
public:
    Block() :
        model(glm::mat4(1.0f)), projection(glm::mat4(1.0f)) {}
    void load_projection_matrix(const ui32& SCR_WIDTH, const ui32& SCR_HEIGHT){
        projection = glm::perspective(glm::radians(45.0f), /* size btw [1, 180] */
            (float)SCR_WIDTH/SCR_HEIGHT, 0.1f, 100.0f);
    }
    bool is_solid() const {return solid;}
    void init(const ui32& x, const ui32& y, const ui32& z, const bool& solid ){
        this->x = x + 1;
        this->y = y;
        this->z = z + 1;
        this->solid =solid;
        model = glm::translate(model, glm::vec3(x, y, z));
    }
};

class World {
public:
    bool wireframe;
private:
    Shader* program;   
    Block ***blocks;
    ui32 textureID; 
    ui32 SCR_WIDTH, SCR_HEIGHT;
    ui32 VAO, VBO; 
    /*TODO: Make this IBO's set fancier*/
    IndexBuffer* UP;
    IndexBuffer* NORTH;
    IndexBuffer* EAST;
    IndexBuffer* WEST;
    IndexBuffer* SOUTH;
    IndexBuffer* DOWN;
    ui32 W, H, D;
public:
    World(const int& code) : program(new Shader("shaders/coord/")) {
        this->wireframe = false;
        W = H = 16; /* 16x16 1 dim mesh */
        D = 256;
        this->mem_init();
        switch(code){
        case 1: 
            textureID = program->loadTexture("blocks/dirt.jpg");
            std::cout << textureID << " [LOADED] \n";
            break;
        }
        blocks_init();
    }
    ~World(){
        /* clean object */
        delete_blocks();
        if(UP) delete UP;
        if(NORTH) delete NORTH;
        if(EAST) delete EAST;
        if(WEST) delete WEST;
        if(EAST) delete EAST;
        if(SOUTH) delete SOUTH;
        if(DOWN) delete DOWN;
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
        for(ui32 z = 0; z < D; z++){
            for(ui32 y = 0; y < H; y++){
                for(ui32 x = 0; x < W; x++){
                    blocks[z][y][x].load_projection_matrix(SCR_WIDTH, SCR_HEIGHT);
                    program->setMat4("model",  blocks[z][y][x].model);
                    program->setMat4("proj",  blocks[z][y][x].projection);

                    glBindVertexArray(VAO);

                    face_draw_call(UP, x, z, y,'U');
                    face_draw_call(NORTH, x, z, y,'N');
                    face_draw_call(EAST, x, z, y,'E');
                    face_draw_call(WEST,x ,z, y,'W');
                    face_draw_call(SOUTH, x, z, y,'S');
                    face_draw_call(DOWN, x, z ,y,'D');
                }
            }
        }
        /* Send block matrices to shaders */
    }
    void face_draw_call(const IndexBuffer* face, const ui32& x, const ui32& y, const ui32& z, const ui8& code) {
        if(on_sight(z, x, y, code)){
            face->bind();
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        } 
    }
    bool on_sight(const ui32& x, const ui32& y, const ui32& z, const ui32& dir){ 
        if(dir == 'D') {
            return z == 0;
        }
        if(dir == 'U'){
            return z == D-1;
        }
        if(dir == 'W') {
            return y == 0;
        }
        if(dir == 'E') {
            return y == H-1;
        }
        if(dir == 'N') {
            return x == 0;
        }
        if(dir == 'S') {
            return x == W-1;
        }
    
        return true;
    }
    Shader* s() { return program; }
    void blocks_init() {
        std::cout << "Block mesh size= ";
        ui32 memsize = 0;
        blocks = new Block**[D];
        memsize += sizeof(blocks);
        for(ui32 z = 0; z < D; z++){
            blocks[z] = new Block*[H];
            memsize += sizeof(blocks[z]);
            for(ui32 y = 0; y < H; y++){
                blocks[z][y] = new Block[W];
                memsize += sizeof(blocks[z][y]);
                for(ui32 x = 0; x < W; x++){
                    blocks[z][y][x].init(x, z, y, 1);
                }
            }
        }
        std::cout << memsize << "bytes \n";
    }
    void delete_blocks() {
        for(ui32 z = 0; z < D; z++){
            for(ui32 y = 0; y < H; y++){
                delete[] blocks[z][y];
            }
            delete[] blocks[z];
        }
        delete[] blocks;
        std::cout << "Blocks deleted succesfully\n";
    }
    void mem_init(){

        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(K::vertices), K::vertices, GL_STATIC_DRAW);

        /*Index buffers for each face of the cube*/
        UP = new IndexBuffer(K::UP, 6);
        NORTH = new IndexBuffer(K::NORTH, 6);
        EAST = new IndexBuffer(K::EAST, 6);
        WEST = new IndexBuffer(K::WEST, 6);
        SOUTH = new IndexBuffer(K::SOUTH, 6);
        DOWN = new IndexBuffer(K::DOWN, 6);

        // posiciones
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // colores
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
        glEnableVertexAttribArray(1);
        // coordenadas de textura
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(6*sizeof(float)));
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);
    }
};
    
    
