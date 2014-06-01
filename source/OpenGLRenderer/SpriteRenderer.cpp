#include "SpriteRenderer.h"
#include "ApplyShader.h"
#include "ResourceManager.h"

enum
{
	TRANSFORMATION_LOCATION = 1,
	ANIMATION_TILES_LOCATION,
	TILING_LOCATION,
	TILE_INDEX_LOCATION,
};

SpriteRenderable::SpriteRenderable(const glm::mat4& T, const glm::vec4& color, const glm::vec2& tiling, unsigned int iCellId) : T(T), color(color), tiling(tiling), iCellId(iCellId)
{
}

void SpriteRenderable::Setup(class ApplyTexturedShader& shader, const IResource &resource)
{
	const Texture* pTexture = static_cast<const Texture*>(resource.QueryInterface(ResourceType::Texture));
	assert("Invalid resource selected" && (pTexture != nullptr));

	shader->BindTexture(*pTexture);
	shader->SetValue(ANIMATION_TILES_LOCATION, glm::vec2(pTexture->GetCellsWidth(), pTexture->GetCellsHeight()));
}

void SpriteRenderable::Render(ApplyTexturedShader& shader, const IResource&)
{
	shader->SetColor(color);
	shader->SetValue(TRANSFORMATION_LOCATION, T);
	shader->SetValue(TILING_LOCATION, tiling);
	shader->SetValue(TILE_INDEX_LOCATION, iCellId);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, 0);
}
