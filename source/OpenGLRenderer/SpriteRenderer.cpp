#include "SpriteRenderer.h"
#include "ApplyShader.h"
#include "ResourceManager.h"
#include "Mesh.h"

SpriteRenderable::SpriteRenderable(const glm::mat4& T, const glm::vec4& color, const glm::vec2& tiling, unsigned int iCellId) : T(T), color(color), tiling(tiling), iCellId(iCellId)
{
}

void SpriteRenderable::Render(const Mesh& mesh, ApplyShader& shader, const IResource*)
{
	shader->SetColor(color);
	shader->SetValue("transformation",T);
	shader->SetValue("tiling",tiling);
	shader->SetValue("tileIndex",(int)iCellId);

	mesh.Draw();
}
