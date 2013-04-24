/* 
 *	Created on:	9.8.2010
 *	Author:		Antti Kamppi
 *	File name:	file.h
 *	Project:		Kactus 2
*/

#ifndef FILE_H_
#define FILE_H_

#include "generaldeclarations.h"
#include "buildcommand.h"

#include <common/Global.h>

#include <QString>
#include <QList>
#include <QDomNode>
#include <QSharedPointer>
#include <QMap>
#include <QXmlStreamWriter>
#include <QStringList>
#include <QSettings>

class FileSet;

/*! \brief Equals the spirit:file element in IP-Xact specification.
 *
 * The File is a reference to a file or directory.
 */
class KACTUS2_API File {

public:

	//! \brief Equals the spirit:define element in IP-Xact specification.
	struct Define {

		//! \brief Contains the nameGroup element of the define
		General::NameGroup nameGroup_;

		//! \brief The value of the define parameter.
		QString value_;

		/*! \brief The constructor
		 *
		 * \param name Name of the define
		 * \param value Value of the define parameter.
		 *
		*/
		Define(const QString name, const QString value);

		/*! \brief The constructor
		 *
		 * \param defineNode Reference to a QDomNode to parse the information
		 * from.
		 *
		*/
		Define(QDomNode& defineNode);

		/*! \brief The default constructor
		 *
		*/
		Define();

		//! \brief Copy constructor
		Define(const Define& other);

		//! \brief Assignment operator
		Define& operator=(const Define& other);

		/*! \brief Check if the define is in valid state.
		 *
		 * \param errorList The list to add possible error messages to.
		 * \param parentIdentifier Contains the string to help to identify the location of the error.
		 *
		 * \return bool True if the define is valid.
		 */
		bool isValid(QStringList& errorList, const QString& parentIdentifier) const;

		/*! \brief Check if the define is in a valid state.
		*
		* \return bool True if the state is valid and writing is possible.
		*/
		bool isValid() const;
	};

	/*! \brief The constructor
	 *
	 * \param fileNode A reference to a QDomNode to parse the information
	 * from.
	 * \param parent Pointer to the FileSet that contains this file.
	 *
	 */
	File(QDomNode &fileNode, FileSet* parent);

	/*! \brief The constructor
	 *
	 *  \param filePath The path to the file. This should be a relative path to
	 *  the containing IP-Xact xml file.
	 *  \param parent Pointer to the FileSet that contains this file.
	 */
	File(const QString filePath, FileSet* parent);

	/*! \brief The constructor
	 * 
	 * \param parent Pointer to the containing file set.
	 * 
	 * This constructor constructs a completely empty file element. This file
	 * is not in valid condition and must be edited to make it valid.
	 *
	*/
	File(FileSet* parent);

	//! \brief Copy constructor
	File(const File &other, FileSet* parent);

	//! \brief Assignment operator
	File &operator=(const File &other);

	/*! \brief The destructor
	 *
	 */
	~File();

	/*! \brief Write the contents of the class using the writer.
	*
	* Uses the specified writer to write the class contents into file as valid
	* IP-Xact.
	*
	* \param writer A reference to a QXmlStreamWriter instance that is used to
	* write the document into file.
	*/
	void write(QXmlStreamWriter& writer);

	/*! \brief Check if the file is in a valid state.
	*
	* \param errorList The list to add the possible error messages to.
	* \param parentIdentifier String from parent to help to identify the location of the error.
	* \param checkChildren If true then the child-items are also checked. If
	* false then only this model is checked.
	*
	* \return bool True if the state is valid and writing is possible.
	*/
	bool isValid(QStringList& errorList, 
		const QString& parentIdentifier, 
		bool checkChildren = true) const;

	/*! \brief Check if the file is in a valid state.
	 * 
	 * \param checkChildren If true then the child-items are also checked. If
	 * false then only this model is checked.
	 *
	 * \return bool True if the state is valid and writing is possible.
	*/
	bool isValid(bool checkChildren = true) const;

	/*! \brief The == operator
	 * 
	 * Files are considered equal if they have same name.
	 * 
	 * \param other Reference to the other file to compare to.
	 *
	 * \return bool True if files had a same name.
	*/
	bool operator==(const File& other) const;

