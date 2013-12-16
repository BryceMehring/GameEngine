#ifndef _RESOURCEFILELOADER_
#define _RESOURCEFILELOADER_

#include <string>

/** Loads a resource file
	 * resource file structure:
	 * texture UniqueStringID PathToImage/img.png
	 * shader UniqueStringID3 PathToShader/VertexShader.vert PathToShader/FragmentShader.frag
	 **/
void LoadResourceFile(const std::string& file, class Game& ,const std::string& folder = ".");

#endif // _RESOURCEFILELOADER_
