#include "renderer/Konstants.hpp"
#include "renderer/IndexBuffer.hpp"
#include "resources/Chunk.hpp"
#include "resources/Object.hpp"
#include <vector>
#include <climits>
#define GEN_TYPE 'W'


class World {
public:
    bool wireframe;
    f32 SunSpeed = 0.05f, SunWide = 48.0f;
private:
    Shader* block_shader;   

    /* Object */
    Object* sunobj; 
    Object* monito;
    std::vector<glm::vec3> tree_positions;

    ui32 view_distance;
    ui32 DirtTexture, WaterTexture; 
    ui32 SandTexture, SunTexture;
    ui32 GrassTopTexture, GrassSideTexture;
    ui32 VAO, VBO; 
    i32 BUFFER_W, BUFFER_H;

    /*TODO: Make this IBO's set fancier*/
    IndexBuffer* UP;
    IndexBuffer* NORTH;
    IndexBuffer* EAST;
    IndexBuffer* WEST;
    IndexBuffer* SOUTH;
    IndexBuffer* DOWN;

    std::vector<Chunk*> chunks;
    std::vector<Chunk*> MAP;

    Worley worley;
    Simplex simplex; 
    FBM* noise;

    glm::mat4 projection;
    glm::vec3 lightColor;


    Chunk* current_chunk, *chunk_aux;

    f32 max_h;
    i32 chunking;
    i32 xChunk, zChunk;
    ui32 current_frame = 0;

public:
    World() {
        projection = glm::mat4(1.0f);
        ui32 n_trees = 5;

        block_shader = new Shader("shaders/default/");


        /*Object*/
        //Sun 
        sunobj = new Object("shaders/coord/", "salsolcito/", "Sunmodel.obj", "sun.jpg");
        sunobj->pos = {0, 65, 0};
        sunobj->scale_value = 20.0f;

        //Monito 
        monito = new Object("shaders/mob/", "monito/", "monito.obj", "rubik.jpg");
        monito->pos = {0, 36, 0};
        monito->scale_value = 2.0f;

        /*done Object*/
        
        if(GEN_TYPE == 'W') {
            noise = new FBM(worley);
        } else if (GEN_TYPE == 'S'){
            noise = new FBM(simplex);
        }
        xChunk = 0;
        zChunk = 0;

        chunking = 5;
        max_h = CHUNK_HEIGHT - CHUNK_HEIGHT/4.0f;
        chunk_update();
        generate_chunks();

        for(ui8 i = 0 ; i < n_trees; i++) {
            ui32 j = rand() % CHUNK_SIDE;
            ui32 k = rand() % CHUNK_SIDE;
            ui32 h = current_chunk->heightmap[j][k];
            tree_positions.push_back({j, h + 5, k});
        }
        
        view_distance = 48;
        this->wireframe = false;
        this->mem_init();

        /* TODO: Make this a tilemap */
        DirtTexture = block_shader->loadTexture("blocks/dirt.jpg");

        GrassTopTexture = block_shader->loadTexture("blocks/grassTop.jpg");

        GrassSideTexture = block_shader->loadTexture("blocks/grassSide.jpg");

        WaterTexture = block_shader->loadTexture("blocks/water.jpg");

        SandTexture = block_shader->loadTexture("blocks/sand.jpg");


        lightColor = {1.0f, 1.0f, 1.0f};

        glBindVertexArray(VAO);
    }

    ~World(){
        /* clean object */
        std::cout << "About to destroy chunks\n";
        i32 ms = MAP.size();
        for(auto c : MAP) {
            if(c) {
                c = nullptr;
            }
        }
        MAP.clear();
        std::cout << "Destroyed " << ms << " chunks from the chunk cache\n";

        //GLuint texs[] = { DirtTexture, WaterTexture,SandTexture, SunTexture,GrassTopTexture, GrassSideTexture};
        //glDeleteTextures(sizeof(texs)/sizeof(GLuint), texs);
        delete noise;
        delete sunobj;
        delete monito;
        delete block_shader;
        if(UP) delete UP;
        if(NORTH) delete NORTH;
        if(EAST) delete EAST;
        if(WEST) delete WEST;
        if(EAST) delete EAST;
        if(SOUTH) delete SOUTH;
        if(DOWN) delete DOWN;
    }