	/*! \brief The != operator.
	 * 
	 * Files are considered unequal if they have different name.
	 * 
	 * \param other Reference to the other file to compare to.
	 *
	 * \return bool True if the files have different name.
	*/
	bool operator!=(const File& other) const;

	/*! \brief Get the file types
	*
	* \return QList containing the file types.
	*/
	const QStringList& getFileTypes() const;

	/*! \brief Get the userFileTypes for the file
	*
	* \return QList containing the userFileTypes for the file.
	*/
	const QStringList& getUserFileTypes();

	/*! \brief Check if the file matches at least one of the specified file types.
	 *
	 * Method: 		matchesFileType
	 * Full name:	File::matchesFileType
	 * Access:		public 
	 *
	 * \param fileTypes The file types to check.
	 *
	 * \return True if at least on of the file types matches the file types of this file.
	*/
	bool matchesFileType(const QStringList& fileTypes) const;

	/*! \brief Check if the file matches the specified file type.
	 *
	 * Method: 		matchesFileType
	 * Full name:	File::matchesFileType
	 * Access:		public 
	 *
	 * \param fileType The file type to check.
	 *
	 * \return True if the file type is found either in file types or user file types.
	*/
	bool matchesFileType(const QString& fileType) const;

	/*! \brief Set the file types for this file
	*
	* Calling this function will delete old file types
	*
	* \param fileTypes QList containing the file types.
	*/
	void setFileTypes(const QStringList& fileTypes);

	/*! \brief Set the file types for the file to match the defined settings.
	 *
	 * Method: 		setFileTypes
	 * Full name:	File::setFileTypes
	 * Access:		public 
	 *
	 * \param settings Contains the file type settings.
	 *
	*/
	void setFileTypes(QSettings& settings);

	/*! \brief Add a new file type definition.
	*
	* FileType is only added if it is not already defined in fileTypes.
	*
	* \param fileType The name of the file type. This must be one of the file
	* types defined in IP-Xact standard specification.
	*/
	void addFileType(const QString fileType);

	/*! \brief Add a new user file type definition.
	*
	* UserFileType is only added if it is not already defined in userFileTypes.
	*
	* \param userFileType The name of the file type. This file type is added
	* to the userFileTypes element so it is not defined in IP-Xact standard.
	*/
	void addUserFileType(const QString userFileType);

	/*! \brief Clear all the fileTypes and userFileTypes
	*
	*/
	void clearFileTypes();

	/*! \brief Get both the file types and user file types in the same list.
	 *
	 * \return QStringList containing all file types.
	*/
	QStringList getAllFileTypes() const;

	/*! \brief Set the file types for the file.
	 *
	 * \param fileTypes Contains both the IP-Xact file types and user file types.
	 *
	*/
	void setAllFileTypes(const QStringList& fileTypes);

	/*! \brief Get the attributes for the file-element
	 *
	 * \return QList containing pointers to the attributes.
	 */
	const QMap<QString, QString>& getAttributes();

	/*! \brief Get the build command for this file
	 *
	 * \return A pointer to the BuildCommand containing the flags and commands.
	 */
	BuildCommand *getBuildcommand();

	/*! \brief Get the dependencies for this file
	 *
	 * \return QList containing the paths to the directories containing files
	 * on which this file depends on.
	 */
	const QList<QString>& getDependencies();

	/*! \brief Get the description of this file.
	 *
	 * \return QString containing the description.
	 */
	QString getDescription() const;

	/*! \brief Get the exportedName for this file
	 *
	 * \return QList containing the names that can be referenced externally.
	 */
	const QList<QString>& getExportedNames();

	/*! \brief Get attribute value of external declarations
	 *
	 * \return boolean value of the attribute.
	 */
	bool getExternalDeclarations() const;

	/*! \brief Get the field-attribute value of the file-element.
	 *
	 * \return QString containing the field-attribute value.
	 */
	QString getFileId() const;

	/*! \brief Get the imageTypes of this file
	 *
	 * \return QList containing the imageTypes.
	 */
	const QList<QString>& getImageTypes();

	/*! \brief Get the isIncludeFile setting
	 *
	 * \return boolean value of the isIncludeFile element.
	 */
	bool getIncludeFile() const;

	/*! \brief Get the logicalName of the file
	 *
	 * \return QString containing the name.
	 */
	QString getLogicalName() const;

