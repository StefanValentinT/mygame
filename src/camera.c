#ifndef CAMERA_H
#define CAMERA_H

#include <cglm/cglm.h>
#include <stdbool.h>
#include <math.h>

typedef enum {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN
} Camera_Movement;

typedef struct {
	vec3 pos;
	vec3 front;  // auto
	vec3 up;     // auto
	vec3 right;  // auto
	vec3 worldUp;

	float yaw;
	float pitch;

	float movementSpeed;
	float mouseSensitivity;
	float zoom;
} Camera;

Camera cameraInit(vec3 position, vec3 up, float yaw, float pitch);
Camera cameraInitScalar(
	float posX,float posY, float posZ,
	float upX, float upY, float upZ,
	float yaw, float pitch);
Camera cameraDefault(void);
void cameraGetViewMatrix(Camera* cam, mat4 mat);
void cameraProcessKeyboard(Camera* cam, Camera_Movement direction, float deltaTime);
void cameraProcessMouseMovement(Camera* cam, float xoffset, float yoffset, bool constrainPitch);
void cameraProcessMouseMovementConstrained(Camera* cam, float xoffset, float yoffset);
void cameraUpdateVectors(Camera* cam);

#endif
#if __INCLUDE_LEVEL__ == 0

static const float YAW         = -90.0f;
static const float PITCH       =  0.0f;
static const float SPEED       =  2.5f;
static const float SENSITIVITY =  0.1f;
static const float ZOOM        =  45.0f;

Camera cameraInit(vec3 position, vec3 up, float yaw, float pitch) {
	Camera cam;

	glm_vec3_copy((vec3){0.0f, 0.0f, -1.0f}, cam.front);
	cam.movementSpeed    = SPEED;
	cam.mouseSensitivity = SENSITIVITY;
	cam.zoom             = ZOOM;

	glm_vec3_copy(position, cam.pos);
	glm_vec3_copy(up, cam.worldUp);
	cam.yaw   = yaw;
	cam.pitch = pitch;

	cameraUpdateVectors(&cam);
	return cam;
}


Camera cameraInitScalar(
	float posX,float posY, float posZ,
	float upX, float upY, float upZ,
	float yaw, float pitch) {
	return cameraInit((vec3){posX, posY, posZ}, (vec3){upX, upY, upZ}, yaw, pitch);
}

Camera cameraDefault(void) {
	vec3 defaultPos = {0.0f, 0.0f, 3.0f};
	vec3 defaultUp = {0.0f, 1.0f, 0.0f};
	return cameraInit(defaultPos, defaultUp, YAW, PITCH);
}

void cameraGetViewMatrix(Camera* cam, mat4 mat){
	vec3 target;
	glm_vec3_add(cam->pos, cam->front, target);
	glm_lookat(cam->pos, target, cam->up, mat);
}

void cameraProcessKeyboard(Camera* cam, Camera_Movement direction, float deltaTime) {
	float velocity = cam->movementSpeed * deltaTime;
	vec3 scaledMove;

	vec3 horizontalFront;
	horizontalFront[0] = cos(glm_rad(cam->yaw));
	horizontalFront[1] = 0.0f;
	horizontalFront[2] = sin(glm_rad(cam->yaw));
	glm_vec3_norm(horizontalFront);

	vec3 horizontalRight;
	horizontalRight[0] = -sin(glm_rad(cam->yaw));
	horizontalRight[1] = 0.0f;
	horizontalRight[2] = cos(glm_rad(cam->yaw));
	glm_vec3_norm(horizontalRight);

	if (direction == FORWARD) {
		glm_vec3_scale(horizontalFront, velocity, scaledMove);
		glm_vec3_add(cam->pos, scaledMove, cam->pos);
	}
	if (direction == BACKWARD) {
		glm_vec3_scale(horizontalFront, velocity, scaledMove);
		glm_vec3_sub(cam->pos, scaledMove, cam->pos);
	}
	if (direction == LEFT) {
		glm_vec3_scale(horizontalRight, velocity, scaledMove);
		glm_vec3_sub(cam->pos, scaledMove, cam->pos);
	}
	if (direction == RIGHT) {
		glm_vec3_scale(horizontalRight, velocity, scaledMove);
		glm_vec3_add(cam->pos, scaledMove, cam->pos);
	}
	if (direction == UP) {
		glm_vec3_scale(cam->worldUp, velocity, scaledMove);
		glm_vec3_add(cam->pos, scaledMove, cam->pos);
	}
	if (direction == DOWN) {
		glm_vec3_scale(cam->worldUp, velocity, scaledMove);
		glm_vec3_sub(cam->pos, scaledMove, cam->pos);
	}
}

void cameraProcessMouseMovement(Camera* cam, float xoffset, float yoffset, bool constrainPitch) {
	xoffset *= cam->mouseSensitivity;
	yoffset *= cam->mouseSensitivity;

	cam->yaw += xoffset;
	cam->pitch += yoffset;

	if (constrainPitch){
		if (cam->pitch > 89.0f)
			cam->pitch = 89.0f;
		if (cam->pitch < -89.0f)
			cam->pitch = -89.0f;
	}
	cameraUpdateVectors(cam);
}

void cameraProcessMouseMovementConstrained(Camera* cam, float xoffset, float yoffset){
	cameraProcessMouseMovement(cam, xoffset, yoffset, true);
}

void cameraUpdateVectors(Camera* cam){
	cam->front[0] = cos(glm_rad(cam->yaw)) * cos(glm_rad(cam->pitch));
	cam->front[1] = sin(glm_rad(cam->pitch));
	cam->front[2] = sin(glm_rad(cam->yaw)) * cos(glm_rad(cam->pitch));
	glm_vec3_norm(cam->front);

	glm_vec3_cross(cam->front, cam->worldUp, cam->right);
	glm_vec3_norm(cam->right);
	
	glm_vec3_cross(cam->right, cam->front, cam->up);
	glm_vec3_norm(cam->up);
}

#endif

