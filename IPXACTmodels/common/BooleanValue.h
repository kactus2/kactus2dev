//-----------------------------------------------------------------------------
// File: BooleanValue.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 05.08.2015
//
// Description:
// <Short description of the class/file contents>
//-----------------------------------------------------------------------------

#ifndef BOOLEANVALUE_H
#define BOOLEANVALUE_H

#include <QString>

#include <IPXACTmodels/ipxactmodels_global.h>

class IPXACTMODELS_EXPORT BooleanValue 
{
public:

	//! The constructor.
	BooleanValue();

    //! Copy constructor;
    BooleanValue(BooleanValue const& other);

	//! The destructor.
	~BooleanValue();

    BooleanValue& operator=(BooleanValue const& other);

    void setValue(bool value);

    bool toBool() const;

    QString toString();


private:

    // Possible boolean values including unspecified.
    enum Boolean {
        BOOL_TRUE,
        BOOL_FALSE,
        BOOL_UNSPECIFIED
    };

    Boolean value_;
};

#endif // BOOLEANVALUE_H
