/* 
 *
 *  Created on: 26.10.2010
 *      Author: Antti Kamppi
 */

#ifndef ALTERNATEREGISTER_H_
#define ALTERNATEREGISTER_H_

#include "registermodel.h"

#include "ipxactmodels_global.h"

#include <QXmlStreamWriter>
#include <QDomNode>
#include <QString>
#include <QList>
#include <QSharedPointer>

class Choice;
class RegisterDefinition;
class VendorExtension;

/*! \brief Equals the spirit:alternateRegister element in IP-Xact specification.
 *
 * This is a subclass of RegisterModel that holds the base class information for
 * alternateRegister, Register and RegisterFile classes.
 */
class IPXACTMODELS_EXPORT AlternateRegister : public RegisterModel {

public:

	/*! \brief The constructor
	 *
	 *\param registerNode A reference to a QDomNode that can be used to parse
	 * the information from.
	 *
	 * Exception guarantee: basic
	 * \exception Parse_error Occurs if mandatory IP-Xact element is missing in
	 * this class or one of it's member classes.
	 */
	AlternateRegister(QDomNode& registerNode);

	//! Copy constructor
	AlternateRegister(const AlternateRegister& other);

	//! Assignment operator
	AlternateRegister& operator=(const AlternateRegister& other);

	//! \brief The destructor
	virtual ~AlternateRegister();

	/*! \brief Clone the alternate register and return pointer to the copy.
	 * 
	 * This is virtual function so it can be used to make a copy of classes that
	 * inherit register model.
	 *
	 * \return QSharedPointer<RegisterModel> Pointer to the cloned alternate register.
	*/
	virtual QSharedPointer<RegisterModel> clone();

	/*! \brief Write the contents of the class using the writer.
	*
	* Uses the specified writer to write the class contents into file as valid
	* IP-Xact.
	*
	* \param writer A reference to a QXmlStreamWrite instance that is used to
	* write the document into file.
	*/
	void write(QXmlStreamWriter& writer);

	/*! \brief Check if the alternate register is in a valid state.
	 *
     * \param componentChoices  Choices in the containing component.
	 * \param errorList The list to add the possible error messages to.
	 * \param parentIdentifier String from parent to help to identify the location of the error.
	 *
	 * \return bool True if the state is valid and writing is possible.
	*/
	virtual bool isValid(QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices,
        QStringList& errorList, 
		const QString& parentIdentifier) const;

	/*! \brief Check if the alternate register is in a valid state.
	 *
     * \param componentChoices  Choices in the containing component.
     *
	 * \return bool True if the state is valid and writing is possible.
	*/
    virtual bool isValid(QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices) const;

	/*! \brief Get the list of the alternate groups
	 *
	 * \return A reference to a QList containing the names of the groups.
	 */
	const QList<QString>& getAlternateGroups() const;

	/*! \brief Get the pointer to the alternateRegisterDefinition
	 *
	 * \return A pointer to the alternateRegisterDefinition.
	 */
	RegisterDefinition* getAlternateRegisterDef() const;

	/*! \brief Set the alternateGroups for this alternateRegister
	 *
	 * Calling this function will reset the old groups.
	 *
	 * \param alternateGroups A QList containing names of the new groups.
	 */
	void setAlternateGroups(const QList<QString>& alternateGroups);

	/*! \brief Set the the alternateRegisterDefinition
	 *
	 * \param alternateRegisterDef A pointer to the new
	 * alternateRegisterDefinition.
	 */
	void setAlternateRegisterDef(RegisterDefinition* alternateRegisterDef);

private:

	/*! \brief Contains the names of the alternateGroups
	 * MANDATORY spirit:alternateGroups
	 */
	QList<QString> alternateGroups_;

	//! \brief A pointer to RegisterDefinition class.
	QSharedPointer<RegisterDefinition> alternateRegisterDef_;

    /*!
	 * OPTIONAL (spirit: vendorExtensions)
	 * Alternate register vendor extensions.
	 */
    QList<QSharedPointer<VendorExtension> > vendorExtensions_;

};

#endif /* ALTERNATEREGISTER_H_ */
