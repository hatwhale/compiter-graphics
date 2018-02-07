#pragma once

class CFlyingCamera
{
public:
	glm::vec3 vEye, vView, vUp;
	float fAngleX; //X angle of camera (head turning up and down)
	float fAngleY; //Y angle of camera (head turning left and right)
	float fSpeed;
	float fSensitivity; // How many degrees to rotate per pixel moved by mouse (nice value is 0.1)

	// Main functions
	void RotateWithMouse();
	void Update();
	glm::mat4 Look();

	void SetMovingKeys(int a_iForw, int a_iBack, int a_iLeft, int a_iRight, int a_SpeedUp);
	void ResetMouse();

	// Functions that set viewing angles
	void SetAngleX(), SetAngleY();

	// Constructors
	CFlyingCamera();
	CFlyingCamera(glm::vec3 a_vEye, glm::vec3 a_vView, glm::vec3 a_vUp, float a_fSpeed, float a_fSensitivity);

private:
	POINT pCur; // For mosue rotation
	int iForw, iBack, iLeft, iRight, iSpeedUp;
};

extern CFlyingCamera cCamera;