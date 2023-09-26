//-----------------------------------------------------------------------------
// File: FieldSlice.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 18.8.2023
//
// Description:
// Describes the ipxact:fieldSlice element.
//-----------------------------------------------------------------------------

#include "FieldSlice.h"

#include <IPXACTmodels/common/Enumeration.h>

#include <QList>
#include <QString>

//-----------------------------------------------------------------------------
// Function: FieldSlice::FieldSlice()
//-----------------------------------------------------------------------------
FieldSlice::FieldSlice(QString const& name):
    NameGroup(name),
    FieldReference(),
    Range(QString(), QString())
{
    
}

//-----------------------------------------------------------------------------
// Function: FieldSlice::FieldSlice()
//-----------------------------------------------------------------------------
FieldSlice::FieldSlice( const FieldSlice& other ):
    NameGroup(other),
    FieldReference(other),
    Range(other)
{

}

//-----------------------------------------------------------------------------
// Function: FieldSlice::operator=()
//-----------------------------------------------------------------------------
FieldSlice& FieldSlice::operator=( const FieldSlice& other )
{
	if (this != &other)
    {
        NameGroup::operator=(other);
        FieldReference::operator=(other);
        Range::operator=(other);
	}
	return *this;
}

//-----------------------------------------------------------------------------
// Function: FieldSlice::~FieldSlice()
//-----------------------------------------------------------------------------
FieldSlice::~FieldSlice()
{

}
