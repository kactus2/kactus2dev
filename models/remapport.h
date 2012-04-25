/* 
 *
 *  Created on: 30.7.2010
 *      Author: Antti Kamppi
 */

#ifndef REMAPPORT_H_
#define REMAPPORT_H_

#include "generaldeclarations.h"

#include <QString>
#include <QList>
#include <QDomNode>
#include <QXmlStreamWriter>

/*! \brief Equals to the spirit:remapPort element in IP-Xact specification
 *
 * Specifies when the remap state gets effective. A collection of remapPort
 * instances make up the condition for the containing remap state.
 */
class RemapPort {

public:

	/*! \brief The constructor
	 *
	 * \param remapPortNode A reference to a QDomNode to parse the information
	 * from.
	 *
	 * Exception guarantee: basic
	 * \exception Parse_error Occurs when a mandatory element is missing in
	 * this class or one of it's member classes.
	 */
	RemapPort(QDomNode &remapPortNode);

	//! \brief Copy constructor
	RemapPort(const RemapPort &other);

	//! \brief Assignment operator
	RemapPort &operator=(const RemapPort &other);

	/*! \brief The destructor
	 *
	 */
	~RemapPort();

	/*! \brief Write the contents of the class using the writer.
	*
	* Uses the specified writer to write the class contents into file as valid
	* IP-Xact.
	*
	* \param writer A reference to a QXmlStreamWriter instance that is used to
	* write the document into file.
	*/
	void write(QXmlStreamWriter& writer);

	/*! \brief Check if the remap port is in a valid state.
	 *
	 * \param portNames List containing the names of the ports of the component.
	 * \param errorList The list to add the possible error messages to.
	 * \param parentIdentifier String from parent to help to identify the location of the error.
	 *
	 * \return bool True if the state is valid and writing is possible.
	*/
	bool isValid(const QStringList& portNames,
		QStringList& errorList, 
		const QString& parentIdentifier) const;

	/*! \brief Check if the remap port is in a valid state.
	 * 
	 * \param portNames List containing the names of the ports of the component.
	 * 
	 * \return bool True if the state is valid and writing is possible.
	*/
	bool isValid(const QStringList& portNames) const;

	/*! \brief Get the value of the port name ref element
	 *
	 * \return QString containing the portNameRef
	 */
	QString getPortNameRef() const;

	/*! \brief Get the value of the remapPort element
	 *
	 * \return QString containing the value.
	 */
	QString getValue() const;

	/*! \brief Set the portNameRef for this remapPort
	 *
	 * \param portNameRef QString containing the portNameRef
	 */
	void setPortNameRef(const QString &portNameRef);

	/*! \brief Set the value for the remapPort element
	 *
	 * \param value QString containing the value
	 */
	void setValue(const QString &value);

	/*! \brief Get index of a port
	 *
	 * \return unsigned int describing the port index
	 */
	int getPortIndex() const;

	/*! \brief Set the port index
	 *
	 * \param portIndex unsigned int describing the port index. If port index
	 * was not defined then -1 is returned
	 */
    void setPortIndex(int portIndex);

private:

	/*!
	 * MANDATORY spirit:remapPort
	 * Logical value of the single port bit
	 * This value is currently in QString format because the value may be
	 * hexadecimal format and the conversion is not automatically guaranteed.
	 */
	QString value_;

	/*!
	 * MANDATORY spirit:portNameRef
	 * The name of the port in the containing description for which this logic
	 * value comparison is assigned.
	 */
	QString portNameRef_;

	/*!
	 * OPTIONAL spirit:portIndex
	 * References the index of a port in the containing description when the
	 * port being referenced is being vectored.
	 */
	int portIndex_;
};

#endif /* REMAPPORT_H_ */
