//-----------------------------------------------------------------------------
// File: Array.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 01.04.2015
//
// Description:
// Kactus2 vendor extension for arrays.
//-----------------------------------------------------------------------------

#ifndef ARRAY_H
#define ARRAY_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QString>
#include <QXmlStreamWriter>

//-----------------------------------------------------------------------------
// Kactus2 vendor extension for arrays.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT Array
{
public:

    /*!
     *  The constructor.
     *
     *      @param [in] left    The left side of the array.
     *      @param [in] right   The right side of the array.
     */
    Array(QString left, QString right);

	/*!
	 *  The destructor.
	*/
    virtual ~Array();

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
    Array(Array const& other);

	// Disable assignment.
    Array& operator=(Array const& rhs);
    
	//-----------------------------------------------------------------------------
	// Data.
	//-----------------------------------------------------------------------------

    //! The left side of the array.
    QString left_;

    //! The right side of the array.
    QString right_;

};
#endif // ARRAY_H
