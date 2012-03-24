#ifndef _CAMERA_
#define _CAMERA_

#include "RefCounting.h"
#include "IKMInput.h"
#include "asVM.h"

class Camera : public RefCounting
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

	void setSpeed(float s);

	void update(IKMInput& input, float dt);

	void RegisterScript(asVM* pVM);

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

	float m_Speed;

};

#endif // _CAMERA_