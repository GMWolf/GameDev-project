#include "VertexBuffer.h"

wagl::VertexBuffer::VertexBuffer(VertexFormat format, int usage) : format(format), usage(usage), elementCount(0)
{
	glGenBuffers(2, buffers);
}

wagl::VertexBuffer::~VertexBuffer()
{
	glDeleteBuffers(2, buffers);
}

void wagl::VertexBuffer::setVertexData(const int vertCount, const void* data)
{
	int size = vertCount *  format.getStride();

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, size, data, usage);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void wagl::VertexBuffer::setElementsData(const int vertCount, const void * data)
{
	elementCount = vertCount;

	int size = vertCount * GetGLTypeSize(GL_UNSIGNED_INT);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, usage);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void wagl::VertexBuffer::bind()
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
}
