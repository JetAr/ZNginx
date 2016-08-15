
void GutUserControl_RotateObject(Matrix4x4 &object_matrix, float time_advance)
{
	// 讀取滑鼠
	GutMouseInfo mouse;
	if ( GutReadMouse(&mouse) )
	{
		float rotation_speed = 1.0 * time_advance;

		// 如果按下滑鼠左鍵，就旋轉鏡頭
		if ( mouse.button[0] ) 
		{
			Matrix4x4 rotate_matrix;

			rotate_matrix.RotateY_Replace(mouse.x * rotation_speed);
			rotate_matrix.RotateX(mouse.y * rotation_speed);

			object_matrix = object_matrix * rotate_matrix;
		}

		// 用滾輪來縮放物件
		if ( mouse.z )
		{
			float scale = 1.0f + mouse.z / 1000.0f;
			object_matrix.Scale(scale, scale, scale);
		}
	}
}

void GetUserControl_FPSCamera(Vector4 &vEye, Vector4 &vUp, Vector4 &vLookAt, float time_advance)
{
	// 讀取滑鼠
	GutMouseInfo mouse;
	GutReadMouse(&mouse);
	// 讀取鍵盤
	char keyboard_state[256];
	GutReadKeyboard(keyboard_state);

	float moving_speed = 2.0f * time_advance;
	float rotation_speed = 1.0f * time_advance;

	// 極座標系統
	static float theta = -MATH_PI * 0.5f;
	static float phi = 0.0f;

	// 如果按下滑鼠左鍵，就旋轉鏡頭
	if ( mouse.button[0] ) 
	{
		theta += mouse.x * rotation_speed;
		phi -= mouse.y * rotation_speed;
	}

	float sin_phi, cos_phi;
	float sin_theta, cos_theta;

	FastMath::SinCos(phi, sin_phi, cos_phi);
	FastMath::SinCos(theta, sin_theta, cos_theta);

	// 計算鏡頭的面向
	Vector4 camera_facing;

	camera_facing[0] = cos_phi * cos_theta;
	camera_facing[1] = sin_phi;
	camera_facing[2] = cos_phi * sin_theta;

	// 計算鏡頭正上方的軸向
	Vector4 camera_up;
	FastMath::SinCos(phi + MATH_PI*0.5f, sin_phi, cos_phi);

	camera_up[0] = cos_phi * cos_theta;
	camera_up[1] = sin_phi;
	camera_up[2] = cos_phi * sin_theta;

	// 取得鏡面右方的方向
	Vector4 camera_right = Vector3CrossProduct(camera_up, camera_facing);

	// 按下W或方向鍵向上
	if ( keyboard_state[GUTKEY_W] || keyboard_state[GUTKEY_UP] )
	{
		vEye += camera_facing * moving_speed;
	}
	// 按下S或方向鍵向下
	if ( keyboard_state[GUTKEY_S] || keyboard_state[GUTKEY_DOWN] )
	{
		vEye -= camera_facing * moving_speed;
	}
	// 按下A或方向鍵向左
	if ( keyboard_state[GUTKEY_A] || keyboard_state[GUTKEY_LEFT] )
	{
		vEye += camera_right * moving_speed;
	}
	// 按下D或方向鍵向右
	if ( keyboard_state[GUTKEY_D] || keyboard_state[GUTKEY_RIGHT] )
	{
		vEye -= camera_right * moving_speed;
	}

	// 計算出鏡頭對準的點, 產生鏡頭轉換矩陣時會用到.
	vLookat = vEye + camera_facing;
	// 因為是對2個軸轉動, 需要更新鏡頭朝上的軸
	vUp = camera_up;

	vEye[3] = vUp[3] = vLookAt[3] = 1.0f;
}
