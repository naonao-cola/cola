/**
 * @FilePath     : /cola/src/UtilsCtrl/Arean/UArean.cpp
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-22 22:38:41
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-22 22:56:19
 **/
#include "UArean.h"

NAO_NAMESPACE_BEGIN

static const int kBlockSize = 4096;

UArena::UArena()
    : alloc_ptr_(nullptr)
    , alloc_bytes_remaining_(0)
    , memory_usage_(0)
{}

UArena::~UArena()
{
    for (NSize i = 0; i < blocks_.size(); i++) {
        delete[] blocks_[i];
    }
}

NCharPtr UArena::AllocateFallback(NSize bytes)
{
    if (bytes > kBlockSize / 4) {
        // Object is more than a quarter of our block size.  Allocate it separately
        // to avoid wasting too much space in leftover bytes.
        NCharPtr result = AllocateNewBlock(bytes);
        return result;
    }

    // We waste the remaining space in the current block.
    alloc_ptr_             = AllocateNewBlock(kBlockSize);
    alloc_bytes_remaining_ = kBlockSize;

    NCharPtr result = alloc_ptr_;
    alloc_ptr_ += bytes;
    alloc_bytes_remaining_ -= bytes;
    return result;
}

NCharPtr UArena::AllocateAligned(NSize bytes)
{
    const int align = (sizeof(void*) > 8) ? sizeof(void*) : 8;
    static_assert((align & (align - 1)) == 0, "Pointer size should be a power of 2");
    size_t   current_mod = reinterpret_cast<uintptr_t>(alloc_ptr_) & (align - 1);
    size_t   slop        = (current_mod == 0 ? 0 : align - current_mod);
    size_t   needed      = bytes + slop;
    NCharPtr result;
    if (needed <= alloc_bytes_remaining_) {
        result = alloc_ptr_ + slop;
        alloc_ptr_ += needed;
        alloc_bytes_remaining_ -= needed;
    }
    else {
        // AllocateFallback always returned aligned memory
        result = AllocateFallback(bytes);
    }
    assert((reinterpret_cast<uintptr_t>(result) & (align - 1)) == 0);
    return result;
}

NCharPtr UArena::AllocateNewBlock(NSize block_bytes)
{
    NCharPtr result = new NChar[block_bytes];
    blocks_.push_back(result);
    memory_usage_.fetch_add(block_bytes + sizeof(NCharPtr), std::memory_order_relaxed);
    return result;
}

NAO_NAMESPACE_END