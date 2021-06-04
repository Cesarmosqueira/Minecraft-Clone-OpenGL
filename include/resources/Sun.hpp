#ifndef SUN_HPP
#define SUN_HPP

#include "resources/Block.hpp"
class Sun : public Block {

public:
    Sun(const glm::vec3& pp) : Block() {
        this->init(pp[1], pp[0], pp[2]+10, true, 'D');
        this->model = glm::scale(this->model, {30,30,30});
    }
    

};

#endif
