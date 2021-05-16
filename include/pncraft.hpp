#include "Konstants.hpp"
#include "Block.hpp"
#include "IndexBuffer.hpp"
#include "ZNOISE_INCLUDE/Perlin.hpp"
#include "ZNOISE_INCLUDE/FBM.hpp"

class World {
public:
    bool wireframe;
private:
    Shader* program;   
    ui32 view_distance;
    Block ***blocks;
    ui32 textureID; 
    i32 SCR_WIDTH, SCR_HEIGHT;
    ui32 VAO, VBO; 
    /*TODO: Make this IBO's set fancier*/
    IndexBuffer* UP;
    IndexBuffer* NORTH;
    IndexBuffer* EAST;
    IndexBuffer* WEST;
    IndexBuffer* SOUTH;
    IndexBuffer* DOWN;
    ui32 SIDE, HEIGHT;
    Perlin perlin;
    FBM* noise;

    Block* b;
    glm::mat4 projection;
public:
    World(const int& code) : 
        program(new Shader("shaders/coord/")), projection(glm::mat4(1.0f)) {

        view_distance = 48;
        this->wireframe = false;
        SIDE = 500; /* 32x32x255 chunk */
        HEIGHT = 64;
        noise = new FBM(perlin);
        this->mem_init();
        switch(code){
        case 1: 
            textureID = program->loadTexture("blocks/dirt.jpg");
            std::cout << textureID << " [LOADED] \n";
            break;
        case 2:
            textureID = program->loadTexture("blocks/pn.jpg");
            std::cout << textureID << " [LOADED] \n";
            break;
        }
        glBindVertexArray(VAO);
        glBindTexture(GL_TEXTURE_2D, textureID);
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
        if(noise) delete noise;
    }
    void update_width_height(const int& w, const int& h) {
        if (SCR_WIDTH != w && SCR_HEIGHT != h) {
            this->SCR_WIDTH = w;
            this->SCR_HEIGHT = h;
            projection = glm::perspective(glm::radians(45.0f), /* size btw [1, 180] */
                (float)SCR_WIDTH/SCR_HEIGHT, 0.1f, 360.0f);

            program->useProgram();
            program->setMat4("proj", projection);
        }
    }
    void send_view_mat(const glm::mat4& view) {
        program->setMat4("view",view); 
        //solid_shader->setMat4("view",view); 
    }
    void toggle_wireframe() { this->wireframe = !this->wireframe;};
    void on_update(const glm::vec3& pp) {
        /* Enable shader */
        program->useProgram();
        /* commpute matrices to send as uniforms */
        glPolygonMode( GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);

        i32 sx = K::max<i32>(pp[0] - view_distance, 0);
        sx = K::min<i32>(sx, SIDE);

        i32 lx = K::min<i32>(pp[0] + view_distance, SIDE);
        lx = K::max<i32>(lx, 0);

        i32 sz = K::max<i32>(pp[2] - view_distance, 0);
        sz = K::min<i32>(sz, SIDE);

        i32 lz = K::min<i32>(pp[2] + view_distance, SIDE);
        lz = K::max<i32>(lz, 0);

        for(ui32 y = 0; y < HEIGHT; y++){

            for(ui32 x = sx; x < lx; x++){

                for(ui32 z = sz; z < lz; z++){

                    b = &blocks[y][x][z];
                    program->setMat4("model",  b->model);

                    if(b->is_solid()) { 
                            //if (!blocks[y+1][x][z].is_solid())
                        block_draw_call(b);
                    }

                }
            }
        }
        /* Send block matrices to shaders */
    }
    void block_draw_call(Block* b) {
        face_draw_call(    UP, b, 'U');
        face_draw_call( NORTH, b, 'N');
        face_draw_call(  EAST, b, 'E');
        face_draw_call(  WEST, b, 'W');
        face_draw_call( SOUTH, b, 'S');
        face_draw_call(  DOWN, b, 'D');
    }
    void face_draw_call(const IndexBuffer* face, Block* b, const ui8& code) { 
        if(!not_visible(b->X(), b->Y(), b->Z(), code)){
            face->bind();
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        } 
    }
    bool not_visible(const ui32& x, const ui32& y, const ui32& z, const ui32& dir){ 
        if(dir == 'D') {
            if ( 0 < y ) {
                return blocks[y-1][x][z].is_solid();
            }
            else return false;
        }
        if(dir == 'N') { 
            if ( x < SIDE - 1 ) {
                return blocks[y][x + 1][z].is_solid();
            }
            else return false;
        }
        if(dir == 'W') { 
            if ( z < SIDE - 1 ) {
                return blocks[y][x][z+1].is_solid();
            }
            else return false;
        }
        if(dir == 'E') { 
            if ( 0 < z ) {
                return blocks[y][x][z-1].is_solid();
            }
            return false;
        }
        if(dir == 'S') { 
            if ( 0 < x ) {
                return blocks[y][x-1][z].is_solid();
            }
            else return false;
        }
        if(dir == 'U'){
            if ( y < HEIGHT-1 ) { 
                return blocks[y+1][x][z].is_solid();
            }
            else return false;
        }
        std::cerr << "Unknown block face\n";
        return false;
    }
    Shader* s() { return program; }
    void blocks_init() {
        std::cout << "Block mesh size= ";
        ui32 memsize = 0;
        blocks = new Block**[HEIGHT];
        memsize += sizeof(blocks);

        for(ui32 y = 0; y < HEIGHT; y++){

            blocks[y] = new Block*[SIDE];
            memsize += sizeof(blocks[y]);

            for(ui32 x = 0; x < SIDE; x++){

                blocks[y][x] = new Block[SIDE];
                memsize += sizeof(blocks[y][x]);

                for(ui32 z = 0; z < SIDE; z++){
                    float h = noise->Get({(float)x,(float)z}, 0.01f) * (float)HEIGHT/2;

                    bool vis = y < h;
                    blocks[y][x][z].init(y, x, z, vis);
                }
            }
        }
        std::cout << memsize << "bytes \n";
    }
    void delete_blocks() {
        for(ui32 y = 0; y < HEIGHT; y++){
            for(ui32 x = 0; x < SIDE; x++){
                delete[] blocks[y][x];
            }
            delete[] blocks[y];
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
    
    
