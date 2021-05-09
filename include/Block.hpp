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
            (float)SCR_WIDTH/SCR_HEIGHT, 0.1f, 360.0f);
    }
    bool is_solid() const { return solid; } 
    void init(const ui32& x, const ui32& y, const ui32& z, const bool& solid ){
        this->x = x + 1;
        this->y = y;
        this->z = z + 1;
        this->solid =solid;
        model = glm::translate(model, glm::vec3(x, y, z));
    }
};
