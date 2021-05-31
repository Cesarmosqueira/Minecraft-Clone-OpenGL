#include "renderer/Konstants.hpp"
#include "renderer/IndexBuffer.hpp"
#include "resources/Chunk.hpp"
#include <vector>


class World {
public:
    bool wireframe;
private:
    Shader* program;   
    ui32 view_distance;
    ui32 DirtTexture, GrassTexture; 
    i32 SCR_WIDTH, SCR_HEIGHT;
    ui32 VAO, VBO; 

    /*TODO: Make this IBO's set fancier*/
    IndexBuffer* UP;
    IndexBuffer* NORTH;
    IndexBuffer* EAST;
    IndexBuffer* WEST;
    IndexBuffer* SOUTH;
    IndexBuffer* DOWN;

    std::vector<Chunk*> chunks;
    std::vector<Chunk*> MAP;

    Perlin perlin;
    FBM* noise;

    glm::mat4 projection;

    Chunk* current_chunk, *chunk_aux;

    f32 max_h;
    i32 chunking;
    i32 xChunk, zChunk;

public:
    World(const int& code) : 
        program(new Shader("shaders/coord/")), projection(glm::mat4(1.0f)) {
        noise = new FBM(perlin);
        xChunk = 0;
        zChunk = 0;

        chunking = 4;
        max_h = CHUNK_HEIGHT - CHUNK_HEIGHT/4.0f;
        chunk_update();
        //init_first_chunks();
        generate_chunks();
        
        view_distance = 48;
        this->wireframe = false;
        this->mem_init();

        DirtTexture = program->loadTexture("blocks/dirt.jpg");
        std::cout << DirtTexture << " [LOADED] \n";

        GrassTexture = program->loadTexture("blocks/grass.jpg");
        std::cout << DirtTexture << " [LOADED] \n";

        glBindVertexArray(VAO);
        glBindTexture(GL_TEXTURE_2D, DirtTexture);
    }
    ~World(){
        /* clean object */
        i32 ms = MAP.size();
        for(auto c : MAP) {
            if(c) {
                delete c;
            }
        }
        std::cout << "Destroyed " << ms << " chunks from the chunk cache\n";
        if(UP) delete UP;
        if(NORTH) delete NORTH;
        if(EAST) delete EAST;
        if(WEST) delete WEST;
        if(EAST) delete EAST;
        if(SOUTH) delete SOUTH;
        if(DOWN) delete DOWN;
        if(program) delete program;
        if(noise) delete noise;
        if(chunk_aux) delete chunk_aux;
        std::cout << "World deleted succesfully\n";
    }

    static Chunk* find_chunk(const std::vector<Chunk*>& vec, const i32& x, const i32& z){
        for(Chunk* c : vec){
            if (c->X() == x && c->Z() == z){
                return c;
            }   
        }
        return nullptr;
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
        if (int(pp[0]/CHUNK_SIDE) != xChunk || int(pp[2]/CHUNK_SIDE) != zChunk) {
            //update xChunk and zChunk
            xChunk =int(pp[0]/CHUNK_SIDE);
            zChunk =int(pp[2]/CHUNK_SIDE);
            chunk_update();
            generate_chunks();
        }
        /* commpute matrices to send as uniforms */
        glPolygonMode( GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
        for(Chunk* c : chunks)
            chunk_draw_call(c->Data());
        /* Send block matrices to shaders */
    }

    void chunk_draw_call(Block*** data) { 
        for(i32 y =0 ; y < CHUNK_HEIGHT; y++){

            for(i32 x = 0; x < CHUNK_SIDE; x++){

                for(i32 z = 0; z < CHUNK_SIDE; z++){
                    block_draw_call(data, x,y,z);
                }
            }
        }
    }

    void block_draw_call(Block*** data, const i32& x, const i32&y, const i32& z) {
        if ( data[y][x][z].is_solid() ) {

            program->setMat4("model", data[y][x][z].model);
            face_draw_call(data,    UP, x,y,z , 'U');
            face_draw_call(data, NORTH, x,y,z , 'N');
            face_draw_call(data,  EAST, x,y,z , 'E');
            face_draw_call(data,  WEST, x,y,z , 'W');
            face_draw_call(data, SOUTH, x,y,z , 'S');
            if(y > 1) face_draw_call(data,  DOWN, x,y,z , 'D');
        }
    }

    void face_draw_call(Block***& data,const IndexBuffer* face, 
           const i32& x, const i32&y, const i32& z, const ui8& code) { 

        if(!not_visible(data, x,y,z, code)){
            
            face->bind();
            //if (code == 'U') { 
            //    glBindTexture(GL_TEXTURE, DirtTexture);
            //}
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        } 
    }

    bool not_visible(Block***& blocks, const ui32& x, const ui32& y, const ui32& z, 
            const ui32& dir){ 
        if(dir == 'D') { //if in the bottom of the chunk
            if ( 0 < y ) {
                return blocks[y-1][x][z].is_solid();
            }
            else return false;
        }
        /* MUST SUPPORT BLOCKS FACING OTHER CHUNKS */
        if(dir == 'N') { 
            /* - Validate if its on the border of the current chunk - */
            if ( x < CHUNK_SIDE - 1 ) {
                return blocks[y][x + 1][z].is_solid();
            }
            else { //its on the border
                //validate with the adjacent chunk to (x+1)  
                return false;
            }
        }
        if(dir == 'W') { 
            if ( z < CHUNK_SIDE - 1 ) {
                return blocks[y][x][z+1].is_solid();
            }
            else {
                return false;
            }
        }
        if(dir == 'E') { 
            if ( 0 < z ) {
                return blocks[y][x][z-1].is_solid();
            }
            else { 
                return false;
            }
        }
        if(dir == 'S') { 
            if ( 0 < x ) {
                return blocks[y][x-1][z].is_solid();
            }
            else {
                return false;
            }
        }
        /* --------------------------------------- */
        if(dir == 'U'){ //if its on top of the chunk
            if ( y < CHUNK_HEIGHT-1 ) { 
                if (! blocks[y+1][x][z].is_solid() ) {
                    glBindTexture(GL_TEXTURE, GrassTexture);
                    return false;

                }
                else return true;
            }
            else { 
                return false;
            }
        }
        std::cerr << "Unknown block face\n";
        return false;
    }

    Shader* s() { return program; }

    void chunk_update(){
        //search for chunk in MAP cache
        chunks.clear();
        Chunk* act = find_chunk(MAP, xChunk, zChunk);
        //if current chunk is in cache already, current is replaced
        if(!act) {
            act = new Chunk(xChunk, zChunk, noise, max_h);
        }
        current_chunk = act;
        MAP.push_back(act); //map will store all the chunks ever computed
    }
    ///
    /// Generates adjacents chunks to the actual chunk where the player is standing.
    ///
    void generate_chunks() {
        for(i32 i = current_chunk->X() - chunking; i <= current_chunk->X() + chunking; i++){
            for(i32 j = current_chunk->Z() - chunking; j <= current_chunk->Z() + chunking; j++){
                chunk_aux = find_chunk(MAP, i, j);
                if(!chunk_aux) {
                    chunk_aux = new Chunk(i,j, noise, max_h);
                    MAP.push_back(chunk_aux);
                }
                chunks.push_back(chunk_aux);
            }
        }
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
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // shadows
        glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3*sizeof(float)));
        glEnableVertexAttribArray(1);
        // coordenadas de textura
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(4*sizeof(float)));
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);
    }
};
