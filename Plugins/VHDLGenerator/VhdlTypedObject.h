// File: VhdlTypedObject.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 04.12.2012
//
// Description:
// Class for containing VHDL type and default value.
//-----------------------------------------------------------------------------

#ifndef VHDLTYPEDOBJECT_H
#define VHDLTYPEDOBJECT_H

#include "vhdlobject.h"

#include <QObject>
#include <QString>
#include <QTextStream>

class VhdlTypedObject : public VhdlObject
{

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] parent   The owner of this vhdl object.
	 *      @param [in] name   Name of this vhdl object.
	 *      @param [in] type   Type of this vhdl object.
	 *      @param [in] defaultValue   The default value for the object.
	 *      @param [in] description   The Description for this vhdl object.
	 */
	VhdlTypedObject(const QString& name = QString(),
		const QString& type = QString(),
		const QString& defaultValue = QString(),
		const QString& description = QString());
	
	/*!
	 *  The destructor.
	 */
	virtual ~VhdlTypedObject();

	/*! \brief Write the contents of the object to the text stream.
	 *
	 * This is a pure virtual function and must be implemented in all classes 
	 * derived from this class.
	 *
	 * \param stream The text stream to write the object into.
	 *
	*/
	virtual void write(QTextStream& stream) const = 0;

	/*!
	 *  Get the type of the vhdl object.
     *
	 *  @return     The type of the object.
	 */
	virtual QString type() const;

	/*!
	 *  Set the type for the vhdl object.
	 *
	 *      @param [in] type   The type for the object.
	 */
	virtual void setType(const QString& type);

	/*!
	 *  Get the default value of the object.
	 *
     *  @return     The default value of the object.
	 */
	virtual QString defaultValue() const;

	/*!
	 *  Set the default value for the object.
	 *
	 *      @param [in] defaultValue   The default value to be set for the object.
	 */
	virtual void setDefaultValue(const QString& defaultValue);

private:
	//! Disable copying
	VhdlTypedObject(const VhdlTypedObject& other);

	//! No assignment
	VhdlTypedObject& operator=(const VhdlTypedObject& other);

    //! The type of the vhdl object.
    QString type_;

    //! The default value for the object.
    QString defaultValue_;
};

#endif // VHDLTYPEDOBJECT_H
