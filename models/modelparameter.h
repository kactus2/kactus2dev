/* 
 *
 *  Created on: 2.8.2010
 *      Author: Antti Kamppi
 */

#ifndef MODELPARAMETER_H_
#define MODELPARAMETER_H_

#include "generaldeclarations.h"

#include <QDomNode>
#include <QString>
#include <QMap>
#include <QXmlStreamWriter>

/*! \brief Equals the spirit:modelParameter element in IP-Xact specification.
 *
 * ModelParameter desribes the properties for a single parameter that is
 * applied to all the models specified under the model/views element.
 */
class ModelParameter {

public:

	/*! \brief The constructor
	 *
	 * \param modelParameterNode A reference to a QDomNode to parse the
	 * information from.
	 *
	 * Exception guarantee: basic
	 * \exception Parse_error Occurs when a mandatory element is missing in
	 * this class or one of it's member classes.
	 */
	ModelParameter(QDomNode &modelParameterNode);

	/*! \brief The default constructor
	 *
	 * Constructs an empty model parameter that is not valid
	*/
	ModelParameter();

	/*! \brief The copy constructor
	 * 
	 */
	ModelParameter(const ModelParameter &other);

	//! \brief Assignment operator
	ModelParameter &operator=(const ModelParameter &other);

	/*! \brief The destructor
	 *
	 */
	~ModelParameter();

	/*! \brief Get the attributes linked to the model parameter element
	 *
	 * \return QMap containing pointers to the attributes
	 */
	const QMap<QString, QString>& getAttributes();

	/*! \brief Set the attributes for the model parameter
	*
	* Calling this function will delete the old attributes
	*
	* \param attributes A QMap containing pointers to the attributes
	*/
	void setAttributes(const QMap<QString, QString> &attributes);

	/*! \brief Get the name of the model parameter
	 *
	 * \return QString containing the name
	 */
	QString getName() const;

	/*! \brief Set the name for the model parameter
	*
	* \param name QString containing the name.
	*/
	void setName(const QString &name);

	/*! \brief Get the display name of the model parameter.
	 *
	 *
	 * \return QString contains the display name.
	*/
	QString getDisplayName() const;

	/*! \brief Set the display name for the model parameter.
	 *
	 * \param displayName The display name to set.
	 *
	*/
	void setDisplayName(const QString& displayName);

	/*! \brief Get the description of the model parameter.
	 *
	 *
	 * \return QString contains the description of the model parameter.
	*/
	QString getDescription() const;

	/*! \brief Set the description for the model parameter.
	 *
	 * \param description contains the description to set.
	 *
	*/
	void setDescription(const QString& description);

	/*! \brief Get the value of the model parameter
	 *
	 * \return QString containing the value
	 */
	QString getValue() const;

	/*! \brief Set the value of the model parameter
	*
	* \param value QString containing the value
	*/
	void setValue(const QString &value);

	/*! \brief Get the data type of the model parameter
	 *
	 * \return QString containing the data type
	*/
	QString getDataType() const;

	/*! \brief Set the data type of the model parameter
	 *
	 * \param dataType QString containing the data type
	 *
	*/
	void setDataType(const QString& dataType);

	/*! \brief Get the usage type of the model parameter
	 *
	 *
	 * \return QString containing the usage type
	*/
	QString getUsageType() const;

	/*! \brief Set the usage type of the model parameter
	 *
	 * \param usageType QString containing the usage type
	 *
	*/
	void setUsageType(const QString& usageType);

	/*! \brief Get the attributes linked to the value element in model parameter
	 *
	 * \return QMap containing pointers to the value attributes
	 */
	const QMap<QString, QString>& getValueAttributes();

	/*! \brief Set the attributes linked to the value element
	 *
	 * Calling this function will delete the old attributes linked to the
	 * value element
	 *
	 * \param valueAttributes QMap containing pointers to the attributes
	 */
	void setValueAttributes(const QMap<QString, QString> &valueAttributes);

	/*! \brief Checks if the model parameter is valid IP-Xact.
	 *
	 * \return True if the model parameter is in valid state.
	*/
	bool isValid() const;

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

private:

	//! \brief Contains the name, display name and description of model parameter.
	General::NameGroup nameGroup_;

	/*!
	 * MANDATORY
	 * Contains the actual value of the model parameter
	 */
	QString value_;

	/*!
	 * OPTIONAL
	 * Contains the attributes for the model parameter
	 */
	QMap<QString, QString> attributes_;

	/*!
	 * OPTIONAL
	 * Contains the attributes for the value element
	 */
	QMap<QString, QString> valueAttributes_;
};


#endif /* MODELPARAMETER_H_ */
