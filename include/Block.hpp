#include "glutil.h"

class Block {
public:
    glm::mat4 model;
private:
    f32 x, y, z;
    bool solid;
public:
    Block() :
        model(glm::mat4(1.0f)) {}
    bool is_solid() const { return solid; } 
    void init(const ui32& y, const ui32& x, const ui32& z, const bool& solid ){
        this->x = x;
        this->y = y;
        this->z = z;
        this->solid =solid;
        model = glm::translate(model, glm::vec3(x, y, z));
    }
    f32 X() { return this->x; }
    f32 Y() { return this->y; }
    f32 Z() { return this->z; }
};
