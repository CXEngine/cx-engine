#pragma once

#include <cx-engine/defs/types.hpp>

#include <memory_resource>
#include <algorithm>
#include <cstdlib>

namespace cx {

struct DynArenaBlock {
    DynArenaBlock* next;
    usize size;
    usize offset;
    byte data[];
};

class DynArena {
public:
    explicit DynArena(usize blockSize = 64 * 1024)
        : blockSize(blockSize), head(nullptr) {}

    ~DynArena() {
        reset(true);
    }

    void* alloc(usize size, usize align) {
        if (!head || !has_space(head, size, align)) {
            addBlock(std::max(blockSize, size + align));
        }

        uintptr ptr = (usize) (head->data + head->offset);
        uintptr aligned = (ptr + align - 1) & ~(align - 1);
        uintptr newOffset = aligned - (usize) head->data + size;

        void* result = (void*) aligned;
        head->offset = newOffset;
        return result;
    }

    template<typename T, typename... Args>
    T* create(Args&&... args) {
        void* mem = alloc(sizeof(T), alignof(T));
        return new (mem) T(std::forward<Args>(args)...);
    }

    void reset(bool doFreeMem = false) {
        DynArenaBlock* block = head;
        while (block) {
            DynArenaBlock* next = block->next;
            if (doFreeMem) {
                std::free(block);
            } else {
                block->offset = 0;
            }
            block = next;
        }

        if (!doFreeMem && head) {
            head->next = nullptr;
        }

        if (doFreeMem) {
            head = nullptr;
        }
    }

private:
    usize blockSize;
    DynArenaBlock* head;

    static bool has_space(DynArenaBlock* block, usize size, usize align) {
        usize ptr = (usize) (block->data + block->offset);
        usize aligned = (ptr + align - 1) & ~(align - 1);
        usize newOffset = aligned - (usize) block->data + size;
        return newOffset <= block->size;
    }

    void addBlock(usize size) {
        usize total = sizeof(DynArenaBlock) + size;
        DynArenaBlock* block = (DynArenaBlock*) std::malloc(total);
        block->next = head;
        block->size = size;
        block->offset = 0;
        head = block;
    }
};

class DynArenaResource: public std::pmr::memory_resource {
public:
    explicit DynArenaResource(DynArena& arena) : arena(arena) {}

private:
    DynArena& arena;

    void* do_allocate(usize bytes, usize alignment) override {
        return arena.alloc(bytes, alignment);
    }

    void do_deallocate(void*, usize, usize) override {
        // no-op
    }

    bool do_is_equal(const memory_resource& other) const noexcept override {
        return this == &other;
    }
};

} // namespace cx