	/*! \brief Get the default-attribute value of the logicalName element.
	 *
	 * \return boolean value of the default-attribute.
	 */
	bool getLogicalNameDefault() const;

	/*! \brief Get the name of the file
	 *
	 * \return QString containing the name.
	 */
	QString getName() const;

	/*! \brief Get the list of the attributes for the name-element.
	 *
	 * \return QMap containing the attributes for the name-element.
	 */
	const QMap<QString, QString>& getNameAttributes();

    /*!
     *  Returns the last calculated hash.
     */
    QString const& getLastHash() const;

    /*! \brief Set the attributes for the file
	 *
	 * Calling this function will delete old attributes.
	 *
	 * \param attributes Qmap containing the new attributes.
	 */
	void setAttributes(const QMap<QString, QString> &attributes);

	/*! \brief Set the buildCommand for this file
	 *
	 * Calling this function will delete the old build command
	 *
	 * \param buildcommand A pointer to the new build command.
	 */
	void setBuildcommand(BuildCommand *buildcommand);

	/*! \brief Set the dependencies for this file.
	 *
	 * Calling this function will delete old dependencies.
	 *
	 * \param dependencies QList containing the paths to directories containing
	 * files on which this file depends on.
	 */
	void setDependencies(const QList<QString> &dependencies);

	/*! \brief Set the description for this file.
	 *
	 * \param description QString containing the description.
	 */
	void setDescription(const QString &description);

	/*! \brief Set the exported names for this file
	 *
	 * Calling this function will delete the old exported names
	 *
	 * \param exportedNames QList containing the exported names.
	 */
	void setExportedNames(const QList<QString> &exportedNames);

	/*! \brief Set the externalDeclarations setting
	 *
	 * \param externalDeclarations boolean value to be set.
	 */
	void setExternalDeclarations(bool externalDeclarations);

	/*! \brief Set the field-setting for this file
	 *
	 * \param field QString containing the setting
	 */
	void setFileId(const QString &fileId);

	/*! \brief Set the image types for this file
	 *
	 * Calling this function will delete the old imageTypes
	 *
	 * \param imageTypes QList containing the new image types.
	 */
	void setImageTypes(const QList<QString> &imageTypes);

	/*! \brief Set the isIncludeFile setting for this file
	 *
	 * \param includeFile boolean value to be set.
	 */
	void setIncludeFile(bool includeFile);

	/*! \brief Set the logical name for this file
	 *
	 * \parma logicalName QString containing the new logical name.
	 */
	void setLogicalName(const QString &logicalName);

	/*! \brief Set the logical name attribute default
	 *
	 * \param logicalNameDefault boolean value of the attribute.
	 */
	void setLogicalNameDefault(bool logicalNameDefault);

	/*! \brief Set the name for this file.
	 *
	 * \param name QString containing the name.
	 */
	void setName(const QString &name);

	/*! \brief Set the attributes for the name-element.
	 *
	 * Calling this function will delete the old attributes for the name-
	 * element
	 *
	 * \param nameAttributes QMap the new attributes.
	 */
	void setNameAttributes(const QMap<QString, QString> &nameAttributes);

    /*!
     *  Sets the newest calculated hash pending for commit.
     *
     *      @param [in] hash The calculated hash value.
     */
    void setPendingHash(QString const& hash);

	/*! \brief Set the userFileTypes for this file
	 *
	 * Calling this function will delete the old userFileTypes.
	 *
	 * \param userFileTypes QList containing the new userFileTypes.
	 */
	void setUserFileTypes(const QStringList& userFileTypes);

	/*! \brief Get the defines of the file
	 *
	 * \return QList containing pointers to the attributes.
	 */
	const QList<Define>& getDefines();

	/*! \brief Set the defines for this file.
	 *
	 * Calling this function will delete the old defines
	 *
	 * \param defines QList containing the pointers to the name-value pairs.
	 */
	void setDefines(const QList<Define> &defines);

	/*! \brief Clear all the defines from the file
	 *
	*/
	void clearDefines();

	/*! \brief Append a new define for the file.
	 *
	 * \param define The define to add
	 *
	*/
	void appendDefine(const Define& define);

	/*! \brief Remove this file.
	 * 
	 * This function calls the containing FileSet to remove this file.
	 *
	*/
	void remove();

	/*! \brief Set the fileSet that contains this file.
	 *
	 * \param fileSet Pointer to the FileSet instance that contains this file.
	 *
	*/
	void setParent(FileSet* fileSet);

