#pragma once
#include "foundation_types.h"

#include <cassert>
#include <array>


/// \struct StaticIDTable
/// \brief lookup table from IDs to objects, with assigned maximum size 
/// \details this is a variation of the IDTable, with no dynamically allocated memory.\n
/// It's best suited for a small/controlled number of objects.
template <typename T, unsigned int N>
struct StaticIDTable {
    StaticIDTable();

    /// create and add an object to the table
    ID add(const T &obj=T());
    /// remove an object; return false if the object does not exist
    bool remove(ID id);
    /// get an object, given its ID
    T& get(ID id);
    /// check if an object is in the table
    bool has(ID id) const;
    /// number of objects stored in the map.
    /// \details the size of the internal ids_ and objects_ arrays can be bigger than this.
    uint32_t size() const {  return _size;  }

    std::array<ID,N>       _ids;               ///< id map used for storing uuid and as a lookup ID -> obj
    std::array<uint32_t,N> _obj_to_idx_lookup; ///< used to map back _objects slots to _ids
    std::array<T,N>        _objects;           ///< contiguous array of objects
    uint32_t               _size;              ///< number of objects stored
    uint32_t               _freelist_idx;      ///< index of the first free slot in the _ids array
    uint32_t               _next_uuid;         ///< \todo replace with a per-slot index
};


// StaticIDTable implementation
template <typename T, unsigned int N>
StaticIDTable<T,N>::StaticIDTable()
: _freelist_idx(0), _size(0), _next_uuid(0) {
    // fill the _ids list with the next_free_idx info
    for (uint32_t i=0; i<N; i++) {
        _ids[i].index=UINT32_MAX;
        _ids[i].next_free_idx=i+1;
    }        
}

/// \todo investigate if it's possible to remove the linear search for updating the index in the _ids array
template <typename T, unsigned int N>
bool StaticIDTable<T,N>::remove(ID id) {
    if (has(id)) {
        // swap with last;
        auto internal_idx = _ids[id.index].index;
        _objects[internal_idx] = _objects[--_size];
        _ids[_obj_to_idx_lookup[_size]].index = internal_idx;
        // update free list chain
        _ids[id.index].index = UINT32_MAX;
        _ids[id.index].next_free_idx = _freelist_idx;
        _freelist_idx = id.index;
        return true;
    }
    return false;
}

template <typename T, unsigned int N>
ID StaticIDTable<T,N>::add(const T &obj) {
    assert(_size<N);
    // add the new object in the first free pos of the objects_ array
    _objects[_size] = obj;
    // create an entry in the _ids array
    ID id {_size,_next_uuid++};
    uint32_t next_free_idx = _ids[_freelist_idx].next_free_idx;
    _ids[_freelist_idx] = id;
    id.index = _freelist_idx;
    _freelist_idx = next_free_idx;
    _obj_to_idx_lookup[_size] = id.index; // keep the lookup array in sync
    ++_size;
    return id;
}

template <typename T, unsigned int N>
T& StaticIDTable<T,N>::get(ID id) {
    return _objects[_ids[id.index].index];
}

template <typename T, unsigned int N>
bool StaticIDTable<T,N>::has(ID id) const {
    return id.index<N && _ids[id.index].internal_id == id.internal_id;
}
