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

	/*  Loads a resource file
		(Image file types support:
 
		JPEG baseline (no JPEG progressive)
		PNG 8-bit-per-channel only

		TGA (not sure what subset, if a subset)
		BMP non-1bpp, non-RLE
		PSD (composited view only, no extra channels)

		GIF (*comp always reports as 4-channel)
		HDR (radiance rgbE format)
		PIC (Softimage PIC)
	  
		resource file structure:
		Texture UniqueStringID PathToImage/img.png
		shader UniqueStringID3 PathToShader/VertexShader.vert PathToShader/FragmentShader.frag
	*/
    virtual bool LoadResourceFile(const std::string& file) = 0;

	// Loads a texture
	// id: uniqueID to be used
	// file: img
    virtual bool LoadTexture(const std::string& id, const std::string& file) = 0;

	// loads a shader
	// id: uniqueID to be used
	// vert: vert shader file
	// frag: frag shader file
    virtual bool LoadShader(const std::string& id, const std::string& vert, const std::string& frag) = 0;

	// return via parameter texture info for a stringID
	// return true if texture is found, false if not
	virtual bool GetTextureInfo(const std::string& id, TextureInfo& out) const = 0;

	// todo: need to implement these, do not use
	virtual void RemoveTexture(const std::string& id) = 0;
	virtual void RemoveAllTextures() = 0;
	virtual void RemoveAllShaders() = 0;

};



#endif //_IRESOURCEMANAGER_
