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