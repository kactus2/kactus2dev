/* 
 *  	Created on: 29.8.2012
 *      Author: Antti Kamppi
 * 		filename: writevalueconstraint.h
 *		Project: Kactus 2
 */

#ifndef WRITEVALUECONSTRAINT_H
#define WRITEVALUECONSTRAINT_H

#include "ipxactmodels_global.h"

#include <QString>
#include <QXmlStreamWriter>
#include <QDomNode>

/*! \brief Equals the spirit:writeValueConstraint element in IP-Xact specification.
 *
 * Describes a set of constraint values that can be written to a containing field.
 */
class IPXACTMODELS_EXPORT WriteValueConstraint {


public:

	//! \brief Specifies the different constraint types
	enum Type {
		WRITE_AS_READ = 0,
		USE_ENUM,
		MIN_MAX,
		TYPE_COUNT
	};

	//! \brief The default constructor
	WriteValueConstraint();

	/*! \brief The constructor
	 *
	 * \param constrNode Reference to the QDomNode to parse the information from.
	 *
	*/
	WriteValueConstraint(QDomNode& constrNode);
	
	//! \brief Copy constructor
	WriteValueConstraint(const WriteValueConstraint& other);

	//! \brief Assignment operator
	WriteValueConstraint& operator=(const WriteValueConstraint& other);

	//! \brief The destructor
	virtual ~WriteValueConstraint();

	/*! \brief Write the contents of the class using the writer.
	*
	* Uses the specified writer to write the class contents into file as valid
	* IP-Xact.
	*
	* \param writer A reference to a QXmlStreamWrite instance that is used to
	* write the document into file.
	*/
	void write(QXmlStreamWriter& writer);

	/*! \brief Get the type of the constraint.
	 *
	 * \return The constraint type.
	*/
	Type getType() const;

	/*! \brief Set the type for the constraint.
	 *
	 * \param type The type to set.
	 *
	*/
	void setType(Type type);

	/*! \brief Get the minimum value of the constraint.
	 * 
	 * If constraint type is not WriteValueConstraint::MIN_MAX then 0 is returned.
	 * 
	 * \return The minimum value.
	*/
	unsigned int getMinimum() const;

	/*! \brief Set the minimum value for the constraint.
	 * 
	 * This function also sets the constraint type to WriteValueConstraint::MIN_MAX
	 *
	 * \param minimum The minimum value to set.
	 *
	*/
	void setMinimum(unsigned int minimum);

	/*! \brief Get the maximum value of the constraint.
	 * 
	 * If constraint type is not WriteValueConstraint::MIN_MAX then 0 is returned.
	 *
	 * \return The maximum value.
	*/
	unsigned int getMaximum() const;

	/*! \brief Set the maximum value for the constraint.
	 * 
	 * This function also sets the constraint type to WriteValueConstraint::MIN_MAX
	 *
	 * \param maximum The maximum value to set.
	 *
	*/
	void setMaximum(unsigned int maximum);

private:

	//! \brief Specifies the type of the constraint.
	Type type_;

	//! \brief The value for the spirit:writeAsRead element.
	bool writeAsRead_;

	//! \brief The value for the spirit:useEnumeratedValues element.
	bool useEnumValues_;

	//! \brief The spirit:minimum setting.
	unsigned int min_;

	//! \brief The spirit:maximum setting.
	unsigned int max_;
};

#endif // WRITEVALUECONSTRAINT_H
