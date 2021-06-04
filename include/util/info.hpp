#ifndef INFO_HPP
#define INFO_HPP


#include "util/camera.hpp"

namespace Screen {
    int W = 960;
    int H = 540;
}

namespace Cam { 
    bool lbutton_down = false;
    f32 mouse_sensitivity = 0.5f;
    bool firstMouse = true;
    float lastx = Screen::W / 2.0f;
    float lasty = Screen::H / 2.0f;

    Camera instance;
    f32 movement_speed = 8.0f;
}



namespace MC {
    bool toggle_wireframe = false;
    bool shifting = false; 
    bool pressed_cursor = false, CURSOR_ON = false;


    f32 deltaTime = 0.0f;
    f32 lastFrame = 0.0f;

    bool resizing = false;
}



#endif
