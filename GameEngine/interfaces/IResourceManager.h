#ifndef _IRESOURCEMANAGER_
#define _IRESOURCEMANAGER_

#include <string>

struct TextureInfo
{
	unsigned int uiWidth; // texture width
	unsigned int uiHeight; // texture height
	unsigned int uiCellsWidth; // number of cells horizontally, used for sprite animations
	unsigned int uiCellsHeight; // number of cell vertically, used for sprite animations
};

// This class manages all graphic resources
class IResourceManager
{
public:

	virtual ~IResourceManager() {}

	/**
		(Image file types support:

		JPEG baseline (no JPEG progressive)
		PNG 8-bit-per-channel only

		TGA (not sure what subset, if a subset)
		BMP non-1bpp, non-RLE
		PSD (composited view only, no extra channels)

		GIF (*comp always reports as 4-channel)
		HDR (radiance rgbE format)
		PIC (Softimage PIC)
	**/

	/** Loads a texture
	 * id: uniqueID to be used
	 * file: img
	 * return: true if the texture is or was loaded, false on error
	 **/
	virtual bool LoadTexture(const std::string& id, const std::string& file) = 0;

	/** Loads a sprite animation
	 * id: uniqueID to be used
	 * file: img
	 * return: true if the sprite animation is or was loaded, false on error
	 **/
	virtual bool LoadAnimation(const std::string& id, const std::string& file) = 0;

	/** Loads a font
	 * id: uniqueID to be used
	 * file: img
	 * return: true if the font is or was loaded, false on error
	 **/
	virtual bool LoadFont(const std::string& id, const std::string& file) = 0;

	/** loads a shader
	 * id: uniqueID to be used
	 * vert: vert shader file
	 * frag: frag shader file
	 * return: true if the shader was loaded, false on error
	 **/
	virtual bool LoadShader(const std::string& id, const std::string& vert, const std::string& frag) = 0;

	// return via parameter texture info for a id
	// return true if texture is found, false if not
	virtual bool GetTextureInfo(const std::string& id, TextureInfo& out) const = 0;

	// Removes and deletes all resources loaded
	virtual void Clear() = 0;

};



#endif //_IRESOURCEMANAGER_
