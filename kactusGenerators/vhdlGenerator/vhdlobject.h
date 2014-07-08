/* 
 *  	Created on: 26.10.2011
 *      Author: Antti Kamppi
 * 		filename: vhdlobject.h
 *		Project: Kactus 2
 */

#ifndef VHDLOBJECT_H
#define VHDLOBJECT_H

#include <QObject>
#include <QString>
#include <QTextStream>

/*! \brief VhdlObject is a pure virtual class that is used as a base class.
 * 
 * VhdlObject is used as base class for the following classes:
 * - VhdlGeneric
 * - VhdlSignal
 * - VhdlPort
 * 
 */
class VhdlObject : public QObject {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this vhdl object.
	 * \param name Name for this vhdl object.
	 * \param type Type for this vhdl object.
	 * \param defaultValue The default value for the object.
	 * \param description The description for this vhdl object.
	 *
	*/
	VhdlObject(QObject* parent, 
		const QString& name = QString(),
		const QString& type = QString(),
		const QString& defaultValue = QString(),
		const QString& description = QString());
	
	//! \brief The destructor
	virtual ~VhdlObject();

	/*! \brief Write the contents of the object to the text stream.
	 *
	 * This is a pure virtual function and must be implemented in all classes 
	 * derived from this class.
	 *
	 * \param stream The text stream to write the object into.
	 *
	*/
	virtual void write(QTextStream& stream) const = 0;

	/*! \brief Get the name of the vhdl object
	 *
	 *
	 * \return QString contains the name of the object.
	*/
	virtual QString name() const;

	/*! \brief Set the name for the vhdl object.
	 *
	 * \param name contains the name to set.
	 *
	*/
	virtual void setName(const QString& name);

	/*! \brief Get the type of the vhdl object.
	 *
	 *
	 * \return QString contains the type of the object.
	*/
	virtual QString type() const;

	/*! \brief Set the type for the vhdl object.
	 *
	 * \param type contains the type for the object.
	 *
	*/
	virtual void setType(const QString& type);

	/*! \brief Get the description of the vhdl object.
	 *
	 *
	 * \return QString contains the description of the object.
	*/
	virtual QString description() const;

	/*! \brief Set the description for the vhdl object.
	 *
	 * \param description contains the description for the object.
	 *
	*/
	virtual void setDescription(const QString& description);

	/*! \brief Get the default value of the object.
	 *
	 * \return QString contains the default value of the object.
	*/
	virtual QString defaultValue() const;

	/*! \brief Set the default value for the object.
	 *
	 * \param defaultValue Contains the default value to set for the object.
	 *
	*/
	virtual void setDefaultValue(const QString& defaultValue);

private:
	//! \brief No copying
	VhdlObject(const VhdlObject& other);

	//! \brief No assignment
	VhdlObject& operator=(const VhdlObject& other);

    //! \brief The name of the vhdl object.
    QString name_;

    //! \brief The type of the vhdl object.
    QString type_;

    //! \brief The description of the vhdl object.
    QString description_;

    //! \brief The default value for the object.
    QString defaultValue_;
};

#endif // VHDLOBJECT_H
