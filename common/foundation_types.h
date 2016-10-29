#pragma once

#include <cstdint>

/// \struct ID
/// \brief opaque identifier used as a handle for referencing objects
struct ID {
    uint32_t index;
    union {
        uint32_t internal_id;
        uint32_t next_free_idx;
    };
};

