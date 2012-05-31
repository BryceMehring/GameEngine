#include "Pong.h"
#include "Game.h"
#include <ctime>
#include <sstream>

using namespace std;

void GetWindowRect(RECT& R)
{
	GetClientRect(GetActiveWindow(),&R);
}

// Ball class

const D3DXVECTOR2 normal[4] = 
{
	D3DXVECTOR2(1.0f,0.0f),
	D3DXVECTOR2(-1.0f,0.0f),
	D3DXVECTOR2(0.0f,1.0f),
	D3DXVECTOR2(0.0f,-1.0f)
};

Ball::Ball(const D3DXVECTOR2& pos, const D3DXVECTOR2& dir, float V, float R) : m_pos(pos), m_dir(dir), m_fSpeed(V), m_fR(R)
{
	SetCollisionPolygon(new CCircle(Circle(pos,R)));
}

void Ball::Update(double dt)
{
	RECT R;
	GetWindowRect(R);

	unsigned int index = -1;
	if(m_pos.x <= 0)
	{
		//m_pos.x = 800.0f;
		index = 0;
	}
	else if(m_pos.x >= R.right)
	{
		//m_pos.x = 0.0f;
		index = 1;
	}

	if(m_pos.y <= 0.0f)
	{
		//m_pos.y = 600.0f;
		index = 2;
	}
	else if(m_pos.y >= R.bottom)
	{
		//m_pos.y = 0.0f;
		index = 3;
		//G = D3DXVECTOR2(0.0f,-1.2f);
	}

	if(index != -1)
	{
		m_dir = Reflect(-m_dir,normal[index]);
	}

	if(HasNode())
	{
		for(unsigned int i = 0; i < m_nodes.size(); ++i)
		{
			Node* pNode = m_nodes[i];

			const Node::LIST_DTYPE& nearObj = *(pNode->GetNearObjects());
			for(auto iter = nearObj.begin(); iter != nearObj.end(); ++iter)
			{
				if(*iter != this)
				{
					if(m_pCollisionPolygon->Intersects((*iter)->GetCollisionPolygon()))
					{
						D3DXVECTOR2 pos = D3DXVECTOR2((*iter)->GetPos().x,(*iter)->GetPos().y);
						D3DXVECTOR2 normal = pos - m_pos;
								
						D3DXVec2Normalize(&normal,&normal);

						m_dir = Reflect(-m_dir,normal);
						break;
					}
				}
			}
		}
	}

	m_pos += m_dir*m_fSpeed*dt;
	static_cast<CCircle*>(m_pCollisionPolygon)->GetCircle().center = m_pos;
	//static_cast<CCircle*>(m_pCollisionPolygon)->Update(m_pos,m_fR);
}

void Ball::Render(IRenderer* pRenderer)
{
	::D3DXMATRIX S, T;
	::D3DXMatrixTranslation(&T,m_pos.x,m_pos.y,0.0f);
	::D3DXMatrixScaling(&S,m_fR / 16.0f,m_fR/16.0f,1.0f);

	pRenderer->DrawSprite(S*T,"Point",0);
}

// paddle class
Paddle::Paddle(const D3DXVECTOR2& pos) : m_pos(pos), m_iScore(0)
{
	D3DXVECTOR2 topLeft(0.0f,0.0f);
	D3DXVECTOR2 bottomRight(0.0f,0.0f);
	GetRect(topLeft,bottomRight);
	SetCollisionPolygon(new CRectangle(FRECT(topLeft,bottomRight)));
}

void Paddle::GetRect(D3DXVECTOR2& topLeft, D3DXVECTOR2& bottomRight)
{
	topLeft = D3DXVECTOR2(m_pos.x - 16.0f,m_pos.y + 64.0f);
	bottomRight = D3DXVECTOR2(m_pos.x + 16.0f,m_pos.y - 64.0f);
}

void Paddle::Render(IRenderer* pRenderer)
{
	::D3DXMATRIX T;
	::D3DXMatrixTranslation(&T,m_pos.x,m_pos.y,0.0f);

	pRenderer->DrawSprite(T,"paddle",2);
}

PlayerPaddle::PlayerPaddle(const D3DXVECTOR2& pos) : Paddle(pos) {}

