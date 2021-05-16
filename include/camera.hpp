#ifndef __CAM_H__
#define __CAM_H__

#include "glutil.h"

enum CameraMovement {
	FORWARD, LEFT, BACKWARD, RIGHT, UP, DOWN
};
namespace C{
struct Info {
    glm::vec3 pos;
	glm::vec3 lookat;
	glm::vec3 up;
};
}

class Cam {
	glm::vec3 pos;
	glm::vec3 lookat;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 worldUp;
	f32       yaw;
	f32       pitch;
	f32       zoom;
	f32       mouseSensitivity;
	f32       speed;

public:
	Cam(f32 px    =   0.0f, f32 py    =   0.0f, f32 pz    =   0.0f,
			f32 fx    =   0.0f, f32 fy    =   0.0f, f32 fz    =  -1.0f,
			f32 ux    =   0.0f, f32 uy    =   1.0f, f32 uz    =   0.0f,
			f32 yaw   = -90.0f, f32 pitch =   0.0f, f32 zoom  =  45.0f)
			:	pos({px, py, pz}), lookat({fx, fy, fz}), up({ux, uy, uz}),
				worldUp({ux, uy, uz}), yaw(yaw), pitch(pitch), zoom(zoom),
				mouseSensitivity(0.1f), speed(2.5f) {
		updateVectors();
	}
    void set_speed(const f32& speed) { this->speed = speed; }
	glm::mat4 getViewM4() {
		return glm::lookAt(pos, pos + lookat, up);
	}
	f32 getZoom() {
		return glm::radians(zoom);
	}
    f32& getSpeed() {
        return this->speed;
    }
    C::Info get_info(){return {pos, lookat, up};}
	void processKeyboard(CameraMovement direction, f32 deltaTime) {
		f32 velocity = speed * deltaTime * 1.2;
		if (direction == FORWARD) {
			pos += lookat * velocity;
		} else if (direction == BACKWARD) {
			pos -= lookat * velocity;
		} else if (direction == LEFT) {
			pos -= right * velocity;
		} else if (direction == RIGHT) {
			pos += right * velocity;
		} else if (direction == UP) {
            pos[1] += velocity;
        } else if (direction == DOWN) {
            pos[1] -= velocity;

        }
	}
	void processMouse(f32 xoffset, f32 yoffset, bool constrainPitch = true) {
		yaw   = xoffset * mouseSensitivity;
		pitch = yoffset * -mouseSensitivity;
		if (constrainPitch) {
			if (pitch > 89.0f) {
				pitch = 89.0f;
			} else if (pitch < -89.0f) {
				pitch = -89.0f;
			}
		}
		updateVectors();
	}
	void processScroll(f32 yoffset) {
		zoom -= (f32)yoffset;
		if (zoom < 1.0f) {
			zoom = 1.0f;
		} else if (zoom > 45.0f) {
			zoom = 45.0f;
		}
	}

    glm::vec3& get_position() {
        return pos;
    }
private:
	void updateVectors() {
		glm::vec3 temp;

		temp.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		temp.y = sin(glm::radians(pitch));
		temp.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		lookat = glm::normalize(temp);
		right  = glm::normalize(glm::cross(lookat, worldUp));
		up     = glm::normalize(glm::cross(right, lookat));
	}
};

#endif
