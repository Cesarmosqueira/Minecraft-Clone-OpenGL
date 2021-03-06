#include "util/glutil.h"

namespace K {
    const f32 vertices[] = {
        // posiciones             texturas        normals
        -0.5f,  0.5f,  0.5f,    0.0f, 1.0f,  0.0f, 1.0f, 0.0f, //   0
         0.5f,  0.5f,  0.5f,    1.0f, 1.0f,  0.0f, 1.0f, 0.0f, //   1
        -0.5f, -0.5f,  0.5f,    0.0f, 0.0f,  0.0f, 1.0f, 0.0f, //   2
         0.5f, -0.5f,  0.5f,    1.0f, 0.0f,  0.0f, 1.0f, 0.0f, //   3
        -0.5f,  0.5f, -0.5f,    1.0f, 1.0f,  0.0f, 0.0f, 1.0f, //   4
         0.5f,  0.5f, -0.5f,    0.0f, 1.0f,  0.0f, 0.0f, 1.0f, //   5
        -0.5f, -0.5f, -0.5f,    1.0f, 0.0f,  0.0f, 0.0f, 1.0f, //   6
         0.5f, -0.5f, -0.5f,    0.0f, 0.0f,  0.0f, 0.0f, 1.0f, //   7
                                                               
        -0.5f,  0.5f,  0.5f,    0.0f, 0.0f,  1.0f, 0.0f, 0.0f, //   8
         0.5f,  0.5f,  0.5f,    1.0f, 0.0f,  1.0f, 0.0f, 0.0f, //   9
        -0.5f, -0.5f,  0.5f,    0.0f, 0.0f,  1.0f, 0.0f, 0.0f, //   10
         0.5f, -0.5f,  0.5f,    1.0f, 0.0f,  1.0f, 0.0f, 0.0f, //   11
        -0.5f,  0.5f, -0.5f,    0.0f, 1.0f,  0.0f,-1.0f, 0.0f, //   12
         0.5f,  0.5f, -0.5f,    1.0f, 1.0f,  0.0f,-1.0f, 0.0f, //   13
        -0.5f, -0.5f, -0.5f,    0.0f, 1.0f,  0.0f,-1.0f, 0.0f, //   14
         0.5f, -0.5f, -0.5f,    1.0f, 1.0f,  0.0f,-1.0f, 0.0f };// 15

    const ui32 EAST[6] = {  4,  5,  6,  5,  6,  7 };
    const ui32 UP[6]   = {  8,  9, 12,  9, 12, 13 };
    const ui32 NORTH[6]= {  1,  5,  3,  3,  5,  7 };
    const ui32 SOUTH[6]= {  0,  4,  6,  0,  2,  6 };
    const ui32 DOWN[6] = { 11, 14, 15, 10, 11, 14 };
    const ui32 WEST[6] = {  0,  1,  2,  1,  2,  3 };

}
