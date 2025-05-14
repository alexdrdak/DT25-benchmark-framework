#ifndef DT25_VERTEXBUFFERLAYOUT_H
#define DT25_VERTEXBUFFERLAYOUT_H

#include <vector>
#include "../d_glob.h"

struct VertexBufferElement
{
    unsigned int type;
    unsigned int count;
    unsigned char normalized;
    std::string dDebugName;

    static unsigned int GetSizeOfType(unsigned int type) {
        switch(type) {
            case GL_FLOAT:          return 4;
            case GL_UNSIGNED_INT:   return 4;
            case GL_UNSIGNED_BYTE:  return 1;
        }

        return 0;
    }
};


class VertexBufferLayout {
private:
    std::vector<VertexBufferElement> d_Elements;
    unsigned int d_Stride;
public:
    VertexBufferLayout() : d_Stride(0) {};

    template<typename T>
    void d_Push(unsigned int count, const std::string& name = "");

    inline std::vector<VertexBufferElement> d_GetElements() const& {return d_Elements; }
    inline unsigned int d_GetStride() const { return d_Stride; }
};

#endif