#include "GenerateBuffers.h"
#include <GL/glew.h>
#include <vector>

unsigned int CreateQuadIndexBuffer(unsigned short length)
{
    std::vector<unsigned short> indexBuffer;

    indexBuffer.resize(6*length);
    for(unsigned short i = 0; i < length; ++i)
    {
        // Tri 1
        indexBuffer[i*6] = 4*i + 1;
        indexBuffer[i*6 + 1] =  4*i + 2;
        indexBuffer[i*6 + 2] = 4*i;

        //Tri 2
        //
        indexBuffer[i*6 + 3] = 4*i + 2;
        indexBuffer[i*6 + 4] = 4*i + 3;
        indexBuffer[i*6 + 5] = 4*i + 1;
    }

    unsigned int id;
    glGenBuffers(1,&id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBuffer.size() * sizeof(unsigned short), &indexBuffer[0], GL_STATIC_DRAW);

    return id;
}
