//-----------------------------------------------------------------------------
// File: Range.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 03.08.2015
//
// Description:
// Implementation for range elements e.g. vector and array.
//-----------------------------------------------------------------------------

#ifndef RANGE_H
#define RANGE_H

#include <QString>

#include <IPXACTmodels/ipxactmodels_global.h>

//-----------------------------------------------------------------------------
//! Implementation for range elements e.g. vector and array.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT Range 
{
public:

	//! The constructor.
	Range(QString const& left, QString const& right);

	//! The destructor.
	virtual ~Range();
    
    /*!
     *  Gets the left value of the range.
     *
     *      @return The left value of the Vector.
     */
    QString getLeft() const;

    /*!
     *  Gets the right value of the range.
     *
     *      @return The right value of the Vector.
     */
    QString getRight() const;

    /*!
     *  Sets the left value for the range.
     *
     *      @param [in] newLeft     The value to be set as left.
     */
    void setLeft(QString const& newLeft);

    /*!
     *  Sets the right value for the range.
     *
     *      @param [in] newright    The value to be set as right.
     */
    void setRight(QString const& newright);

private:

	// Disable copying.
	Range(Range const& rhs);
	Range& operator=(Range const& rhs);

    //! The left side of the range.
    QString left_;

    //! The right side of the range.
    QString right_;
};


#endif // RANGE_H
