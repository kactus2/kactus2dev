/* 
 *
 *  Created on: 9.8.2010
 *      Author: Antti Kamppi
 */

#ifndef BUILDCOMMAND_H_
#define BUILDCOMMAND_H_

#include "buildmodel.h"

#include "ipxactmodels_global.h"

#include <QString>
#include <QMap>
#include <QDomNode>
#include <QXmlStreamWriter>

struct AttributePair;

/*! \brief Equals the spirit:buildCommand element in IP-Xact specification
 *
 * BuildCommand contains flags or commands for building the containing source
 * file.
 */
class IPXACTMODELS_EXPORT BuildCommand : public BuildModel {

public:

	/*! \brief The constructor
	 *
	 * \param buildCommandNode A reference to a QDomNode to parse the
	 * information from.
	 *
	 * Exception guarantee: basic
	 * \exception Parse_error Occurs when a mandatory element is missing in
	 * this class or one of it's member classes.
	 */
	BuildCommand(QDomNode &buildCommandNode);

	/*! \brief The default constructor.
	 *
	*/
	BuildCommand();

	//! \brief The copy constructor
	BuildCommand(const BuildCommand &other);

	//! \brief The assignment operator
	BuildCommand &operator=(const BuildCommand &other);

	/*! \brief The destructor
	 *
	 */
	virtual ~BuildCommand();

	/*! \brief Write the contents of the class using the writer.
	*
	* Uses the specified writer to write the class contents into file as valid
	* IP-Xact.
	*
	* \param writer A reference to a QXmlStreamWrite instance that is used to
	* write the document into file.
	*/
	void write(QXmlStreamWriter& writer);

	/*! \brief Get the target name
	 *
	 * \return QString containing the target name.
	 */
	QString getTargetName() const;

	/*! \brief Get the attributes for the target name-element.
	 *
	 * \return QMap containing the attributes.
	 */
	const QMap<QString, QString>& getTargetNameAttributes();

	/*! \brief Set the target name
	 *
	 * \param targetName QString containing the name.
	 */
	void setTargetName(const QString &targetName);

	/*! \brief Set the attributes for the targetName-element
	 *
	 * Calling this fuction will delete the old attributes.
	 *
	 * \param targetNameAttributes QMap containing the new attributes.
	 */
	void setTargetNameAttributes(
			const QMap<QString, QString> &targetNameAttributes);

private:

	/*! \brief Defines the path to the file derived from the source file.
	 * OPTIONAL spirit:targetName
	 */
	QString targetName_;

	/*! \brief Contains the attributes for the targetName element.
	 * OPTIONAL spirit:targetName
	 */
	QMap<QString, QString> targetNameAttributes_;
};

#endif /* BUILDCOMMAND_H_ */
