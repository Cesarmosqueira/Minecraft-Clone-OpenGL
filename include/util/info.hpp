#ifndef INFO_HPP
#define INFO_HPP


#include "pncraft.hpp"
#include "util/camera.hpp"

namespace Screen {
    int WIDTH = 960;
    int HEIGHT = 540;
}

namespace Cam { 
    bool lbutton_down = false;
    f32 mouse_sensitivity = 0.5f;
    bool firstMouse = true;
    float lastx = Screen::WIDTH / 2.0f;
    float lasty = Screen::HEIGHT / 2.0f;

    Camera instance;
}

f32 movement_speed = 8.0f;

bool toggle_wireframe = false;
bool shifting = false; 


f32 deltaTime = 0.0f;
f32 lastFrame = 0.0f;

bool pressed_cursor = false, CURSOR_ON = false;




#endif
