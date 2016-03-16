//-----------------------------------------------------------------------------
// File: WriteValueConstraint.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 24.09.2015
//
// Description:
// Describes the ipxact:writeValueConstraint element.
//-----------------------------------------------------------------------------

#ifndef WRITEVALUECONSTRAINT_H
#define WRITEVALUECONSTRAINT_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QString>
#include <QXmlStreamWriter>

//-----------------------------------------------------------------------------
//! Describes the ipxact:writeValueConstraint element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT WriteValueConstraint
{

public:

	//! Specifies the different write value constraint types.
	enum Type
    {
		WRITE_AS_READ = 0,
		USE_ENUM,
		MIN_MAX,
		TYPE_COUNT
	};

	//! The default constructor.
	WriteValueConstraint();

	//! Copy constructor.
	WriteValueConstraint(const WriteValueConstraint& other);

	//! Assignment operator.
	WriteValueConstraint& operator=(const WriteValueConstraint& other);

	//! The destructor.
	virtual ~WriteValueConstraint();

	/*! 
     *  Get the type of the constraint.
	 *
	 *      @return The constraint type.
	 */
	Type getType() const;

	/*! 
     *  Set the type for the constraint.
	 *
	 *      @param [in] newType     The new type.
	 */
	void setType(Type newType);

	/*!
     *  Get the minimum value of the constraint.
	 * 
	 *      @return The minimum value.
	 */
    QString getMinimum() const;

	/*!
     *  Set the minimum value for the constraint.
	 * 
	 *      @param [in] newMinimum  The minimum value to set.
	 */
    void setMinimum(QString const& newMinimum);

	/*!
     *  Get the maximum value of the constraint.
	 * 
	 *      @return The maximum value.
	 */
    QString getMaximum() const;

	/*!
     *  Set the maximum value for the constraint.
	 * 
	 *      @param [in] newMaximum  The maximum value to set.
	 */
    void setMaximum(QString const& newMaximum);

private:

	//! Specifies the type of the constraint.
	Type type_;

	//! Contains whether legal values for this field must be read from this field or not.
	bool writeAsRead_;

	//! Implies whether the only legal values are specified in the use enumerated values element or not.
	bool useEnumValues_;

	//! The minimum value for this field.
    QString minimum_;

	//! The maximum value for this field.
    QString maximum_;
};

#endif // WRITEVALUECONSTRAINT_H
