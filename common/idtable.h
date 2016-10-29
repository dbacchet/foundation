#pragma once
#include "foundation_types.h"

#include <vector>


/// \struct IDTable
/// \brief lookup table from IDs to objects
/// \details the IDTable is optimized for lookup and access, and allows to store contiguously
/// in memory the managed objects. Also reordering the objects is possble without any modifications 
/// in the exposed IDs.\n
template <typename T>
struct IDTable {
    IDTable();

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

    std::vector<ID>       _ids;               ///< id map used for storing uuid and as a lookup ID -> obj
    std::vector<uint32_t> _obj_to_idx_lookup; ///< used to map back _objects slots to _ids
    std::vector<T>        _objects;           ///< contiguous array of objects
    uint32_t              _size;              ///< number of objects stored
    uint32_t              _freelist_idx;      ///< index of the first free slot in the _ids array
    uint32_t              _next_uuid;         ///< \todo replace with a per-slot index
};


// IDTable implementation
template <typename T>
IDTable<T>::IDTable()
: _freelist_idx(UINT32_MAX), _size(0), _next_uuid(0) {
}

/// \todo investigate if it's possible to remove the linear search for updating the index in the _ids array
template <typename T>
bool IDTable<T>::remove(ID id) {
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

template <typename T>
ID IDTable<T>::add(const T &obj) {
    // add the new object in the first free pos of the objects_ array
    if (_size>=_objects.size()) {
        _objects.resize(_size+1);
        _obj_to_idx_lookup.resize(_size+1);
    }
    _objects[_size] = obj;
    // create an entry in the _ids array
    ID id {_size,_next_uuid++};
    if (_freelist_idx==UINT32_MAX) {
        _ids.push_back(id);       // internally store the index of the object in the _objects array
        id.index = _ids.size()-1; // outside report the index of the item in the _ids array
    } else {
        uint32_t next_free_idx = _ids[_freelist_idx].next_free_idx;
        _ids[_freelist_idx] = id;
        id.index = _freelist_idx;
    }
    _obj_to_idx_lookup[_size] = id.index; // keep the lookup array in sync
    ++_size;
    return id;
}

template <typename T>
T& IDTable<T>::get(ID id) {
    return _objects[_ids[id.index].index];
}

template <typename T>
bool IDTable<T>::has(ID id) const {
    return id.index<_ids.size() && _ids[id.index].internal_id == id.internal_id;
}

