//-----------------------------------------------------------------------------
// File: choice.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 15.10.2010
//
// Description:
// Equals the spirit:choice element in IP-Xact specification.
// Choice contains the name of the choice and the enumerations defined in it.
//-----------------------------------------------------------------------------

#ifndef CHOICE_H_
#define CHOICE_H_

#include "ipxactmodels_global.h"

#include <QDomNode>
#include <QString>
#include <QStringList>
#include <QList>
#include <QXmlStreamWriter>

class Enumeration;

//-----------------------------------------------------------------------------
//! Equals the spirit:choice element in IP-Xact specification.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT Choice
{

public:

	/*! The constructor
	 *
	 * @param [in] choice A QDomNode that can be used to parse the information from.
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

	//! The destructor
	virtual ~Choice();

	/*! Write the contents of the class using the writer.
	*
	* Uses the specified writer to write the class contents into file as valid IP-Xact.
	*
	* @param [in] writer A reference to a QXmlStreamWrite instance that is used to
	* write the document into file.
	*/
    void write(QXmlStreamWriter& writer) const;

	/*! Check if the choice is in a valid state.
	 *
	 * @param [inout] errorList The list to add the possible error messages to.
	 * @param [in] parentIdentifier String from parent to help to identify the location of the error.
	 *
	 * \return bool True if the state is valid and writing is possible.
	*/
	bool isValid(QStringList& errorList, 
		const QString& parentIdentifier) const;

	/*! Check if the choice is in a valid state.
	 *
	 * \return bool True if the state is valid and writing is possible.
	*/
	bool isValid() const;

	/*! Get the name of the choice.
	 *
	 * \return QString containing the name.
	 */
	QString getName() const;

	/*! Set the name of the choice.
	 *
	 * @param [in] name QString containing the name.
	 */
	void setName(const QString& name);

private:

	/*! Name of the Choice element
	 * MANDATORY spirit:name
	 */
	QString choiceName_;

	/*! List of enumerations attached to the choice.
	 * MANDATORY spirit:enumeration
	 */
	QList<QSharedPointer<Enumeration> > enumerations_;

};

#endif /* CHOICE_H_ */
