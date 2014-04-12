// DreamWorks Animation LLC Confidential Information.
// TM and (c) 2014 DreamWorks Animation LLC.  All Rights Reserved.
// Reproduction in whole or in part without prior written permission of a
// duly authorized representative is prohibited.

#ifndef SDC_ARRAY_INTERFACE_H
#define SDC_ARRAY_INTERFACE_H

#include "../version.h"

#include <vector>

namespace OpenSubdiv {
namespace OPENSUBDIV_VERSION {

//
//  This class provides a simple array-like interface -- a subset of the interface
//  of std::vector -- for a sequence of elements stored in contiguous memory.  It
//  provides a unified representation for referencing data on the stack, all or a
//  subset of std::vector<>, or anywhere else in memory.
//
//  Note that its members are head/size rather than begin/end as in std::vector --
//  we frequently need only the size for many queries, and that is most often what
//  is stored elsewhere in other classes, so we hope to reduce unnecessary address
//  arithmetic constructing the interface and accessing the size.  The size type
//  is also specifically 32-bit (rather than size_t) to match internal usage and
//  avoid unnecessary conversion to/from 64-bit.
//
//  Still debating whether to have two separate classes -- const and non-const --
//  or to unify them into one.  A unified class will require a non-const* member
//  and is more easily accidentally modified by forgetting a "const" somewhere.
//  The separate classes make the distinction more visible and enforcible.
//
//  To do:
//      - need to have the non-const version inherit from the const version so
//        that the non-const array can be accepted as a const array.
//
template <typename TYPE>
class SdcArrayInterface
{
public:
    typedef TYPE  value_type;
    typedef int   size_type;
    typedef TYPE& reference;
    typedef TYPE* iterator;

public:
    SdcArrayInterface() : _begin(0), _size(0) { }
    SdcArrayInterface(value_type* ptr, size_type size) : _begin(ptr), _size(size) { }
    SdcArrayInterface(std::vector<value_type>& vec) : _begin(vec.begin()), _size(vec.size()) { }
    ~SdcArrayInterface() { }

    size_type size() const { return _size; }

    reference operator[](int index) const { return _begin[index]; }

    iterator begin() const { return _begin; }
    iterator end()   const { return _begin + _size; }

protected:
    value_type* _begin;
    size_type   _size;
};

template <typename TYPE>
class SdcConstArrayInterface
{
public:
    typedef TYPE        value_type;
    typedef int         size_type;
    typedef TYPE const& const_reference;
    typedef TYPE const* const_iterator;

public:
    SdcConstArrayInterface() : _begin(0), _size(0) { }
    SdcConstArrayInterface(const value_type* ptr, size_type size) : _begin(ptr), _size(size) { }
    SdcConstArrayInterface(const std::vector<value_type>& vec) : _begin(&vec[0]), _size(vec.size()) { }
    SdcConstArrayInterface(const SdcArrayInterface<value_type>& a) : _begin(&a[0]), _size(a.size()) { }
    ~SdcConstArrayInterface() { }

    size_type size() const { return _size; }

    const_reference operator[](int index) const { return _begin[index]; }

    const_iterator begin() const { return _begin; }
    const_iterator end()   const { return __begin + _size; }

protected:
    value_type const* _begin;
    size_type         _size;
};

} // end namespace OPENSUBDIV_VERSION
using namespace OPENSUBDIV_VERSION;
} // end namespace OpenSubdiv

#endif /* SDC_TYPES_H */

// TM and (c) 2014 DreamWorks Animation LLC.  All Rights Reserved.
// Reproduction in whole or in part without prior written permission of a
// duly authorized representative is prohibited.
