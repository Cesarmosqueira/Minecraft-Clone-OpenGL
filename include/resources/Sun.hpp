#ifndef SUN_HPP
#define SUN_HPP

#include "resources/Block.hpp"
class Sun : public Block {

public:
    Sun(const glm::vec3& pp) : Block() {
        this->init(pp[1], pp[0], pp[2]+10, true, 'D');
    }
    
    void rotation (){
        this->model = glm::mat4(1.0f);
        this->model = glm::scale(this->model, {30,30,30});
        this->model = glm::translate(this->model, {this->X(), this->Y(), this->Z()});
    }
};

#endif