    /* FOR GUI PURPOSES */
    void rearrange_trees() {
        for(glm::vec3& p : tree_positions) {
            ui32 j = rand() % CHUNK_SIDE;
            ui32 k = rand() % CHUNK_SIDE;
            ui32 h = current_chunk->heightmap[j][k];
            p = {j, h + 5, k};
        }
    }
    i32& get_chunking(){
        return chunking;
    }
    glm::vec3& get_lsrc_pos(){
        return sunobj->pos;
    }
    float u_starting_specular = 100.0f;
    float u_starting_ambient = 80.0f;
    float u_minimum_ambient = 50.0f;

    f32& monito_scale() { return monito->scale_value;}
    f32& monito_specular(){ return monito->u_starting_specular;}
    f32& monito_ambient(){ return monito->u_starting_ambient;}
    f32& monito_min_ambient(){ return monito->u_minimum_ambient;}
    glm::vec3& get_monito_pos() {return monito->pos; }


    f32& sun_scale() { return sunobj->scale_value;}
    /* END OF FOR GUI PURPOSES  XD */

    static Chunk* find_chunk(const std::vector<Chunk*>& vec, const i32& x, const i32& z){
        for(Chunk* c : vec){
            if (c->X() == x && c->Z() == z){
                return c;
            }   
        }
        return nullptr;
    }
        

