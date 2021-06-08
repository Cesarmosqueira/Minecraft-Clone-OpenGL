#include "resources/Sun.hpp"
#include "ZNOISE_INCLUDE/Perlin.hpp"
#include "ZNOISE_INCLUDE/Worley.hpp"
#include "ZNOISE_INCLUDE/FBM.hpp"
#define CHUNK_HEIGHT 128
#define CHUNK_SIDE 16
#define WATER_LEVEL 16
bool vis;
char tex;

class Chunk { 
public: 
    Chunk(const i32& _x, const i32& _z, FBM*& noise, const f32& max_h) : XPOS(_x), ZPOS(_z) { 
        XOFF = XPOS * CHUNK_SIDE;
        ZOFF = ZPOS * CHUNK_SIDE;

        for(i32 x = 0; x < CHUNK_SIDE; x++){
            for(i32 z = 0; z < CHUNK_SIDE; z++){
                heightmap[x][z] = noise->Get({(float)(x+XOFF),(float)(z+ZOFF)}, 0.0055f) * CHUNK_HEIGHT;
            }
        }

        blocks = new Block**[CHUNK_HEIGHT];
        for(i32 y = 0; y < CHUNK_HEIGHT; y++) { 

            blocks[y] = new Block*[CHUNK_SIDE];
            for(i32 x = 0; x < CHUNK_SIDE; x++){

                blocks[y][x] = new Block[CHUNK_SIDE];

                for(i32 z = 0; z < CHUNK_SIDE; z++){
                    tex = 0;
                    if(y < heightmap[x][z]) {
                        vis = true;
                        tex = 'D';
                        if(y >= heightmap[x][z]-1 && y >= WATER_LEVEL) {
                            tex ='G';
                        }
                    } else vis = false;
                    blocks[y][x][z].init(y, x + XOFF, z + ZOFF, vis, tex);
                }
            }
        }
        for(ui32 x = 0; x < CHUNK_SIDE; x++){
            for(ui32 z = 0; z < CHUNK_SIDE; z++){
                    
                for(int i = 0; i <= WATER_LEVEL; i++){
                    if ( !blocks[i][x][z].Solid() ) {
                        blocks[i][x][z].Solid() = true; //water is a solid tho
                        blocks[i][x][z].get_TexCode() = 'W';
                    } else {
                        if(i == WATER_LEVEL){
                            blocks[i][x][z].get_TexCode() = 'S';
                        }
                    }
                    
                }
            }
        }

    } 
    ~Chunk() { 
        std::cout << "About to destroy some chunk\n";
        for(ui32 y = 0; y < CHUNK_HEIGHT; y++) { 

            for(ui32 x = 0; x < CHUNK_SIDE; x++){
                delete[] blocks[y][x];
            }
            delete[] blocks[y];
        }
        delete[] blocks;
    }
    Block*** Data() {
        return blocks;
    }
    i32 X() { 
        return XPOS;
    }
    i32 Z() { 
        return ZPOS;
    }
private:
    i32 XPOS, ZPOS;
    Block ***blocks;
public:
    i32 heightmap[CHUNK_SIDE][CHUNK_SIDE];
    i32 XOFF, ZOFF;
};
