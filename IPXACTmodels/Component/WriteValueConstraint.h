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
	WriteValueConstraint(WriteValueConstraint const& other);

	//! Assignment operator.
	WriteValueConstraint& operator=(WriteValueConstraint const& other);

	//! The destructor.
	virtual ~WriteValueConstraint();

	/*! 
     *  Get the type of the constraint.
	 *
	 *    @return The constraint type.
	 */
	Type getType() const;

	/*! 
     *  Set the type for the constraint.
	 *
	 *    @param [in] newType     The new type.
	 */
	void setType(Type newType);

	/*!
     *  Get the minimum value of the constraint.
	 * 
	 *    @return The minimum value.
	 */
    QString getMinimum() const;

	/*!
     *  Set the minimum value for the constraint.
	 * 
	 *    @param [in] newMinimum  The minimum value to set.
	 */
    void setMinimum(QString const& newMinimum);

	/*!
     *  Get the maximum value of the constraint.
	 * 
	 *    @return The maximum value.
	 */
    QString getMaximum() const;

	/*!
     *  Set the maximum value for the constraint.
	 * 
	 *    @param [in] newMaximum  The maximum value to set.
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

//! Namespace for converting write value constraint types
namespace WriteValueConversions
{
    /*!
     *  Convert a string to write value constraint type.
     *
     *    @param [in] typeText    The selected string.
     *
     *    @return Write value constraint type matching the selected string.
     */
    IPXACTMODELS_EXPORT WriteValueConstraint::Type stringToType(QString const& typeText);

    /*!
     *  Convert a write value constraint type to string.
     *
     *    @param [in] constraintType      The selected write value constraint type.
     *
     *    @return String matching the selected write value constraint type.
     */
    IPXACTMODELS_EXPORT QString typeToString(WriteValueConstraint::Type const& constraintType);

    /*!
     *  Get a list of all the write value constraint types in string format.
     *
     *    @return String list of all the write value constraint types.
     */
    IPXACTMODELS_EXPORT QStringList getConstraintTypes();
};

#endif // WRITEVALUECONSTRAINT_H
