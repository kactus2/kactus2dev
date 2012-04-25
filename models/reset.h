/* 
 *
 *  Created on: 21.10.2010
 *      Author: Antti Kamppi
 */

#ifndef RESET_H_
#define RESET_H_

#include <QDomNode>
#include <QList>
#include <QMap>
#include <QXmlStreamWriter>
#include <QString>

/*! \brief Equals the spirit:reset element in IP-Xact specification.
 *
 * This class describes the value of a register at reset.
 */
class Reset {

public:

	/*! \brief The destructor
	 *
	 * \param resetNode A reference to a QDomNode to parse the information from.
	 *
	 * Exception guarantee: basic
	 * \exception Parse_error Occurs when a mandatory element is missing in
	 * this class or one of it's member classes.
	 */
	Reset(QDomNode& resetNode);

	//! \brief Copy constructor
	Reset(const Reset& other);

	//! \brief Assignment operator
	Reset& operator=(const Reset& other);

	/*! \brief the destructor
	 *
	 */
	virtual ~Reset();

	/*! \brief Write the contents of the class using the writer.
	*
	* Uses the specified writer to write the class contents into file as valid
	* IP-Xact.
	*
	* \param writer A reference to a QXmlStreamWriter instance that is used to
	* write the document into file.
	*/
	void write(QXmlStreamWriter& writer);

	/*! \brief Check if the reset is in a valid state.
	 *
	 * \param errorList The list to add the possible error messages to.
	 * \param parentIdentifier String from parent to help to identify the location of the error.
	 *
	 * \return bool True if the state is valid and writing is possible.
	*/
	bool isValid(QStringList& errorList, 
		const QString& parentIdentifier) const;

	/*! \brief Check if the reset is in a valid state.
	 *
	 * \return bool True if the state is valid and writing is possible.
	*/
	bool isValid() const;

	/*! \brief Get the mask.
	 *
	 * \return The mask bits.
	 */
	int getMask() const;

	/*! \brief Get the attributes for the mask-element.
	 *
	 * \return QMap containing the attributes.
	 */
	const QMap<QString,QString>& getMaskAttributes() const;

	/*! \brief Get the value of the register in reset.
	 *
	 * \return the value.
	 */
	int getValue() const;

	/*! \brief Get the attributes for the value-element.
	 *
	 * \return QMap containing the attributes.
	 */
	const QMap<QString,QString>& getValueAttributes() const;

	/*! \brief Set the mask bits.
	 *
	 * \param mask The new mask bits.
	 */
	void setMask(int mask);

	/*! \brief Set attributes for the mask element.
	 *
	 * \param maskAttributes QMap containing the attributes.
	 */
	void setMaskAttributes(const QMap<QString,QString>& maskAttributes);

	/*! \brief Set the value of the register in reset.
	 *
	 * \param value The value.
	 */
	void setValue(int value);

	/*! \brief Set the attributes for the value-element.
	 *
	 * \param valueAttributes QMap containing the attributes.
	 */
	void setValueAttributes(const QMap<QString,QString>& valueAttributes);

private:

	/*! \brief The value that is assigned at reset.
	 * MANDATORY spirit:value
	 */
	int value_;

	/*! \brief The attributes for the value-element.
	 * OPTIONAL
	 */
	QMap<QString, QString> valueAttributes_;

	/*! \brief Mask that defines which bits have defined reset-value.
	 * OPTIONAL spirit:mask
	 */
	int mask_;

	/*! \brief The attributes for the mask-element.
	 * OPTIONAL
	 */
	QMap<QString, QString> maskAttributes_;
};

#endif /* RESET_H_ */
