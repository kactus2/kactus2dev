/* 
 *	Created on:	26.3.2013
 *	Author:		Antti Kamppi
 *	File name:	swbuildcommand.h
 *	Project:		Kactus 2
*/

#ifndef SWBUILDCOMMAND_H_
#define SWBUILDCOMMAND_H_

#include <common/Global.h>

#include <QDomNode>
#include <QXmlStreamWriter>
#include <QStringList>
#include <QString>

/*! \brief Equals the kactus2:SWBuildCommand element in Kactus2 extensions.
 * 
 * Specifies the build commands for software objects.
 */
class KACTUS2_API SWBuildCommand {

public:

	/*! \brief The constructor
	 *
	 * Method: 		SWBuildCommand
	 * Full name:	SWBuildCommand::SWBuildCommand
	 * Access:		public 
	 *
	 * \param swBuildNode Reference to the QDomNode to parse the information from.
	 *
	*/
	SWBuildCommand(QDomNode& swBuildNode);

	/*! \brief The default constructor
	 *
	 * Method: 		SWBuildCommand
	 * Full name:	SWBuildCommand::SWBuildCommand
	 * Access:		public 
	 *
	 *
	*/
	SWBuildCommand();

	//! \brief Copy constructor
	SWBuildCommand(const SWBuildCommand& other);

	//! \brief Assignment operator
	SWBuildCommand& operator=(const SWBuildCommand& other);

	//! \brief The destructor
	virtual ~SWBuildCommand();

	/*! \brief Write the contents of the class using the writer.
	*
	* Uses the specified writer to write the class contents into file as valid
	* IP-Xact.
	*
	* \param writer A reference to a QXmlStreamWriter instance that is used to
	* write the document into file.
	* \param withinHWComp If true then also the command is written. If false then command is ignored.
	*/
	void write(QXmlStreamWriter& writer, bool withinHWComp);

	/*! \brief Check if the build command is in a valid state.
	*
	* \param errorList The list to add the possible error messages to.
	* \param parentIdentifier String from parent to help to identify the location of the error.
	*
	* \return bool True if the state is valid and writing is possible.
	*/
	bool isValid(QStringList& errorList, const QString& parentIdentifier) const;

	/*! \brief Check if the build command is in a valid state.
	 *
	 * \return bool True if the state is valid and writing is possible.
	*/
	bool isValid() const;

	/*! \brief Get the file type of the SW build command.
	 *
	 * Method: 		getFileType
	 * Full name:	SWBuildCommand::getFileType
	 * Access:		public 
	 *
	 *
	 * \return The file type for the command.
	*/
	QString getFileType() const;

	/*! \brief Set the file type for the command.
	 *
	 * Method: 		setFileType
	 * Full name:	SWBuildCommand::setFileType
	 * Access:		public 
	 *
	 * \param fileType The file type to set.
	 *
	*/
	void setFileType(const QString& fileType);

	/*! \brief Get the command of the SW build command.
	 *
	 * Method: 		getCommand
	 * Full name:	SWBuildCommand::getCommand
	 * Access:		public 
	 *
	 *
	 * \return The command.
	*/
	QString getCommand() const;

	/*! \brief Set the command for the SW build command.
	 *
	 * Method: 		setCommand
	 * Full name:	SWBuildCommand::setCommand
	 * Access:		public 
	 *
	 * \param command The command to set.
	 *
	*/
	void setCommand(const QString& command);

	/*! \brief Get the flags of the SW build command.
	 *
	 * Method: 		getFlags
	 * Full name:	SWBuildCommand::getFlags
	 * Access:		public 
	 *
	 *
	 * \return The flags for the SW build command.
	*/
	QString getFlags() const;

	/*! \brief Set the flags of the SW build command.
	 *
	 * Method: 		setFlags
	 * Full name:	SWBuildCommand::setFlags
	 * Access:		public 
	 *
	 * \param flags The flags to set.
	 *
	*/
	void setFlags(const QString& flags);

	/*! \brief Check if the flags for the SW build command are replaced or appended.
	 *
	 * Method: 		getReplaceDefaultFlags
	 * Full name:	SWBuildCommand::getReplaceDefaultFlags
	 * Access:		public 
	 *
	 *
	 * \return If true then the flags are replaced, otherwise appended.
	*/
	bool getReplaceDefaultFlags() const;

	/*! \brief Set the replace default flags value for the SW build command.
	 *
	 * Method: 		setReplaceDefaultFlags
	 * Full name:	SWBuildCommand::setReplaceDefaultFlags
	 * Access:		public 
	 *
	 * \param replaceDefault The value to set.
	 *
	*/
	void setReplaceDefaultFlags(bool replaceDefault);

private:

	//! \brief File type for the command (mandatory field)
	QString fileType_;

	//! \brief The build command (optional field)
	QString command_;

	//! \brief Flags for the build command (optional field)
	QString flags_;

	//! \brief Specifies if the default flags are replaced or appended. (optional field)
	bool replaceDefault_;
};

#endif /* SWBUILDCOMMAND_H_ */