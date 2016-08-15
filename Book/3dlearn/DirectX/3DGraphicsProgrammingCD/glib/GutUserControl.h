#ifndef _GUT_USERCONTROL_
#define _GUT_USERCONTROL_

#include "Vector4.h"
#include "Matrix4x4.h"

class CGutUserControl
{
public:
	enum UP_AXIS
	{
		UP_X,
		UP_Y,
		UP_Z,
		UP_CUSTOMIZED
	};

	enum CONTROLLER_TYPE
	{
		CONTROLLER_ROTATEOBJECT,
		CONTROLLER_FPSCAMERA
	};

	Matrix4x4 m_ObjectMatrix;
	Matrix4x4 m_ViewMatrix;
	Matrix4x4 m_CameraMatrix;
	Matrix4x4 m_RefCameraMatrix;

	Vector4 m_vEye, m_vUp, m_vLookAt;
	Vector4 m_vCustomized[3];

	UP_AXIS m_eUP;
	CONTROLLER_TYPE m_eControllerType;

	float m_fRotateSpeed;
	float m_fMoveSpeed;
	float m_fCameraHeading, m_fCameraPitch;

public:
	CGutUserControl(void);
	~CGutUserControl(void);

	void Update(float fTimeAdvance, CONTROLLER_TYPE type);
	void RotateObject(float fTimeAdvance);
	void SetObjectPosition(Vector4 &vPosition);

	void FPSCamera(float fTimeAdvance);

	void SetCamera(Vector4 &vEye, float fHeading, float fPitch);
	void SetCamera(Vector4 &vEye, Vector4 &vLookAt, Vector4 &vUp);
	void UpdateViewMatrix(void);

	void SetUpAxis(UP_AXIS axis) { m_eUP = axis; };
	void SetRotateSpeed(float speed) { m_fRotateSpeed = speed; }
	void SetMoveSpeed(float speed) { m_fMoveSpeed = speed; }

	Matrix4x4 &GetViewMatrix(void) { return m_ViewMatrix; }
	Matrix4x4 &GetObjectMatrix(void) { return m_ObjectMatrix; }
	Matrix4x4 &GetCameraMatrix(void) { return m_CameraMatrix; }
	const Vector4   &GetCameraPosition(void) { return m_vEye; }
};

#endif // _GUT_USERCONTROL_