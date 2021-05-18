#include "Block.hpp"
#include "ZNOISE_INCLUDE/Perlin.hpp"
#include "ZNOISE_INCLUDE/FBM.hpp"
#define CHUNK_HEIGHT 128
#define CHUNK_SIDE 16
float h;
bool vis;

class Chunk { 
public: 
    Chunk(const ui32& _x, const ui32& _z, FBM*& noise, const f32& max_h) : XPOS(_x), ZPOS(_z) { 
        XOFF = XPOS * CHUNK_SIDE;
        ZOFF = ZPOS * CHUNK_SIDE;

        blocks = new Block**[CHUNK_HEIGHT];
        for(ui32 y = 0; y < CHUNK_HEIGHT; y++) { 

            blocks[y] = new Block*[CHUNK_SIDE];
            for(ui32 x = 0; x < CHUNK_SIDE; x++){

                blocks[y][x] = new Block[CHUNK_SIDE];

                for(ui32 z = 0; z < CHUNK_SIDE; z++){
            
                    h = noise->Get({(float)(x+XOFF),(float)(z+ZOFF)}, 0.007f) * max_h;
                    vis = y && y < h;
                    blocks[y][x][z].init(y, x + XOFF, z + ZOFF, vis);

                }
            }
        }
    } 
    ~Chunk() { 
        delete_blocks();
    }
    Block*** Data() {
        return blocks;
    }
    int X() { 
        return XPOS;
    }
    int Z() { 
        return ZPOS;
    }
private:
    void delete_blocks() {
        for(ui32 y = 0; y < CHUNK_HEIGHT; y++){
            for(ui32 x = 0; x < CHUNK_SIDE; x++){
                delete[] blocks[y][x];
            }
            delete[] blocks[y];
        }
        delete[] blocks;
    }
     
private:
    ui32 XPOS, ZPOS;
    Block ***blocks;
public:
    ui32 XOFF, ZOFF;

};
