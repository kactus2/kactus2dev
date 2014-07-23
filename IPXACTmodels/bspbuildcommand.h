/* 
 *	Created on:	27.3.2013
 *	Author:		Antti Kamppi
 *	File name:	bspbuildcommand.h
 *	Project:		Kactus 2
*/

#ifndef BSPBUILDCOMMAND_H_
#define BSPBUILDCOMMAND_H_

#include "ipxactmodels_global.h"

#include <QDomNode>
#include <QXmlStreamWriter>
#include <QStringList>
#include <QString>

/*! \brief Equals the kactus2:BSPBuildCommand in Kactus2 extensions.
 * 
 * Specifies the command to build a board support package for CPU components.
 */
class IPXACTMODELS_EXPORT BSPBuildCommand {

public:

	/*! \brief The constructor.
	 *
	 * Method: 		BSPBuildCommand
	 * Full name:	BSPBuildCommand::BSPBuildCommand
	 * Access:		public 
	 *
	 * \param bspNode The node to parse the information from.
	 *
	*/
	BSPBuildCommand(QDomNode& bspNode);

	/*! \brief The default constructor.
	 *
	 * Method: 		BSPBuildCommand
	 * Full name:	BSPBuildCommand::BSPBuildCommand
	 * Access:		public 
	 *
	*/
	BSPBuildCommand();

	//! \brief Copy constructor.
	BSPBuildCommand(const BSPBuildCommand& other);

	//! \brief Assignment operator.
	BSPBuildCommand& operator=(const BSPBuildCommand& other);

	//! \brief The destructor.
	virtual ~BSPBuildCommand();

	/*! \brief Write the contents of the class using the writer.
	*
	* Uses the specified writer to write the class contents into file as valid
	* IP-Xact.
	*
	* \param writer A reference to a QXmlStreamWriter instance that is used to
	* write the document into file.
	*/
	void write(QXmlStreamWriter& writer);

	/*! \brief Check if the build command is in a valid state.
	*
	* \param cpuNames List of the names of the CPU elements of the containing component.
	* \param errorList The list to add the possible error messages to.
	* \param parentIdentifier String from parent to help to identify the location of the error.
	*
	* \return bool True if the state is valid and writing is possible.
	*/
	bool isValid(const QStringList& cpuNames, QStringList& errorList, const QString& parentIdentifier) const;

	/*! \brief Check if the build command is in a valid state.
	 *
	 * \return bool True if the state is valid and writing is possible.
	*/
	bool isValid(const QStringList& cpuNames) const;

	/*! \brief Check if there is anything specified for the BSP build command.
	 *
	 * Method: 		isEmpty
	 * Full name:	BSPBuildCommand::isEmpty
	 * Access:		public 
	 *
	 *
	 * \return True if at least some element has a value.
	*/
	bool isEmpty() const;

	//! \brief Get the file type of the BSP build command.
	QString getFileType() const;

	/*! \brief Set the file type for the BSP build command.
	 *
	 * Method: 		setFileType
	 * Full name:	BSPBuildCommand::setFileType
	 * Access:		public 
	 *
	 * \param fileType The file type to set.
	 *
	*/
	void setFileType(const QString& fileType);

	//! \brief Get the build command of the BSP command.
	QString getCommand() const;

	/*! \brief Set the command for the BSP build command.
	 *
	 * Method: 		setCommand
	 * Full name:	BSPBuildCommand::setCommand
	 * Access:		public 
	 *
	 * \param command The build command to set.
	 *
	*/
	void setCommand(const QString& command);

	//! \brief Get the arguments of the BSP build command.
	QString getArguments() const;

	/*! \brief Set the arguments for the BSP build command.
	 *
	 * Method: 		setArguments
	 * Full name:	BSPBuildCommand::setArguments
	 * Access:		public 
	 *
	 * \param args The arguments to set.
	 *
	*/
	void setArguments(const QString& args);

	//! \brief Get the CPU name of the BSP build command.
	QString getCPUName() const;

	/*! \brief Set the CPU name for the BSP build command.
	 *
	 * Method: 		setCPUName
	 * Full name:	BSPBuildCommand::setCPUName
	 * Access:		public 
	 *
	 * \param cpuName The CPU name to set.
	 *
	*/
	void setCPUName(const QString& cpuName);

private:

	//! \brief The file type for the BSP file.
	QString fileType_;

	//! \brief The command used to build the BSP
	QString command_;

	//! \brief The arguments for the build command
	QString arguments_;

	//! \brief The CPU name for the command
	QString cpuName_;
};

#endif /* BSPBUILDCOMMAND_H_ */
