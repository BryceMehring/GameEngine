#include "FlockingAlgorithm.h"
#include "Game.h"
#include <glm/gtx/transform.hpp>


RTTI_IMPL(FlockingAlgorithm);

void ClampWrap(glm::vec3& pos)
{
    if(pos.x < -100.0f)
	{
        pos.x = 100.0f;
	}
    else if(pos.x > 100.0f)
	{
        pos.x = -100.0f;
	}

    if(pos.y < -100.0f)
	{
        pos.y = 100.0f;
	}
    else if(pos.y > 100.0f)
	{
        pos.y = -100.0f;
	}
}

extern "C" PLUGINDECL IPlugin* CreatePlugin(asIScriptEngine* p)
{
    p->Release(); // todo: remove this
	return new FlockingAlgorithm();
}

Bird::Bird()
{
    m_pos = glm::vec3(Math::GetRandFloat(-100.0f,100.0f),Math::GetRandFloat(-100.0f,100.0f),0.0f);
	m_fAngle = Math::GetRandFloat(0.0f,6.2832f);

	m_color = glm::vec4(Math::GetRandFloat(0.0f,1.0f),Math::GetRandFloat(0.0f,1.0f),Math::GetRandFloat(0.0f,1.0f),1.0f);

	m_CollisionCircle.GetCircle().center = ::glm::vec2(m_pos.x,m_pos.y);
    m_CollisionCircle.GetCircle().r = 6.0f;
}

Bird::Bird(const glm::vec2& pos)
{
	m_pos = glm::vec3(pos.x,pos.y,0.0f);
	m_fAngle = Math::GetRandFloat(0.0f,6.2832f);

	m_color = glm::vec4(Math::GetRandFloat(0.0f,1.0f),Math::GetRandFloat(0.0f,1.0f),Math::GetRandFloat(0.0f,1.0f),1.0f);

	m_CollisionCircle.GetCircle().center = ::glm::vec2(m_pos.x,m_pos.y);
    m_CollisionCircle.GetCircle().r = 6.0f;
}

void Bird::Update(QuadTree& tree, const glm::vec2& avgPos, double dt)
{
	// time complexity of QueryNearObjects
	// Log4(N)

	std::vector<ISpatialObject*> nearBirds;
	tree.QueryNearObjects(GetCollisionPolygon(),nearBirds,this);

    glm::vec3 localDir(0.0f,0.0f,0.0f);

    glm::vec3 dir(0.0f,0.0f,0.0f);
    float fPriorityToGroup = 4.0f;
	// This operation should operate most of the time in constant time because the number of elements that need to be averaged is const
	if(!nearBirds.empty())
	{
		glm::vec2 avgCurrentDir(0.0f,0.0f);
		for(unsigned int i = 0; i < nearBirds.size(); ++i)
		{
            avgCurrentDir += nearBirds[i]->GetDir();

            glm::vec3 localDir = m_pos - glm::vec3(nearBirds[i]->GetPos(),0.0f);
            float fLocalLength = glm::length(localDir);

            if(fLocalLength < 2.0f)
            {
                dir += glm::vec3(6.0f)*(localDir / fLocalLength);
            }
		}

        localDir = (glm::vec3(avgCurrentDir.x,avgCurrentDir.y,0.0f) - glm::vec3(GetDir(),0.0f)) / glm::vec3(2);

        fPriorityToGroup = 0.0001f;
	}


    glm::vec3 toCenter = glm::vec3(avgPos,0.0f) - m_pos;

	// Calculate the avg between the two avg directions
    dir += (glm::vec3(5.0f)*localDir +
            glm::vec3(2.0f)*glm::vec3(GetDir(),0.0f) +
            glm::vec3(fPriorityToGroup)*toCenter);


	//dir.x += ::Math::GetRandFloat(0.0f,100.0f);
	//dir.y += ::Math::GetRandFloat(0.0f,100.0f);
	dir = glm::normalize(dir);
    m_pos += dir * glm::vec3(20.0f) * glm::vec3(dt);

	float angleDiff = atan2(-dir.x,dir.y) - m_fAngle;
	m_fAngle += angleDiff * 5.0f * dt; // this is the problem

	tree.Erase(*this);
	m_CollisionCircle.GetCircle().center = glm::vec2(m_pos.x,m_pos.y);
	tree.Insert(*this);

	ClampWrap(m_pos);
	/*D3DXVECTOR3 dir(-sin(m_fAngle),cos(m_fAngle),0.0f);

	m_pos += dir * 10.0f * dt;

	D3DXMATRIX R, T;
	D3DXMatrixTranslation(&T,m_pos.x,m_pos.y,m_pos.z);
	D3DXMatrixRotationZ(&R,m_fAngle);

	m_Transformation = R * T;

	tree.Erase(*this);
	m_CollisionCircle.GetCircle().center = ::D3DXVECTOR2(m_pos.x,m_pos.y);
	tree.Insert(*this);

	ClampWrap(m_pos);*/
}

