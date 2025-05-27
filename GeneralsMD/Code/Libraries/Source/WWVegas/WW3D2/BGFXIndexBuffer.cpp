// BGFXIndexBuffer.cpp
// Implementation for BGFX index buffer abstraction
#include "BGFXIndexBuffer.h"

BGFXIndexBuffer::BGFXIndexBuffer()
    : m_handle(BGFX_INVALID_HANDLE), m_dynamic(false), m_indexSize(0), m_numIndices(0) {}

BGFXIndexBuffer::~BGFXIndexBuffer() {
    Destroy();
}

bool BGFXIndexBuffer::Create(const void* indexData, uint32_t indexSize, uint32_t numIndices) {
    Destroy();
    m_indexSize = indexSize;
    m_numIndices = numIndices;
    m_dynamic = false;
    const bgfx::Memory* mem = bgfx::copy(indexData, indexSize * numIndices);
    m_handle = bgfx::createIndexBuffer(mem);
    return bgfx::isValid(m_handle);
}

bool BGFXIndexBuffer::Update(const void* indexData, uint32_t indexSize, uint32_t numIndices) {
    if (!m_dynamic || !bgfx::isValid(m_handle)) return false;
    // For dynamic buffers, use bgfx::update
    // Not implemented in this stub
    return false;
}

void BGFXIndexBuffer::Destroy() {
    if (bgfx::isValid(m_handle)) {
        bgfx::destroy(m_handle);
        m_handle = BGFX_INVALID_HANDLE;
    }
}

bgfx::IndexBufferHandle BGFXIndexBuffer::GetHandle() const {
    return m_handle;
}

bool BGFXIndexBuffer::IsValid() const {
    return bgfx::isValid(m_handle);
}
