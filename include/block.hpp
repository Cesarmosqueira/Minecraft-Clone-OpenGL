#include "glutil.h"
const ui32 FSIZE = sizeof(float);
const f32  ASPECT = 16.0f / 9.0f;

namespace K {
    struct Memory {
        ui32 vbo, vao, ebo;
        ui32 ind;
    };
    template<typename T> 
    unsigned int len(T* array){
        return sizeof(array)/sizeof(array[0]);
    }
    const f32 vertices[] = {
        // posiciones        colores          texturas
        -0.5f,  0.5f,  0.5f,   1.0f, 0.0, 0.0,   0.0f, 1.0f,  // 0
        0.5f,  0.5f,  0.5f,   1.0f, 0.0, 0.0,   1.0f, 1.0f,  // 1
        -0.5f, -0.5f,  0.5f,   1.0f, 0.0, 0.0,   0.0f, 0.0f,  // 2
        0.5f, -0.5f,  0.5f,   1.0f, 0.0, 0.0,   1.0f, 0.0f,  // 3
        -0.5f,  0.5f, -0.5f,   1.0f, 0.0, 0.0,   1.0f, 1.0f,  // 4
        0.5f,  0.5f, -0.5f,   1.0f, 0.0, 0.0,   0.0f, 1.0f,  // 5
        -0.5f, -0.5f, -0.5f,   1.0f, 0.0, 0.0,   1.0f, 0.0f,  // 6
        0.5f, -0.5f, -0.5f,   1.0f, 0.0, 0.0,   0.0f, 0.0f,  /* 7*/};

    const ui32 UP[] = {0, 1, 4, 1, 4, 5};
    const ui32 DOWN[] = {3, 6, 7, 2, 3, 6,};
    const ui32 NORTH[] = {4, 5, 6, 5, 6, 7};
    const ui32 SOUTH[] = {0, 1, 2, 1, 2, 3,};
    const ui32 EAST[] = {1, 5, 3, 3, 5, 7 };
    const ui32 WEST[] = {0, 4, 6, 0, 2, 6};
}
void freeMemspace(K::Memory* self) {
    glDeleteVertexArrays(1, &self->vao);
    glDeleteBuffers(1, &self->vbo);
    glDeleteBuffers(1, &self->ebo);

}
void InitBlockFace(K::Memory* self, const ui32* face, const ui32& ind){
    self->ind = ind;
    glGenVertexArrays(1, &self->vao);
	glGenBuffers(1, &self->vbo);
	glGenBuffers(1, &self->ebo);

	glBindVertexArray(self->vao);

	glBindBuffer(GL_ARRAY_BUFFER, self->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(K::vertices), K::vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, self->ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(face), face, GL_STATIC_DRAW);

    // posiciones
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*FSIZE, (void*)0);
    glEnableVertexAttribArray(self->ind);
    // colores
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*FSIZE, (void*)(3*FSIZE));
    glEnableVertexAttribArray(self->ind + 1);
    // coordenadas de textura
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*FSIZE, (void*)(6*FSIZE));
    glEnableVertexAttribArray(self->ind + 2);

    glBindVertexArray(0);
} 
struct BlockUtil {
    K::Memory UP;
    K::Memory DOWN;
    K::Memory NORTH;
    K::Memory SOUTH;
    K::Memory EAST;
    K::Memory WEST;
    BlockUtil() { 
        InitBlockFace(&UP, K::UP, 2);
        InitBlockFace(&DOWN, K::DOWN, 5);
        InitBlockFace(&NORTH, K::NORTH, 6);
        InitBlockFace(&SOUTH, K::SOUTH, 1);
        InitBlockFace(&WEST, K::WEST, 3);
        InitBlockFace(&EAST, K::EAST, 4);
    }
    ~BlockUtil() {
        freeMemspace(&UP);
        freeMemspace(&DOWN);
        freeMemspace(&NORTH);
        freeMemspace(&SOUTH);
        freeMemspace(&EAST);
        freeMemspace(&WEST);
    }
};