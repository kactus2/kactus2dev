/* 
 *
 *  Created on: 27.7.2010
 *      Author: Antti Kamppi
 */

#ifndef PARAMETER_H_
#define PARAMETER_H_

#include <QDomNode>
#include <QString>
#include <QMap>
#include <QXmlStreamWriter>

/*! \brief Equals the spirit:parameter element in IP-Xact specification
 *
 * Parameter defines a configurable element related to the containing element.
 */
class Parameter {

public:

	/*! \brief The constructor
	 *
	 * \param parameterNode A reference to the QDomNode to parse the information
	 * from.
	 *
	 * Exception guarantee: basic
	 * \exception Parse_error Occurs when a mandatory element is missing in
	 * this class or one of it's member classes.
	 */
	Parameter(QDomNode &parameterNode);

	/*! \brief The default constructor
	 *
	 * Constructs an empty invalid parameter.
	*/
	Parameter();

	//! \brief Copy constructor
	Parameter(const Parameter &other);

	//! \brief Assignment operator
	Parameter &operator=(const Parameter &other);

	/*! \brief The destructor
	 *
	 */
	~Parameter();

	/*! \brief Get the attributes for the parameter
	 *
	 * \return QMap containing pointers to the attributes
	 */
	const QMap<QString, QString>& getValueAttributes();

	/*! \brief Get the name of the parameter
	 *
	 * \return QString containing the name
	 */
	QString getName() const;

	/*! \brief Get the value of the parameter
	 *
	 * \return QString containing the value
	 */
	QString getValue() const;

	/*! \brief Get the description of the parameter.
	 *
	 * \return QString containing the description.
	 */
	QString getDescription() const;

	/*! \brief Set the description for the parameter.
	 *
	 * \param description QString containing the description.
	 */
	void setDescription(const QString& description);

	/*! \brief Set the attributes for the parameter
	 *
	 * Calling this function will delete old attributes for the parameter.
	 *
	 * \param attributes QMap containing the attributes
	 */
	void setValueAttributes(const QMap<QString, QString> &attributes);

	/*! \brief Set the name for the parameter
	 *
	 * \param name QString containing the name
	 */
	void setName(const QString &name);

	/*! \brief Set the value for the parameter
	 *
	 * \param value QString containing the value
	 */
	void setValue(const QString &value);

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

	/*! \brief Check if the Parameter is in valid state or not.
	 *
	 * \return True if the parameter is valid.
	*/
	bool isValid() const;

private:

	/*! \brief Identifies the parameter
	 * MANDATORY
	 */
	QString name_;

	/*! \brief  The actual value of the parameter
	 * MANDATORY
	 */
	QString value_;

	//! \brief The attributes for the value
	QMap<QString, QString> valueAttributes_;

	//! \brief The description for the parameters
	QString description_;
};

#endif /* PARAMETER_H_ */
