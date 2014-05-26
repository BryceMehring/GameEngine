#include "SpriteRenderer.h"
#include "ApplyShader.h"
#include "ResourceManager.h"

SpriteRenderable::SpriteRenderable(const glm::mat4& T, const glm::vec4& color, const glm::vec2& tiling, unsigned int iCellId) : T(T), color(color), tiling(tiling), iCellId(iCellId)
{
}

void SpriteRenderable::Setup(class ApplyTexturedShader& shader, const IResource &resource)
{
	const Texture* pTexture = static_cast<const Texture*>(resource.QueryInterface(ResourceType::Texture));
	assert("Invalid resource selected" && (pTexture != nullptr));

	shader->BindTexture(*pTexture);
	shader->SetValue("animationTiles", glm::vec2(pTexture->GetCellsWidth(), pTexture->GetCellsHeight()));
}

void SpriteRenderable::Render(ApplyTexturedShader& shader, const IResource&)
{
	shader->SetColor(color);
	shader->SetValue("transformation",T);
	shader->SetValue("tiling",tiling);
	shader->SetValue("tileIndex",iCellId);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, 0);
}