void PlayerPaddle::Update(IKMInput* pInput, double dt)
{
	RECT R;
	GetWindowRect(R);

	m_pos.y = pInput->MousePos().y;

	/*if(pInput->KeyDown(UP))
	{
		m_pos.y -= 500.0f * dt;
	}
	else if(pInput->KeyDown(DOWN))
	{
		m_pos.y += 500.0f * dt;
	}
	else
	{
		m_pos.y += -500.0f * pInput->MouseY() * dt;
	}*/

	if(m_pos.y < R.top)
	{
		m_pos.y = R.top;
	}
	else if(m_pos.y > R.bottom)
	{
		m_pos.y = R.bottom;
	}

	static_cast<CRectangle*>(m_pCollisionPolygon)->GetRect().bottomRight.y = m_pos.y - 64.0f;
	static_cast<CRectangle*>(m_pCollisionPolygon)->GetRect().topLeft.y = m_pos.y + 64.0f;
}

void ComputerPaddle::Update(IKMInput* pInput, double dt)
{

}



// Pong state

RTTI_IMPL(Pong);

Pong::Pong() : m_iLeftScore(0), m_iRightScore(0)
{
}

Pong::~Pong()
{
	delete m_pQuadTree;
}

void Pong::Init(Game* pGame)
{
	const RECT& R = pGame->GetWindow()->GetRECT();
	FRECT fRect(D3DXVECTOR2(R.left,R.top),D3DXVECTOR2(R.right,R.bottom));
	
	m_pQuadTree = new QuadTree(fRect);


	//m_LeftPaddles.push_back(new ComputerPaddle());

	PlayerPaddle* pPaddle = new PlayerPaddle(D3DXVECTOR2(R.right - 40.0f,(R.bottom + R.top) / 2.0f));
	m_RightPaddles.push_back(pPaddle);

	m_pQuadTree->Insert(pPaddle);

	//Ball* pBall = new Ball(::D3DXVECTOR2(R.left / 2,R.right / 2),D3DXVECTOR2(1.0f,0.0f),100.0f,1.0f);
	//m_balls.push_back(pBall);
	//m_pQuadTree->Insert(pBall);
}

void Pong::Destroy(Game* pGame)
{

}

void Pong::Update(Game* pGame)
{
	UpdatePaddles(pGame);
	UpdateBalls(pGame);

	m_pQuadTree->Update();
}

void Pong::UpdateBalls(Game* pGame)
{
	const RECT& R = pGame->GetWindow()->GetRECT();

	// Update balls
	auto iter = m_balls.begin();
	while(iter != m_balls.end())
	{
		Ball* pBall = *iter;

		pBall->Update(pGame->GetDt());

		if(pBall->GetPos().x < (float)R.left || pBall->GetPos().x > (float)R.right)
		{
			if(pBall->GetPos().x < (float)R.left)
			{
				m_iRightScore++;
			}
			else if(pBall->GetPos().x > (float)R.right)
			{
				m_iLeftScore++;
			}

			delete pBall;
			iter = m_balls.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

void Pong::UpdatePaddles(Game* pGame)
{
	IKMInput* pInput = pGame->GetInput();
	double dt = pGame->GetDt();

	for_each(m_LeftPaddles.begin(),m_LeftPaddles.end(),[&](Paddle* pPaddle)
	{
		pPaddle->Update(pInput,dt);
	});

	for_each(m_RightPaddles.begin(),m_RightPaddles.end(),[&](Paddle* pPaddle)
	{
		pPaddle->Update(pInput,dt);
	});
}

void Pong::IncreasePaddlePoints(const std::vector<Paddle*>& ref)
{
	for_each(ref.begin(),ref.end(),[](Paddle* pBall)
	{
		pBall->IncreaseScore();
	});
}

void Pong::Draw(Game* pGame)
{
	IRenderer* pRenderer = pGame->GetRenderer();

	for_each(m_LeftPaddles.begin(),m_LeftPaddles.end(),[&](Paddle* pPaddle)
	{
		pPaddle->Render(pRenderer);
	});

	for_each(m_RightPaddles.begin(),m_RightPaddles.end(),[&](Paddle* pPaddle)
	{
		pPaddle->Render(pRenderer);
	});

	for_each(m_balls.begin(),m_balls.end(),[&](Ball* pBall)
	{
		pBall->Render(pRenderer);
	});

	DrawScore(pGame);

	m_pQuadTree->Render(pRenderer);

}

void Pong::DrawScore(Game* pGame)
{
	IRenderer* pRenderer = pGame->GetRenderer();
	RECT R = pGame->GetWindow()->GetRECT();

	const POINT POS[2] = 
	{
		{R.left + 200,R.top + 50},
		{R.right - 200,R.top + 50}
	};

	std::stringstream stream;
	stream << m_iLeftScore << " " << m_iRightScore;

	unsigned int i = 0;
	while(stream)
	{
		std::string str;
		stream >> str;

		pRenderer->DrawString(str.c_str(),POS[i++],0xffffffff);
	}
}