void Bird::Render(IRenderer& renderer)
{
	static const glm::vec3 vertices[] = 
	{
        glm::vec3(0.0f,0.0f,0.0f),
        glm::vec3(1.0f,3.0f,0.0f),
        glm::vec3(2.0f,0.0f,0.0f)
	};

	glm::mat4 T = glm::translate(m_pos);
	T = glm::rotate(T,(57.2957f)*m_fAngle,0.0f,0.0f,1.0f);

	renderer.DrawLine(vertices,3,m_color,T);
}

void FlockingAlgorithm::Init(Game& game)
{
    m_pQuadtree = new QuadTree(Math::FRECT(glm::vec2(-100.0f,100.0f),glm::vec2(100.0f,-100.0f)));

    m_birds.resize(400);
	for(unsigned int i = 0; i < m_birds.size(); ++i)
	{
		m_birds[i] = new Bird();

		m_pQuadtree->Insert(*m_birds[i]);
	}

	m_fFlockAngle = Math::GetRandFloat(0.0f,6.2832f);

}
void FlockingAlgorithm::Destroy(Game& game)
{
	ClearBirds();

	delete m_pQuadtree;
}

void FlockingAlgorithm::ClearBirds()
{
	for(unsigned int i = 0; i < m_birds.size(); ++i)
	{
		m_pQuadtree->Erase(*m_birds[i]);
		delete m_birds[i];
	}

	m_birds.clear();
}
void FlockingAlgorithm::Update(Game& game)
{
	glm::vec2 avgPos(0.0f,0.0f);

    if(game.GetInput().MouseClick(0))
	{
		//m_birds[i] = new Bird();
		Bird* pBird = new Bird(game.GetInput().GetTransformedMousePos());
		m_birds.push_back(pBird);

		m_pQuadtree->Insert(*pBird);
	}

	if(game.GetInput().KeyDown(KeyCode::ENTER))
	{
		ClearBirds();
    }

	// O(n)
	for(unsigned int i = 0; i < m_birds.size(); ++i)
	{
		avgPos += m_birds[i]->GetPos();
	}

	// O(n*log4(n))
	for(unsigned int i = 0; i < m_birds.size(); ++i)
	{
		m_birds[i]->Update(*m_pQuadtree,avgPos,game.GetDt());
	}
}
void FlockingAlgorithm::Draw(Game& game)
{
	//game.GetRenderer().Get2DRenderer().DrawString("Hello World",::D3DXVECTOR2(-10.0f,0.0f));

   /* for(unsigned int i = 0; i < m_birds.size(); ++i)
	{
		m_birds[i]->Render(game.GetRenderer());
    }*/

   // m_pQuadtree->Render(game.GetRenderer());

	//game.GetRenderer().Get2DRenderer().
}
