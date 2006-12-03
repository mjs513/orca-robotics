/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef ORCA_OG_BUFFER_H
#define ORCA_OG_BUFFER_H

#include <map>

#include <orca/ogfusion.h>
//#include "ogfeature.h"

/*!
    A buffer for Occupancy (Certainty) Grid representation.
    
  @author Alex Makarenko
*/
namespace ogfusion{

class OgBuffer
{
public:
    OgBuffer();
    ~OgBuffer();

    bool isEmpty() const;
    uint size() const;
    void clear();

    orca::OgCellLikelihood& begin();
    orca::OgCellLikelihood& end();

    orca::OgCellLikelihood& insertAdd(const int ind, orca::OgCellLikelihood& Obs );
    orca::OgCellLikelihood& insertReplace(const int ind, orca::OgCellLikelihood& Obs );

    void erase( const int ind ) {};
    void eraseFront();
    void eraseBack();

    void popFront( orca::OgCellLikelihood& obs);
    void popBack( orca::OgCellLikelihood& obs );

    void operator[] (const int ) const {};

private:


    std::map<int,orca::OgCellLikelihood> buffer_;

    //! A memory space for a single feature, stored here after popping from buffer_.
    orca::OgCellLikelihood currentFeature_;

};

// INLINE FUNCTIONS ==================

inline bool OgBuffer::isEmpty() const
{ return buffer_.empty(); };

inline uint OgBuffer::size() const
{ return buffer_.size(); };

inline void OgBuffer::clear()
{ buffer_.clear(); };

inline orca::OgCellLikelihood& OgBuffer::begin()
{ return buffer_.begin()->second; };

inline orca::OgCellLikelihood& OgBuffer::end()
{ return buffer_.end()->second; };

}
#endif
