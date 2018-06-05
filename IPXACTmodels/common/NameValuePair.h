//-----------------------------------------------------------------------------
// File: NameValuePair.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 16.09.2015
//
// Description:
// Describes the ipxact:nameValuePair element.
//-----------------------------------------------------------------------------

#ifndef NAMEVALUEPAIR_H
#define NAMEVALUEPAIR_H

#include <IPXACTmodels/common/NameGroup.h>
#include <IPXACTmodels/common/Extendable.h>

#include <QString>

//-----------------------------------------------------------------------------
//! Describes the ipxact:nameValuePair element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT NameValuePair : public NameGroup, public Extendable
{

public:

    /*!
     *  The constructor.
     *
     *      @param [in] name    Name of the pair.
     *      @param [in] value   Value of the pair.
     */
    NameValuePair(QString const& name = QString(), QString const& value = QString());

    //! Copy constructor.
    NameValuePair(const NameValuePair& other);

    //! Assignment operator.
    NameValuePair& operator=(const NameValuePair& other);

    //! The destructor.
    ~NameValuePair();

    /*!
     *  Get the contained value.
     *
     *      @return The value.
     */
    QString getValue() const;

    /*!
     *  Set a value.
     *
     *      @param [in] newValue    The new value.
     */
    void setValue(QString const& newValue);

private:

    //! The value.
    QString value_;
};

#endif // NAMEVALUEPAIR_H