    void on_update(glm::vec3 pp, const glm::mat4& camView) {
        current_frame = (current_frame+1) % (INT_MAX-6);
        block_shader->useProgram();
        glBindVertexArray(VAO);

        if (int(pp[0]/CHUNK_SIDE) != xChunk || int(pp[2]/CHUNK_SIDE) != zChunk) {
            //update xChunk and zChunk
            xChunk =int(pp[0]/CHUNK_SIDE);
            zChunk =int(pp[2]/CHUNK_SIDE);
            chunk_update();
            generate_chunks();
        }
        
        monito->pos.x = 65*sin(current_frame*0.01);
        monito->pos.y = 10*sin(current_frame*0.1) + WATER_LEVEL + 20;
        monito->pos.z = 65*cos(current_frame*0.01);
        sunobj->pos.x = SunWide*sin(current_frame*SunSpeed);
        sunobj->pos.z = SunWide*cos(current_frame*SunSpeed);



        block_shader->setVec3("xyz", sunobj->pos);
        block_shader->setVec3("xyzColor", lightColor);
        block_shader->setMat4("xyzView", camView);

        glPolygonMode( GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
        block_shader->useProgram();

        block_shader->setFloat("u_starting_ambient", u_starting_ambient);
        block_shader->setFloat("u_starting_specular", u_starting_specular);
        block_shader->setFloat("u_minimum_ambient", u_minimum_ambient);

        for(Chunk* c : chunks){
            chunk_draw_call(c->Data(), c->heightmap);
        }
        //godzillas chunk
        //godzilla->pos.y = find_chunk(chunks, godzilla->pos.x, godzilla->pos.z)->heightmap[int(godzilla->pos.x)][int(godzilla->pos.z)];
        monito->on_update(pp, lightColor, sunobj->pos);
        sunobj->on_draw_call();
    }


    void update_width_height(const i32& w, const i32& h) {
        if (w != this->BUFFER_W && h != this->BUFFER_H) {
            std::cout << "Resizing\n";
            this->BUFFER_W = w;
            this->BUFFER_H = h;
            projection = glm::perspective(glm::radians(45.0f), /* size btw [1, 180] */
                (float)w/h, 0.1f, 360.0f);

            block_shader->useProgram();
            block_shader->setMat4("proj", projection);
            
            sunobj->send_proj(projection);
            monito->send_proj(projection);
        }
    }

    void send_view_mat(const glm::mat4& view) {
        block_shader->useProgram();
        block_shader->setMat4("view",view); 

        sunobj->send_view(view);
        monito->send_view(view);
    }

    void toggle_wireframe() { this->wireframe = !this->wireframe;};

private:
    void chunk_draw_call(Block*** data, i32 hm[CHUNK_SIDE][CHUNK_SIDE]) { 
        i32 s;
        for(i32 x = 0; x < CHUNK_SIDE; x++){
            for(i32 z = 0; z < CHUNK_SIDE; z++){
                s = hm[x][z];
                block_draw_call(data, x,(s-1 >= 0) ? s-1 : 0,z);
                block_draw_call(data, x,WATER_LEVEL,z);
            }
        }
    }

    void block_draw_call(Block*** data, const i32& x, const i32&y, const i32& z) {
        if ( data[y][x][z].is_solid() ) {
            ui8 code = data[y][x][z].get_TexCode();
            if(code != 'G'){
                block_shader->setVec3("xyzBlock", { data[y][x][z].X(),
                                                    data[y][x][z].Y(),
                                                    data[y][x][z].Z() });
                glBindTexture(GL_TEXTURE_2D, code_to_tex(code));

                face_draw_call(data,    UP, x,y,z , 'U');
                face_draw_call(data, NORTH, x,y,z , 'N');
                face_draw_call(data,  EAST, x,y,z , 'E');
                face_draw_call(data,  WEST, x,y,z , 'W');
                face_draw_call(data, SOUTH, x,y,z , 'S');
                if(y > 1) face_draw_call(data,  DOWN, x,y,z , 'D');
            } else {
                block_shader->setVec3("xyzBlock", { data[y][x][z].X(),
                                                    data[y][x][z].Y(),
                                                    data[y][x][z].Z() });


                glBindTexture(GL_TEXTURE_2D, GrassTopTexture);
                face_draw_call(data,    UP, x,y,z , 'U');
                glBindTexture(GL_TEXTURE_2D, GrassSideTexture);
                face_draw_call(data, NORTH, x,y,z , 'N');
                face_draw_call(data,  EAST, x,y,z , 'E');
                face_draw_call(data,  WEST, x,y,z , 'W');
                face_draw_call(data, SOUTH, x,y,z , 'S');
                if(y > 1){
                    glBindTexture(GL_TEXTURE_2D, DirtTexture);
                    face_draw_call(data,  DOWN, x,y,z , 'D');
                }
        
            }
        }
    }

    ui32 code_to_tex(const ui8& code){
        switch(code){
        case 'W': return WaterTexture;
        case 'D': return DirtTexture;
        case 'S': return SandTexture;
        }
        std::cout << "Undefined texture code\n";
        return 1;
    }

    void face_draw_call(Block***& data,const IndexBuffer* face, 
           const i32& x, const i32&y, const i32& z, const ui8& code) { 
        
        if(!not_visible(data, x,y,z, code)){
            switch(code){
            case 'N': block_shader->setVec3("normal", { 1, 0, 0} ); break;
            case 'E': block_shader->setVec3("normal", { 0, 0,-1} ); break;
            case 'U': block_shader->setVec3("normal", { 0, 1, 0} ); break;
            case 'D': block_shader->setVec3("normal", { 0,-1, 0} ); break;
            case 'W': block_shader->setVec3("normal", { 0, 0, 1} ); break;
            case 'S': block_shader->setVec3("normal", {-1, 0, 0} ); break;
            }
            block_shader->setMat4("model", data[y][x][z].model);
            face->bind();
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
                    glBindTexture(GL_TEXTURE, GrassTopTexture);
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

    void chunk_update(){
        //search for chunk in MAP cache
        chunks.clear();
        Chunk* act = find_chunk(MAP, xChunk, zChunk);
        //if current chunk is in cache already, current is replaced
        if(!act) {
            act = new Chunk(xChunk, zChunk, noise, max_h, GEN_TYPE);
        }
        current_chunk = act;
        MAP.push_back(act); //map will store all the chunks ever computed
    }
    ///
    /// Generates adjacents chunks to the actual chunk where the player is standing.
    ///
    void generate_chunks() {
        int total = 0;
        for(i32 i = current_chunk->X() - chunking; i <= current_chunk->X() + chunking; i++){
            for(i32 j = current_chunk->Z() - chunking; j <= current_chunk->Z() + chunking; j++){
                chunk_aux = find_chunk(MAP, i, j);
                if(!chunk_aux) {
                    chunk_aux = new Chunk(i,j, noise, max_h, GEN_TYPE);
                    total++;
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
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // shadows
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
        glEnableVertexAttribArray(1);
        //normal
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(5*sizeof(float)));
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);
    }

};
