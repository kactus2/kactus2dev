//-----------------------------------------------------------------------------
// File: Assertion.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 05.08.2015
//
// Description:
// Matches the ipxact:assertion element.
//-----------------------------------------------------------------------------

#ifndef ASSERTION_H
#define ASSERTION_H

#include <IPXACTmodels/common/NameGroup.h>

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QString>
#include <QMap>

//-----------------------------------------------------------------------------
//! Matches the ipxact:assertion element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT Assertion : public NameGroup
{
public:

    /*!
     *  The constructor.
     */
    Assertion();

    /*!
     *  Copy constructor.
     */
    Assertion(const Assertion& other);

    /*!
     *  The destructor.
     */
    ~Assertion();

    /*!
     *  Get the assert associated with this assertion.
     *
     *      @return The assert.
     */
    QString getAssert() const;

    /*!
     *  Set the assert associated with this assertion.
     *
     *      @param [in] newAssert   The new assert value.
     */
    void setAssert(QString const& newAssert);

private:

    //! The assert value. Expression that is expected to evaluate to true.
    QString assert_;

};

#endif // ASSERTION_H
