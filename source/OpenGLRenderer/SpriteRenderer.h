#ifndef _SPRITE_RENDERER_
#define _SPRITE_RENDERER_

#include "IRenderable.h"
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>

// Defines how a sprite should be rendered
class SpriteRenderable : public IRenderable
{
public:

	SpriteRenderable(const glm::mat4& T, const glm::vec4& color, const glm::vec2& tiling, unsigned int iCellId);

	void Render(const class Mesh& mesh, class ApplyShader& shader, const IResource* resource) override;

private:

	// Transformation applied to the sprite
	glm::mat4 T;

	// Color blended with the sprite
	glm::vec4 color;

	// The amount of tiling applied to the sprite
	glm::vec2 tiling;

	// Tile of the spite animation
	unsigned int iCellId;
};

#endif // _SPRITE_RENDERER_
