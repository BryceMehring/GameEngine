#ifndef _CAMERA_
#define _CAMERA_

#include <d3dx9math.h>

class Camera
{
public:

	Camera();

	const D3DXMATRIX& view() const;
	const D3DXMATRIX& proj() const;
	const D3DXMATRIX& viewProj() const;

	const D3DXVECTOR3& right() const;
	const D3DXVECTOR3& up() const;
	const D3DXVECTOR3& look() const;

	D3DXVECTOR3& pos();

	void lookAt(D3DXVECTOR3& pos, D3DXVECTOR3& target, D3DXVECTOR3& up);

	void setLens(float fov, float aspect, float nearZ, float farZ);

	void setDir(const D3DXVECTOR3& dir);

	void update(float dt);

protected:

	void buildView();

protected:

	D3DXMATRIX m_View;
	D3DXMATRIX m_Proj;
	D3DXMATRIX m_ViewProj;

	D3DXVECTOR3 m_PosW;
	D3DXVECTOR3 m_RightW;
	D3DXVECTOR3 m_UpW;
	D3DXVECTOR3 m_LookW;

	D3DXVECTOR3 m_dir;

};

#endif // _CAMERA_