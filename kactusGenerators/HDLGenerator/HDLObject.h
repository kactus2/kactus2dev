//-----------------------------------------------------------------------------
// File: HDLObject.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 14.7.2014
//
// Description:
// Base class for objects used in HDL generation.
//-----------------------------------------------------------------------------

#ifndef HDLObject_H
#define HDLObject_H

#include <QObject>
#include <QString>
#include <QTextStream>

//-----------------------------------------------------------------------------
//! Base class for objects used in HDL generation.
//-----------------------------------------------------------------------------
class HDLObject : public QObject {
	Q_OBJECT

public:

	/*! The constructor
	 *
	 *      @param [in] parent          The parent object.
	 *      @param [in] name            Name of the object.
	 *      @param [in] type            Type of the object.
	 *      @param [in] defaultValue    The default value of the object.
	 *      @param [in] description     The description of the object.
	*/
	HDLObject(QObject* parent, 
		QString const& name = QString(),
		QString const& type = QString(),
		QString const& defaultValue = QString(),
		QString const& description = QString());
	
	//! The destructor
	virtual ~HDLObject();

	/*! Write the contents of the object to the text stream.	 	 
	 *
	 *      @param [in] stream The text stream to write the object into.
	 */
	virtual void write(QTextStream& stream) const = 0;

	/*! Get the name of the object
	 *
	 *      @return contains the name of the object.
	 */
	virtual QString name() const;

	/*! Set the name for the object.
	 *
	 *      @param [in] name contains the name to set.
	 */
	virtual void setName(QString const& name);

	/*! Get the type of the object.
	 *
	 *      @return The type of the object.
	 */
	virtual QString type() const;

	/*! Set the type for the object.
	 *
	 *      @param [in] type contains the type for the object.
	 */
	virtual void setType(QString const& type);

	/*! Get the description of the object.
	 *
	 *      @return The description of the object.
	 */
	virtual QString description() const;

	/*! Set the description for the object.
	 *
	 *      @param [in] description The new description for the object.
	 */
	virtual void setDescription(QString const& description);

	/*! Get the default value of the object.
	 *
	 *      @return The default value of the object.
	 */
	virtual QString defaultValue() const;

	/*! Set the default value for the object.
	 *
	 *      @param [in] defaultValue The default value to set for the object.	 
	*/
	virtual void setDefaultValue(QString const& defaultValue);

private:
	//! No copying
	HDLObject(const HDLObject& other);

	//! No assignment
	HDLObject& operator=(const HDLObject& other);

    //! The name of the object.
    QString name_;

    //! The type of the object.
    QString type_;

    //! The description of the object.
    QString description_;

    //! The default value for the object.
    QString defaultValue_;
};

#endif // HDLObject_H
