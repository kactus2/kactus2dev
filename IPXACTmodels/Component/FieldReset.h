//-----------------------------------------------------------------------------
// File: FieldReset.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 09.01.2019
//
// Description:
// Describes the ipxact:reset element.
//-----------------------------------------------------------------------------

#ifndef FIELDRESET_H
#define FIELDRESET_H


#include <IPXACTmodels/ipxactmodels_global.h>

#include <QString>

//-----------------------------------------------------------------------------
//! Describes the ipxact:reset element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT FieldReset
{

public:

	/*!
     *  The constructor.
     */
	FieldReset();

	/*!
     *  Copy constructor.
     */
    FieldReset(const FieldReset& other);

	/*!
     *  Assignment operator.
     */
    FieldReset& operator=(const FieldReset& other);

	/*!
     *  The destructor.
     */
    virtual ~FieldReset();

    /*!
     *  Get the reset type reference.
     *
     *      @return The reset type reference.
     */
    QString getResetTypeReference() const;

    /*!
     *  Set the reset type reference.
     *
     *      @param [in] newResetTypeReference   The new reset type reference.
     */
    void setResetTypeReference(QString const& newResetTypeReference);

    /*!
     *  Get the reset value.
     *
     *      @return The reset value.
     */
    QString getResetValue() const;

    /*!
     *  Set the reset value.
     *
     *      @param [in] newResetValue   The new reset value.
     */
    void setResetValue(QString const& newResetValue);

    /*!
     *  Get the reset mask.
     *
     *      @return The reset mask.
     */
    QString getResetMask() const;

    /*!
     *  Set the reset mask.
     *
     *      @param [in] newResetMask    The new reset mask.
     */
    void setResetMask(QString const& newResetMask);

private:

	//-----------------------------------------------------------------------------
	// Data.
	//-----------------------------------------------------------------------------

    //! The type of the defined reset.
    QString resetTypeReference_;

    //! The reset value.
    QString resetValue_;

    //! Defines the bits that have a known reset value.
    QString resetMask_;
};

#endif // FIELDRESET_H
