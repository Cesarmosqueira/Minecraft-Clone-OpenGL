#include "util/glutil.h"

class IndexBuffer {
public:
    IndexBuffer(const ui32* data, const ui32& count);
    ~IndexBuffer();
    void bind() const;
    void unbind() const;
    ui32 Addr() const { return this->ID; };
    inline ui32 get_count() const { return this->count; };
private:
    ui32 ID;
    ui32 count;
};

IndexBuffer::IndexBuffer(const ui32* data, const ui32& count) {
    this->count = count;
    glGenBuffers(1, &this->ID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->count * sizeof(ui32), data, GL_STATIC_DRAW);
}
IndexBuffer::~IndexBuffer() {
    glDeleteVertexArrays(1, &this->ID);
}
void IndexBuffer::bind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ID);
}
void IndexBuffer::unbind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
