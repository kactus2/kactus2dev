//-----------------------------------------------------------------------------
// File: BSPBuildCommand.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 27.03.2013
//
// Description:
// Specifies the command to build a board support package for CPU components.
//-----------------------------------------------------------------------------

#ifndef BSPBUILDCOMMAND_H_
#define BSPBUILDCOMMAND_H_

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QDomNode>
#include <QXmlStreamWriter>
#include <QStringList>
#include <QString>

//-----------------------------------------------------------------------------
//! Specifies the command to build a board support package for CPU components.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT BSPBuildCommand
{
public:

	/*! The constructor.
	 *
	 *      @param [in] bspNode The node to parse the information from.
	 */
	BSPBuildCommand(QDomNode const& bspNode);

	/*! The default constructor.
	 *
	 */
	BSPBuildCommand();

	//! Copy constructor.
	BSPBuildCommand(const BSPBuildCommand& other);

	//! Assignment operator.
	BSPBuildCommand& operator=(const BSPBuildCommand& other);

	//! The destructor.
	virtual ~BSPBuildCommand();

	/*! Write the contents of the class using the writer.
	*
	*      @param [in] writer   The writer to write the document into XML.
	*/
	void write(QXmlStreamWriter& writer);

	/*! Check if the build command is in a valid state.
	*
	*      @param [in] cpuNames List of the names of the CPU elements of the containing component.
	*      @param [in] errorList The list to add the possible error messages to.
	*      @param [in] parentIdentifier String from parent to help to identify the location of the error.
	*
	*      @return bool True if the state is valid and writing is possible.
	*/
	bool isValid(const QStringList& cpuNames, QVector<QString>& errorList, const QString& parentIdentifier) const;

	/*! Check if the build command is in a valid state.
	 *
	 *      @return bool True if the state is valid and writing is possible.
	 */
	bool isValid(const QStringList& cpuNames) const;

	/*! Check if there is anything specified for the BSP build command.
	 *
	 *      @return True if at least some element has a value.
	 */
	bool isEmpty() const;

	//! Get the file type of the BSP build command.
	QString getFileType() const;

	/*! Set the file type for the BSP build command.
	 *
	 *      @param [in] fileType The file type to set.
	 */
	void setFileType(const QString& fileType);

	//! Get the build command of the BSP command.
	QString getCommand() const;

	/*! Set the command for the BSP build command.
	 *
	 *      @param [in] command The build command to set.
	 *
	*/
	void setCommand(const QString& command);

	//! Get the arguments of the BSP build command.
	QString getArguments() const;

	/*! Set the arguments for the BSP build command.
	 *
	 *      @param [in] args The arguments to set.
	*/
	void setArguments(const QString& args);

	//! Get the CPU name of the BSP build command.
	QString getCPUName() const;

	/*! Set the CPU name for the BSP build command.
	 *
	 *      @param [in] cpuName The CPU name to set.
	*/
	void setCPUName(const QString& cpuName);

private:

	//! The file type for the BSP file.
	QString fileType_;

	//! The command used to build the BSP
	QString command_;

	//! The arguments for the build command
	QString arguments_;

	//! The CPU name for the command
	QString cpuName_;
};

#endif /* BSPBUILDCOMMAND_H_ */
