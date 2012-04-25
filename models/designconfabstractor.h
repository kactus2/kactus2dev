/* 
 *
 *  Created on: 30.8.2010
 *      Author: Antti Kamppi
 */

#ifndef DESIGNCONFABSTRACTOR_H_
#define DESIGNCONFABSTRACTOR_H_

#include <LibraryManager/vlnv.h>

#include <QString>
#include <QMap>
#include <QSharedPointer>
#include <QDomNode>
#include <QXmlStreamWriter>

class VLNV;

/*! \brief Equals the spirit:abstractor element in IP-Xact specification
 *
 * This element is used inside DesignConfiguration's
 * interconectionConfiguration element.
 */
class DesignConfAbstractor {

public:

	/*! \brief The constructor
	 *
	 * \param abstractorNode A reference to a QDomNode to parse the
	 * information from.
	 *
	 * Exception guarantee: basic
	 * \exception Parse_error Occurs when a mandatory element is missing in
	 * this class or one of it's member classes.
	 */
	DesignConfAbstractor(QDomNode& abstractorNode);

	//! \brief Copy constructor
	DesignConfAbstractor(const DesignConfAbstractor& other);

	//! \brief Assignment operator
	DesignConfAbstractor& operator=(const DesignConfAbstractor& other);

	/*! \brief The destructor
	 *
	 */
	~DesignConfAbstractor();

	/*! \brief Write the contents of the class using the writer.
	*
	* Uses the specified writer to write the class contents into file as valid
	* IP-Xact.
	*
	* \param writer A reference to a QXmlStreamWrite instance that is used to
	* write the document into file.
	*/
	void write(QXmlStreamWriter& writer);

	/*! \brief Get the VLNV tag specifying the abstractor
	 *
	 * \return A pointer to the VLNV tag identifying the abstractor for the
	 * containing design configuration.
	 */
	VLNV getAbstractorRef() const;

	/*! \brief Get the configurable elements for this abstractor
	 *
	 * \return QMap containing the id-value pairs of configurable elements
	 */
	const QMap<QString,QString>& getConfigurableElements();

	/*! \brief Get the description
	 *
	 * \return QString containing the description
	 */
	QString getDescription() const;

	/*! \brief Get the display name
	 *
	 * \return QString containing the display name.
	 */
	QString getDisplayName() const;

	/*! \brief Get the instance name
	 *
	 * \return QString containing the instance name.
	 */
	QString getInstanceName() const;

	/*! \brief Get the current view for the selected abstractor instance.
	 *
	 * \return QString containing the view name.
	 */
	QString getViewName() const;

	/*! \brief Set the abstractor
	 *
	 * \param abstractorRef A VLNV tag identifying the abstractor for the
	 * containing design configuration.
	 */
	void setAbstractorRef(const VLNV& abstractorRef);

	/*! \brief Set the configurable elements
	 *
	 * \param configurableElements A reference to a QMap containing the
	 * id-value pairs of the configurable elements.
	 */
	void setConfigurableElements(QMap<QString,QString>& configurableElements);

	/*! \brief Set the description
	 *
	 * \param description A reference to a QString containing the description
	 */
	void setDescription(const QString& description);

	/*! \brief Set the display name
	 *
	 * \param displayName A reference to a QString containing the display name
	 */
	void setDisplayName(const QString& displayName);

	/*! \brief Set the instance name
	 *
	 * \param instanceName A reference to a QString containing the instance
	 * name.
	 */
	void setInstanceName(const QString& instanceName);

	/*! \brief Set the view name
	 *
	 * \param viewName A reference to a QString containing the view name.
	 */
	void setViewName(const QString& viewName);

private:

	/*! \brief MANDATORY spirit:instanceName
	 * A unique name for this instance of the abstractor
	 */
	QString instanceName_;

	/*! \brief OPTIONAL spirit:displayName
	 * descriptive text associated with the instance.
	 */
	QString displayName_;

	/*! \brief OPTIONAL spirit:description
	 * Textual description of the instance.
	 */
	QString description_;

	/*! \brief OPTIONAL spirit:abstractorRef
	 * VLNV tag identifying an abstraction for this configuration.
	 */
	VLNV abstractorRef_;

	/*! \brief OPTIONAL spirit:configurableElementValues
	 *
	 * QMap containing the abstractor instances configurable parameter values.
	 * Key = spirit:referenceId
	 * value = configurableElementValue
	 */
	QMap<QString, QString> configurableElements_;

	/*! \brief MANDATORY spirit:viewName
	 * Defines the current valid view for the selected abstractor instance.
	 */
	QString viewName_;
};

#endif /* DESIGNCONFABSTRACTOR_H_ */
