/* 
 *
 *  Created on: 10.8.2010
 *      Author: Antti Kamppi
 */

#ifndef BUILDMODEL_H_
#define BUILDMODEL_H_

#include <common/Global.h>

#include <QMap>
#include <QString>
#include <QDomNode>
#include <QXmlStreamWriter>

/*! \brief Acts as a base class for FileBuilder & BuildCommand classes.
 *
 * There is no use for BuildModel instances, this class is meant to be
 * purely a base class for generalization.
 */
class KACTUS2_API BuildModel {

public:

	/*! \brief The constructor
	 *
	 * \param node A reference to a QDomNode to parse the information from.
	 *
	 * Exception guarantee: basic
	 * \exception Parse_error Occurs when a mandatory element is missing in
	 * this class or one of it's member classes.
	 */
	BuildModel(QDomNode &node);

	/*! \brief The default constructor
	 *
	*/
	BuildModel();

	//! \brief The copy constructor
	BuildModel(const BuildModel &other);

	//! \brief The assignment operator
	BuildModel &operator=(const BuildModel &other);

	/*! \brief The destructor
	 *
	 */
	virtual ~BuildModel();

	/*! \brief Write the contents of the class using the writer.
	*
	* Uses the specified writer to write the class contents into file as valid
	* IP-Xact.
	*
	* \param writer A reference to a QXmlStreamWrite instance that is used to
	* write the document into file.
	*/
	void write(QXmlStreamWriter& writer);

	/*! \brief Get the command
	 *
	 * \return QString containing the command
	 */
	virtual QString getCommand() const;

	/*! \brief Get the attributes for the command-element
	 *
	 * \return QMap containing pointers to the attributes.
	 */
	virtual const QMap<QString, QString>& getCommandAttributes();

	/*! \brief Get the attributes for the replaceDefaultFlags-element.
	 *
	 * \return QMap containing pointers to the attributes.
	 */
	virtual const QMap<QString, QString>& getDefaultFlagAttributes();

	/*! \brief Get the attributes for the flags-element.
	 *
	 * \return QMap containing pointers to the attributes.
	 */
	virtual const QMap<QString, QString>& getFlagAttributes();

	/*! \brief Get the flags
	 *
	 * \return QString containing the flags
	 */
	virtual QString getFlags() const;

	/*! \brief Get the replaceDefaultFlags value
	 *
	 * \return boolean value.
	 */
	virtual bool getReplaceDefaultFlags() const;

	/*! \brief Set the command
	 *
	 * \param QString containing the command
	 */
	virtual void setCommand(const QString &command);

	/*! \brief Set the command attributes
	 *
	 * Calling this function will delete the old attributes
	 *
	 * \param commandAttributes QMap containing pointers to the new attributes.
	 */
	virtual void setCommandAttributes(
			const QMap<QString, QString> &commandAttributes);

	/*! \brief Set the replaceDefaultFlags attributes
	 *
	 * Calling this function will delete the old attributes
	 *
	 * \param defaultFlagAttributes QMap containing pointers to the attributes.
	 */
	virtual void setDefaultFlagAttributes(
			const QMap<QString, QString> &defaultFlagAttributes);

	/*! \brief Set the flag attributes
	 *
	 * Calling this function will delete the old attributes
	 *
	 * \param flagAttributes QMap containing pointers to the new attributes.
	 */
	virtual void setFlagAttributes(
			const QMap<QString, QString> &flagAttributes);

	/*! \brief Set the flags
	 *
	 * \param flags QString containing the flags.
	 */
	virtual void setFlags(const QString &flags);

	/*! \brief Set the replaceDefaultFlags value
	 *
	 * \param replaceDefaultFlags boolean value.
	 */
	virtual void setReplaceDefaultFlags(bool replaceDefaultFlags);

protected:

	/*! \brief Defines a compiler or assembler tool that processes files.
	 * OPTIONAL spirit:command
	 */
	QString command_;

	/*! \brief Contains attributes for the command-element.
	 * OPTIONAL
	 */
	QMap<QString, QString> commandAttributes_;

	/*! \brief Documents any flags to be passed along with the software tool.
	 * OPTIONAL spirit:flags
	 */
	QString flags_;

	/*! \brief Contains attributes for the flag-element.
	 * OPTIONAL
	 */
	QMap<QString, QString> flagAttributes_;

	/*! \brief Specifies if the flags for generator are replaces of appended.
	 *
	 * OPTIONAL spirit:replaceDefaultFlags
	 * When true, documents flags that replace any of the default flags from
	 * the build script generator. If false, the flags are appended.
	 */
	bool replaceDefaultFlags_;

	/*! \brief Contains the attributes for the replaceDefaultFlags element.
	 * OPTIONAL
	 */
	QMap<QString, QString> defaultFlagAttributes_;
};

#endif /* BUILDMODEL_H_ */
