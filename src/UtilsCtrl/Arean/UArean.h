/**
 * @FilePath     : /cola/src/UtilsCtrl/Arean/UArean.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-22 22:38:25
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-22 22:38:40
 **/
#ifndef NAO_UAREAN_H
#define NAO_UAREAN_H

#include <atomic>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <vector>

#include "../UtilsObject.h"
NAO_NAMESPACE_BEGIN

class UArena : public UtilsObject
{
public:
    UArena();

    ~UArena() override;

    // Return a pointer to a newly allocated memory block of "bytes" bytes.
    NCharPtr Allocate(NSize bytes);

    // Allocate memory with the normal alignment guarantees provided by malloc.
    NCharPtr AllocateAligned(NSize bytes);

    // Returns an estimate of the total memory usage of data allocated
    // by the arena.
    NSize MemoryUsage() const { return memory_usage_.load(std::memory_order_relaxed); }

protected:
    NAO_NO_ALLOWED_COPY(UArena)
private:
    NCharPtr AllocateFallback(NSize bytes);
    NCharPtr AllocateNewBlock(NSize block_bytes);

    // Allocation state
    NCharPtr alloc_ptr_;
    NSize    alloc_bytes_remaining_;

    // Array of new[] allocated memory blocks
    std::vector<char*> blocks_;

    // Total memory usage of the arena.
    //
    // TODO(costan): This member is accessed via atomics, but the others are
    //               accessed without any locking. Is this OK?
    std::atomic<NSize> memory_usage_;
};

inline NCharPtr UArena::Allocate(NSize bytes)
{
    // The semantics of what to return are a bit messy if we allow
    // 0-byte allocations, so we disallow them here (we don't need
    // them for our internal use).
    assert(bytes > 0);
    if (bytes <= alloc_bytes_remaining_) {
        NCharPtr result = alloc_ptr_;
        alloc_ptr_ += bytes;
        alloc_bytes_remaining_ -= bytes;
        return result;
    }
    return AllocateFallback(bytes);
}

NAO_NAMESPACE_END
#endif
