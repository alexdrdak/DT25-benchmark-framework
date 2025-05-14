#include "VertexBufferLayout.h"

template<>
void VertexBufferLayout::d_Push<float>(unsigned int count, const std::string& name) {
    d_Elements.push_back({GL_FLOAT, count, GL_FALSE, name});
    d_Stride += VertexBufferElement::GetSizeOfType(GL_FLOAT) * count;
}
