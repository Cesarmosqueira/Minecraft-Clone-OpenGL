#include "util/glutil.h"

class VertexBuffer {
public:
    VertexBuffer(const void* data, ui32 size);
    ~VertexBuffer();
    void bind() const;
    void unbind() const;
private:
    ui32 ID;
};

VertexBuffer::VertexBuffer(const void* data, ui32 size) {
    glGenBuffers(1, &this->ID);
    glBindBuffer(GL_ARRAY_BUFFER, this->ID);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}
VertexBuffer::~VertexBuffer() {
    glDeleteVertexArrays(1, &this->ID);
}
void VertexBuffer::bind() const {
    glBindBuffer(GL_ARRAY_BUFFER, this->ID);
}
void VertexBuffer::unbind() const {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
