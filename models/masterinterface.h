/* 
 *
 *  Created on: 31.8.2010
 *      Author: Antti Kamppi
 */

#ifndef MASTERINTERFACE_H_
#define MASTERINTERFACE_H_

#include <QString>
#include <QMap>
#include <QDomNode>
#include <QXmlStreamWriter>

/*! \brief Equals the spirit:master element in IP-Xact specification
 *
 *	This class also equals the spirit:mirroredMaster element.
 * Contains the elements that define the master interface.
 */
class MasterInterface {

public:

	/*! \brief The constructor
	 *
	 * \param masterNode A reference to a QDomNode to parse the information
	 * from.
	 *
	 * Exception guarantee: basic
	 * \exception Parse_error Occurs when a mandatory element is missing in
	 * this class or one of it's member classes.
	 */
	MasterInterface(QDomNode& masterNode);

	/*! \brief Default constructor
	 *
	*/
	MasterInterface();

	//! \brief Copy constructor
	MasterInterface(const MasterInterface& other);

	//! \brief Assignment operator
	MasterInterface& operator=(const MasterInterface& other);

	/*! \brief The destructor
	 *
	 */
	~MasterInterface();

	/*! \brief Write the contents of the class using the writer.
	*
	* Uses the specified writer to write the class contents into file as valid
	* IP-Xact.
	*
	* \param writer A reference to a QXmlStreamWriter instance that is used to
	* write the document into file.
	*
	* Exception guarantee: basic
	* \exception Write_error Occurs if class or one of it's member classes is
	* not valid IP-Xact in the moment of writing.
	*/
	void write(QXmlStreamWriter& writer);

	/*! \brief Get the address space reference
	 *
	 * \return QString containing the address space name
	 */
	QString getAddressSpaceRef() const;

	/*! \brief Get the base address
	 *
	 * \return QString containing the base address
	 */
	QString getBaseAddress() const;

	/*! \brief Get the base address attributes
	 *
	 * \return QMap containing the attributes for the base address element
	 */
	const QMap<QString,QString>& getBaseAttributes();

	/*! \brief Get the prompt attribute
	 *
	 * \return QString containing the value of the attribute
	 */
	QString getPrompt() const;

	/*! \brief Set the address space reference
	 *
	 * \param addressSpaceRef A reference to a QString containing the name of
	 * the address space
	 */
	void setAddressSpaceRef(const QString& addressSpaceRef);

	/*! \brief Set the base address
	 *
	 * \param  baseAddress Q reference to a QString containing the base
	 * address.
	 */
	void setBaseAddress(const QString& baseAddress);

	/*! \brief Set the base address attributes
	 *
	 * \param baseAttributes A reference to a QMap containing the attributes
	 * for the base address element.
	 */
	void setBaseAttributes(const QMap<QString,QString>& baseAttributes);

	/*! \brief Set the prompt attribute value
	 *
	 * \param prompt A reference to a QString containing the value for the
	 * prompt attribute.
	 */
	void setPrompt(const QString& prompt);

private:

	/*! \brief MANDATORY spirit:addressSpaceRef
	 * References a name of an address space that defines the range and width
	 * for transaction on this interface.
	 */
	QString addressSpaceRef_;

	/*! \brief OPTIONAL spirit:baseAddress
	 * Specifies the starting address of the address space.
	 */
	QString baseAddress_;

	/*! \brief OPTIONAL attribute spirit:prompt
	 * Allows the setting of a string for the configuration.
	 */
	QString prompt_;

	/*! \brief OPTIONAL attributes for the spirit:baseAddress
	 * key = attribute name
	 * value = attribute value
	 */
	QMap<QString, QString> baseAttributes_;
};

#endif /* MASTERINTERFACE_H_ */
