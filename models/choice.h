/* $Id
 *
 *  Created on: 15.10.2010
 *      Author: Antti kamppi
 */

#ifndef CHOICE_H_
#define CHOICE_H_

#include "generaldeclarations.h"

#include <QString>
#include <QList>
#include <QXmlStreamWriter>

/*! \brief Equals the spirit:choice element in IP-Xact specification
 *
 * Contains the name of the choice and the enumerations defined in it.
 */
class Choice {

public:

	/*! \brief The constructor
	 *
	 * \param choice A QDomNode that can be used to parse the information from.
	 *
	 * Exception guarantee: basic
	 * \exception Parse_error Occurs when a mandatory element is missing in
	 * this class or one of it's member classes.
	 */
	Choice(QDomNode &choice);

	//! Copy constructor
	Choice(const Choice& other);

	//! Assignment operator
	Choice& operator=(const Choice& other);

	//! \brief The destructor
	virtual ~Choice();

	/*! \brief Write the contents of the class using the writer.
	*
	* Uses the specified writer to write the class contents into file as valid
	* IP-Xact.
	*
	* \param writer A reference to a QXmlStreamWrite instance that is used to
	* write the document into file.
	*/
	void write(QXmlStreamWriter& writer);

	/*! \brief Check if the choice is in a valid state.
	 *
	 * \param errorList The list to add the possible error messages to.
	 * \param parentIdentifier String from parent to help to identify the location of the error.
	 *
	 * \return bool True if the state is valid and writing is possible.
	*/
	bool isValid(QStringList& errorList, 
		const QString& parentIdentifier) const;

	/*! \brief Check if the choice is in a valid state.
	 *
	 * \return bool True if the state is valid and writing is possible.
	*/
	bool isValid() const;

	/*! \brief Get the name of the choice.
	 *
	 * \return QString containing the name.
	 */
	QString getName() const;

	/*! \brief Set the name of the choice.
	 *
	 * \param name QString containing the name.
	 */
	void setName(const QString& name);

	/*! \brief Get the enumerations.
	 *
	 * \return QList containing the enumerations.
	 */
	const QList<General::Enumeration>& getEnumerations() const;

	/*! \brief Set the enumerations for this choice.
	 *
	 * Calling this function will clear all previous enumerations.
	 *
	 * \param enumerations QList containing the new enumerations.
	 */
	void setEnumerations(QList<General::Enumeration>& enumerations);

private:

	/*! \brief Name of the Choice element
	 * MANDATORY spirit:name
	 */
	QString choiceName_;

	/*! \brief List of enumerations attached to the choice.
	 * MANDATORY spirit:enumeration
	 */
	QList<General::Enumeration> enumerations_;

};

#endif /* CHOICE_H_ */
