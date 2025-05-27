// BGFXVertexBuffer.cpp
// Implementation for BGFX vertex buffer abstraction
#include "BGFXVertexBuffer.h"

BGFXVertexBuffer::BGFXVertexBuffer()
    : m_handle(BGFX_INVALID_HANDLE), m_dynamic(false), m_vertexSize(0), m_numVertices(0) {}

BGFXVertexBuffer::~BGFXVertexBuffer() {
    Destroy();
}

bool BGFXVertexBuffer::Create(const void* vertexData, uint32_t vertexSize, uint32_t numVertices, const bgfx::VertexLayout& layout) {
    Destroy();
    m_vertexSize = vertexSize;
    m_numVertices = numVertices;
    m_dynamic = false;
    const bgfx::Memory* mem = bgfx::copy(vertexData, vertexSize * numVertices);
    m_handle = bgfx::createVertexBuffer(mem, layout);
    return bgfx::isValid(m_handle);
}

bool BGFXVertexBuffer::Update(const void* vertexData, uint32_t vertexSize, uint32_t numVertices) {
    if (!m_dynamic || !bgfx::isValid(m_handle)) return false;
    // For dynamic buffers, use bgfx::update
    // Not implemented in this stub
    return false;
}

void BGFXVertexBuffer::Destroy() {
    if (bgfx::isValid(m_handle)) {
        bgfx::destroy(m_handle);
        m_handle = BGFX_INVALID_HANDLE;
    }
}

bgfx::VertexBufferHandle BGFXVertexBuffer::GetHandle() const {
    return m_handle;
}

bool BGFXVertexBuffer::IsValid() const {
    return bgfx::isValid(m_handle);
}
