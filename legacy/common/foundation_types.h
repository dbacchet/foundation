#pragma once

#include <cstdint>

/// \struct ID
/// \brief opaque identifier used as a handle for referencing objects
struct ID {
    uint32_t index = UINT32_MAX;
    union {
        uint32_t internal_id   = UINT32_MAX;
        uint32_t next_free_idx;
    };

    // ID() {}
    ID(uint32_t idx=UINT32_MAX,uint32_t internal_idx=UINT32_MAX)
    : index(idx), internal_id(internal_idx) {}
};

inline bool valid(ID id) {
    return id.index!=UINT32_MAX;
}

inline bool operator<(const ID &id1, const ID &id2) {
    return (id1.index<id2.index) || (id1.index==id2.index && id1.internal_id<id2.internal_id);
}
/// size of a cache line, in bytes
#define CACHE_LINE_SIZE 64
