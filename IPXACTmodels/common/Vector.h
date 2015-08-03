//-----------------------------------------------------------------------------
// File: Vector.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 31.07.2015
//
// Description:
// <Short description of the class/file contents>
//-----------------------------------------------------------------------------

#ifndef VECTOR_H
#define VECTOR_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QString>
#include <QXmlStreamWriter>

//-----------------------------------------------------------------------------
// Kactus2 vendor extension for Vectors.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT Vector
{
public:

    /*!
     *  The constructor.
     *
     *      @param [in] left    The left side of the Vector.
     *      @param [in] right   The right side of the Vector.
     */
    Vector(QString left, QString right);

	/*!
	 *  The destructor.
	*/
    virtual ~Vector();

    /*!
     *  Gets the left value of the Vector.
     *
     *      @return The left value of the Vector.
     */
    QString getLeft() const;

    /*!
     *  Gets the right value of the Vector.
     *
     *      @return The right value of the Vector.
     */
    QString getRight() const;

    /*!
     *  Sets the left value for the Vector.
     *
     *      @param [in] newLeft     The value to be set as left.
     */
    void setLeft(QString const& newLeft);

    /*!
     *  Sets the right value for the Vector.
     *
     *      @param [in] newright    The value to be set as right.
     */
    void setRight(QString const& newright);

private:
    //! Disable copying.
    Vector(Vector const& other);

	// Disable assignment.
    Vector& operator=(Vector const& rhs);
    
	//-----------------------------------------------------------------------------
	// Data.
	//-----------------------------------------------------------------------------

    //! The left side of the Vector.
    QString left_;

    //! The right side of the Vector.
    QString right_;

};
#endif // VECTOR_H
