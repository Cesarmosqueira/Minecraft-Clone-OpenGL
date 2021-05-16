#include "Konstants.hpp"
#include "IndexBuffer.hpp"
#include "Chunk.hpp"

class World {
public:
    bool wireframe;
private:
    Shader* program;   
    ui32 view_distance;
    ui32 textureID; 
    i32 SCR_WIDTH, SCR_HEIGHT;
    ui32 VAO, VBO; 
    Chunk*** chunks;
    /*TODO: Make this IBO's set fancier*/
    IndexBuffer* UP;
    IndexBuffer* NORTH;
    IndexBuffer* EAST;
    IndexBuffer* WEST;
    IndexBuffer* SOUTH;
    IndexBuffer* DOWN;

    Perlin perlin;
    FBM* noise;
    glm::mat4 projection;
    ui32 chunks_count;
public:
    World(const int& code) : 
        program(new Shader("shaders/coord/")), projection(glm::mat4(1.0f)) {
        chunks_count = 4; 
        chunks = new Chunk**[chunks_count];
        for(ui32 i = 0; i < chunks_count; i++) {
            chunks[i] = new Chunk*[chunks_count];
            for(ui32 j = 0; j < chunks_count; j++) { 
                chunks[i][j] = new Chunk(i,j, noise);
            }
        }

        view_distance = 48;
        this->wireframe = false;
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
    }
    ~World(){
        /* clean object */
        

        for(ui32 i = 0; i < chunks_count; i++) {
            for(ui32 j = 0; j < chunks_count; j++) { 
                delete chunks[i][j];
            }
            delete[] chunks[i];
        }
        delete[] chunks;
        std::cout << "Chunks deleted succesfully\n";

        if(UP) delete UP;
        if(NORTH) delete NORTH;
        if(EAST) delete EAST;
        if(WEST) delete WEST;
        if(EAST) delete EAST;
        if(SOUTH) delete SOUTH;
        if(DOWN) delete DOWN;
        if(program) delete program;
        if(noise) delete noise;
        std::cout << "World deleted succesfully\n";
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
        sx = K::min<i32>(sx, CHUNK_SIDE);

        i32 lx = K::min<i32>(pp[0] + view_distance, CHUNK_SIDE);
        lx = K::max<i32>(lx, 0);

        i32 sz = K::max<i32>(pp[2] - view_distance, 0);
        sz = K::min<i32>(sz, CHUNK_SIDE);

        i32 lz = K::min<i32>(pp[2] + view_distance, CHUNK_SIDE);
        lz = K::max<i32>(lz, 0);



        
        for(int i = 0; i < 4; i++ ) {
            for(int j = 0; j < 4; j++ ) {
                chunk_draw_call(chunks[i][j]->Data());
            }
        }
        /* Send block matrices to shaders */
    }

    void chunk_draw_call(Block*** data) { 
        for(ui32 y = 0; y < CHUNK_HEIGHT; y++){

            for(ui32 x = 0; x < CHUNK_SIDE; x++){

                for(ui32 z = 0; z < CHUNK_SIDE; z++){
                    block_draw_call(data, x,y,z);
                }
            }
        }


    }

    void block_draw_call(Block***& data, const i32& x, const i32&y, const i32& z) {
        if ( data[y][x][z].is_solid() ) {
            program->setMat4("model", data[y][x][z].model);
            face_draw_call(data,    UP, y,x,z,  'U');
            face_draw_call(data, NORTH, y,x,z,  'N');
            face_draw_call(data,  EAST, y,x,z,  'E');
            face_draw_call(data,  WEST, y,x,z,  'W');
            face_draw_call(data, SOUTH, y,x,z,  'S');
            face_draw_call(data,  DOWN, y,x,z,  'D');
        }
    }
    void face_draw_call(Block***& data,const IndexBuffer* face, 
           const i32& x, const i32&y, const i32& z, const ui8& code) { 
        if(!not_visible(data, x,y,z, code)){
            face->bind();
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        } 
    }
    bool not_visible(Block***& blocks, const ui32& x, const ui32& y, const ui32& z, const ui32& dir){ 
        if(dir == 'D') {
            if ( 0 < y ) {
                return blocks[y-1][x][z].is_solid();
            }
            else return false;
        }
        if(dir == 'N') { 
            if ( x < CHUNK_SIDE - 1 ) {
                return blocks[y][x + 1][z].is_solid();
            }
            else return false;
        }
        if(dir == 'W') { 
            if ( z < CHUNK_SIDE - 1 ) {
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
            if ( y < CHUNK_HEIGHT-1 ) { 
                return blocks[y+1][x][z].is_solid();
            }
            else return false;
        }
        std::cerr << "Unknown block face\n";
        return false;
    }
    Shader* s() { return program; }
    
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
    
    
