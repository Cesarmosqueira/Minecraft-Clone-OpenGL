#ifndef SANDBOX_H
#define SANDBOX_H

#include "pncraft.hpp"
#include "camera.hpp"


class Sandbox {
public:
    Sandbox() {

    }

    static void showvec3(const glm::vec3& v) {
        std::cout << "[" << v[0] << "," << v[1] << "," << v[2] << "]";
    }
private:
};


#endif