    /*!
     *  Returns the parent file set.
     */
    FileSet* getParent() const;

	/*! \brief Get the build command's command-part
	 *
	 * \return QString containing the command.
	*/
	QString getCommand() const;

	/*! \brief Get the build flags for this file.
	 *
	 * \return QString containing the flags for this file.
	*/
	QString getFlags() const;

	/*! \brief Check if the file is a RTL file or not.
	 *
	 * \return bool True if file is vhdl or verilog file.
	*/
	bool isRTLFile() const;

	/*! \brief Set a build command for the file
	 *
	 * \param buildCommand QString containing the command.
	 *
	*/
	void setCommand(const QString& buildCommand);

	/*! \brief Set the flags for the build command
	 *
	 * \param buildFlags QString containing the flags
	 * \param replaceDefault Should the default flags be replaced or appended.
	 *
	*/
	void setBuildFlags(const QString& buildFlags, bool replaceDefault);

	/*! \brief Checks if the file is a Quartus pin map.
	 *
	 * \return True if file is pin map.
	*/
	bool isQuartusPinmap() const;

	/*! \brief Check if this file is a vhdl file.
	 *
	 *
	 * \return bool True if the file is vhdl-file.
	*/
	bool isVhdlFile() const;

	/*! \brief Get the name of the file set this file belongs to.
	 *
	 * \return QString containing the name of the parent file set.
	*/
	QString fileSetName() const;

private:

	/*! \brief Contains a path to a filename of a directory
	 * MANDATORY spirit:name
	 *
	 */
	QString name_;

	/*! \brief Contains the id.
	 * OPTIONAL spirit:field
	 * This contains the fileId attribute which is used for references to this
	 * file from inside the fileSet/function/fileRef element.
	 */
	QString fileId_;

	/*! \brief Contains any additional attributes for the File-element.
	 * OPTIONAL
	 */
	QMap<QString, QString> attributes_;

	/*! \brief Contains the attributes for the name-element.
	 * OPTIONAL
	 */
	QMap<QString, QString> nameAttributes_;

	/*! \brief Either fileType or userFileType is mandatory
	 * MANDATORY spirit:fileType
	 */
	QStringList fileTypes_;

	/*! \brief Either FileType or userFileType is mandatory
	 * MANDATORY spirit:userFileType
	 */
	QStringList userFileTypes_;

	/*! \brief Declares if the file is an include file or not.
	 * OPTIONAL spirit:inludeFile
	 * default = false
	 */
	bool includeFile_;

	/*! \brief Indicates if the include file is needed by users
	 * OPTIONAL spirit:externalDeclarations
	 * Attribute for the includeFile element.
	 * default = false
	 */
	bool externalDeclarations_;

	/*! \brief The logical name for the file or directory.
	 * OPTIONAL spirit:logicalName
	 */
	QString logicalName_;

	/*! \brief Can another process override this name or not.
	 * OPTIONAL spirit:default
	 * Attribute for the logicalName element.
	 * default = false
	 */
	bool logicalNameDefault_;

	/*! \brief Defines any names that can be referenced externally.
	 * OPTIONAL spirit:exportedName
	 */
	QList<QString> exportedNames_;

	/*! \brief Contains paths to the directories containing dependecy files
	 * OPTIONAL spirit:dependency
	 */
	QList<QString> dependencies_;

	/*! \brief Relates the file to a executable image type in the design.
	 * OPTIONAL spirit:imageType
	 */
	QList<QString> imageTypes_;

	/*! \brief Details the file for the user.
	 * OPTIONAL spirit:description
	 */
	QString description_;

	/*! \brief Contains the buildCommand element
	 * OPTIONAL spirit:buildCommand
	 * Pointer to a BuildCommand which contains flags or commands for building
	 * the containing source file.
	 */
	QSharedPointer<BuildCommand> buildcommand_;

	/*! \brief Specifies the define symbols to use in the source file
	 * OPTIONAL spirit:define
	 */
	QList<Define> defines_;

    //! The last calculated hash (from the file contents).
    QString lastHash_;

    //! The current hash that is pending for writing to disk.
    QString pendingHash_;

	//! \brief Pointer to the FileSet that contains this file.
	FileSet* parent_;
};

#endif /* FILE_H_ */
