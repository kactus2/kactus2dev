//-----------------------------------------------------------------------------
// File: Kactus2Array.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 01.04.2015
//
// Description:
// Kactus2 vendor extension for arrays.
//-----------------------------------------------------------------------------

#ifndef KACTUS2ARRAY_H
#define KACTUS2ARRAY_H

#include <IPXACTmodels/VendorExtension.h>

//-----------------------------------------------------------------------------
// Kactus2 vendor extension for arrays.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT Kactus2Array : public VendorExtension
{
public:

    /*!
     *  The constructor.
     *
     *      @param [in] left    The left side of the array.
     *      @param [in] right   The right side of the array.
     */
    Kactus2Array(QString left, QString right);

	/*!
	 *  The destructor.
	*/
    virtual ~Kactus2Array();

    /*!
     *  Clones the vendor extension.
     *
     *      @return The clone copy of the vendor extension.
     */
    virtual Kactus2Array* clone() const;

    /*!
     *  Returns a type identifier for the vendor extension.
     *
     *      @return A type identifier of the vendor extension.
     */
    virtual QString type() const;

    /*!
     *  Writes the vendor extension to XML.
     *
     *      @param [in] writer   The writer used for writing the XML.
     */
    virtual void write(QXmlStreamWriter& writer) const;

    /*!
     *  Gets the left value of the array.
     *
     *      @return The left value of the array.
     */
    QString getLeft() const;

    /*!
     *  Gets the right value of the array.
     *
     *      @return The right value of the array.
     */
    QString getRight() const;

    /*!
     *  Sets the left value for the array.
     *
     *      @param [in] newLeft     The value to be set as left.
     */
    void setLeft(QString const& newLeft);

    /*!
     *  Sets the right value for the array.
     *
     *      @param [in] newright    The value to be set as right.
     */
    void setRight(QString const& newright);

private:
    //! Disable copying.
    Kactus2Array(Kactus2Array const& other);

	// Disable assignment.
    Kactus2Array& operator=(Kactus2Array const& rhs);
    
	//-----------------------------------------------------------------------------
	// Data.
	//-----------------------------------------------------------------------------

    //! The left side of the array.
    QString left_;

    //! The right side of the array.
    QString right_;

};
#endif // KACTUS2ARRAY_H
