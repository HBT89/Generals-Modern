// BGFXIndexBuffer.h
// Abstraction for BGFX index buffer management
#pragma once
#include <bgfx/bgfx.h>
#include <vector>

class BGFXIndexBuffer {
public:
    BGFXIndexBuffer();
    ~BGFXIndexBuffer();

    // Create a static index buffer from raw data
    bool Create(const void* indexData, uint32_t indexSize, uint32_t numIndices);
    // Update a dynamic index buffer
    bool Update(const void* indexData, uint32_t indexSize, uint32_t numIndices);
    // Destroy the buffer
    void Destroy();
    // Get BGFX handle
    bgfx::IndexBufferHandle GetHandle() const;
    // Is buffer valid
    bool IsValid() const;

private:
    bgfx::IndexBufferHandle m_handle;
    bool m_dynamic;
    uint32_t m_indexSize;
    uint32_t m_numIndices;
};
