// BGFXVertexBuffer.h
// Abstraction for BGFX vertex buffer management
#pragma once
#include <bgfx/bgfx.h>
#include <vector>

class BGFXVertexBuffer {
public:
    BGFXVertexBuffer();
    ~BGFXVertexBuffer();

    // Create a static vertex buffer from raw data
    bool Create(const void* vertexData, uint32_t vertexSize, uint32_t numVertices, const bgfx::VertexLayout& layout);
    // Update a dynamic vertex buffer
    bool Update(const void* vertexData, uint32_t vertexSize, uint32_t numVertices);
    // Destroy the buffer
    void Destroy();
    // Get BGFX handle
    bgfx::VertexBufferHandle GetHandle() const;
    // Is buffer valid
    bool IsValid() const;

private:
    bgfx::VertexBufferHandle m_handle;
    bool m_dynamic;
    uint32_t m_vertexSize;
    uint32_t m_numVertices;
};
