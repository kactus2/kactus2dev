//-----------------------------------------------------------------------------
// File: File.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 16.09.2015
//
// Description:
// Describes the ipxact:file element.
//-----------------------------------------------------------------------------

#ifndef FILE_H
#define FILE_H

#include <IPXACTmodels/common/BuildModel.h>
#include <IPXACTmodels/Component/BuildCommand.h>

#include <IPXACTmodels/common/NameValuePair.h>
#include <IPXACTmodels/common/Extendable.h>

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QString>
#include <QSharedPointer>
#include <QStringList>
#include <QSettings>

//-----------------------------------------------------------------------------
//! Describes the ipxact:file element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT File : public Extendable
{

public:

    /*!
     *  The constructor.
     *
     *      @param [in] filePath    Path to or name of the file.
     *      @param [in] fileType    The file type.
     */
    File (QString const& filePath = QString(), QString const& fileType = QString());

	//! Copy constructor.
    File(const File& other);

	//! Assignment operator.
	File& operator=(const File &other);

	//! The destructor.
	~File();

    //! Comparison operator
	bool operator==(const File& other) const;

    //! Opposed comparison operator.
	bool operator!=(const File& other) const;

	/*!
	 *  Get the id of the file.
	 *
     *      @return The ID of the file.
	 */
	QString getFileId() const;
    
	/*!
     *  Set the id.
	 *
	 *      @param [in]     newFileId The new id.
	 */
	void setFileId(QString const& newFileId);

	/*! 
     *  Get the attributes for the file-element
	 *
	 *      @return QMap containing the attributes.
	 */
	QMap<QString, QString> getAttributes() const;

    /*! 
     *  Set the attributes for the file
	 *
	 *      @param [in]     newAttributes Qmap containing the new attributes.
	 */
	void setAttributes(QMap<QString, QString> const& newAttributes);

	/*! 
     *  Get the name of the file
	 *
	 *      @return QString containing the name.
	 */
	QString name() const;

	/*! 
     *  Set the name for this file.
	 *
	 *      @param [in] name    QString containing the name.
	 */
	void setName(QString const& newName);

    /*!
     *  Get the presence of this file.
     *
     *      @return The presence of this file.
     */
    QString getIsPresent() const;

    /*!
     *  Set the presence for this file.
     *
     *      @param [in] newIsPresent    The new presence.
     */
    void setIsPresent(QString const& newIsPresent);

	/*! 
    *   Get the file types
	*
	*       @return Pointer to a QList containing the file types.
	*/
    QSharedPointer<QStringList> getFileTypes() const;

	/*! 
    *   Set the file types for this file
	*
	*       @param [in] fileTypes   QList containing the new file types.
	*/
	void setFileTypes(QSharedPointer<QStringList> newFileTypes);

    /*!
     *  Check if the file contains structural RTL.
     *
     *      @return True if the file contains structural RTL, false otherwise.
     */
    bool isStructural() const;

    /*!
     *  Set the file to contain structural RTL.
     *
     *      @param [in] structuralStatus    The new structural status.
     */
    void setStructural(bool structuralStatus);

	/*!
     *  Get the isIncludeFile setting.
	 *
	 *      @return True if the file is an include file, otherwise false.
	 */
	bool isIncludeFile() const;

	/*! 
     *  Set the isIncludeFile setting for this file.
	 *
	 *      @param [in] includeFile     boolean value to be set.
	 */
	void setIncludeFile(bool includeFile);

	/*! 
     *  Get attribute value of external declarations.
	 *
	 *      @return True of the file has external declarations, otherwise false.
	 */
	bool hasExternalDeclarations() const;

	/*! 
     *  Set the externalDeclarations setting
	 *
	 *      @param [in] externalDeclarations    Boolean value to be set.
	 */
	void setExternalDeclarations(bool externalDeclarations);

	/*! 
     *  Get the logicalName of the file
	 *
	 *      @return QString containing the name.
	 */
	QString getLogicalName() const;

	/*! 
     *  Set the logical name for this file.
	 *
	 *      @param [in] logicalName     QString containing the new logical name.
	 */
	void setLogicalName(QString const& logicalName);

	/*! 
     *  Get the default-attribute value of the logicalName element.
	 *
	 *      @return True of the logical name is default, otherwise false.
	 */
	bool isLogicalNameDefault() const;

	/*! 
     *  Set the logical name attribute default.
	 *
	 *      @param [in] logicalNameDefault  Boolean value of the attribute.
	 */
	void setLogicalNameDefault(bool logicalNameDefault);

	/*! 
     *  Get the exportedName for this file
	 *
	 *      @return QList containing the names that can be referenced externally.
	 */
    QSharedPointer<QStringList> getExportedNames() const;

	/*! 
     *  Set the exported names for this file
	 *
	 *      @param [in] exportedNames   QList containing the exported names.
	 */
	void setExportedNames(QSharedPointer<QStringList> exportedNames);

	/*! 
     *  Get the build command for this file
	 *
	 *      @return A pointer to the BuildCommand containing the flags and commands.
	 */
    QSharedPointer<BuildCommand> getBuildCommand() const;

	/*! 
     *  Set the buildCommand for this file
	 *
	 *      @param [in] buildcommand    A pointer to the new build command.
	 */
    void setBuildcommand(QSharedPointer<BuildCommand> newBuildCommand);

	/*! 
     *  Get the dependencies for this file
	 *
	 *      @return QList containing the paths to the directories containing files on which this file depends on.
	 */
    QSharedPointer<QStringList> getDependencies();

	/*! 
     *  Set the dependencies for this file.
	 *
	 *      @param [in] dependencies    The paths to directories containing files on which this file depends on.
	 */
    void setDependencies(QSharedPointer<QStringList> newDependencies);

	/*! 
     *  Get the defines of the file
	 *
	 *      @return Name value pairs of the file defines.
	 */
    QSharedPointer<QList<QSharedPointer<NameValuePair> > > getDefines();

	/*! 
     *  Set the defines for this file.
	 *
	 *      @param [in] defines     The new name-value pairs.
	 */
    void setDefines(QSharedPointer<QList<QSharedPointer<NameValuePair> > > newDefines);

	/*! 
     *  Get the imageTypes of this file
	 *
	 *      @return QList containing the imageTypes.
	 */
    QSharedPointer<QStringList> getImageTypes();

	/*! 
     *  Set the image types for this file
	 *
	 *      @param [in] imageTypes  QList containing the new image types.
	 */
    void setImageTypes(QSharedPointer<QStringList> newImageTypes);

	/*! 
     *  Get the description of this file.
	 *
	 *      @return QString containing the description.
	 */
	QString getDescription() const;

	/*! 
     *  Set the description for this file.
	 *
	 *      @param [in] description     QString containing the description.
	 */
	void setDescription(QString const& description);

    /*!
     *  Returns the last calculated hash.
     *
     *      @return The last calculated hash.
     */
    QString getLastHash() const;

    /*!
     *  Set the last calculated hash.
     *
     *      @param [in] newHash     The new hash value.
     */
    void setLastHash(QString const& newHash);

    /*!
     *  Gets the newest calculated hash pending for commit.
     *
     *      @return The pending hash.
     */
    QString getPendingHash() const;

    /*!
     *  Sets the newest calculated hash pending for commit.
     *
     *      @param [in] hash    The calculated hash value.
     */
    void setPendingHash(QString const& hash);

	/*! 
     *  Check if the file matches at least one of the specified file types.
	 *
	 *      @param [in] fileTypes   The file types to check.
	 *
	 *      @return True if at least on of the file types matches the file types of this file.
     */
	bool matchesFileType(const QStringList& fileTypes) const;

	/*! 
     *  Check if the file matches the specified file type.
	 *
	 *      @param [in] fileType    The file type to check.
	 *
	 *      @return True if the file type is found either in file types or user file types.
	 */
	bool matchesFileType(const QString& fileType) const;

	/*! Set the file types for the file to match the defined settings.
	 *
	 *      @param [in] settings    Contains the file type settings.
	 *
	 */
	void setFileTypes(QSettings& settings);

	/*! 
     *  Add a new file type definition.
	 *
	 *      @param [in] fileType    The name of the file type.
	 */
	void addFileType(const QString fileType);

	/*!
     *  Clear all the fileTypes and userFileTypes
	 *
	 */
	void clearFileTypes();

	/*! 
     *  Clear all the defines from the file
	 */
	void clearDefines();

	/*! 
     *  Get the build command's command-part
	 *
	 *      @return QString containing the command.
	 */
	QString getCommand() const;

	/*! 
     *  Get the build flags for this file.
	 *
	 *      @return QString containing the flags for this file.
	 */
	QString getFlags() const;

	/*! 
     *  Check if the file is a RTL file or not.
	 *
	 *      @return bool True if file is vhdl or verilog file.
	 */
	bool isRTLFile() const;
    
	/*! 
     *  Check if this file is a vhdl file.
	 *
	 *      @return bool True if the file is vhdl-file.
	 */
	bool isVhdlFile() const;

    /*!
     *  Check if the file is a verilog file.
     *
     *      @return True if the file is a verilog file.
     */
    bool isVerilogFile() const;

	/*!
     *  Set a build command for the file
	 *
	 *      @param [in] buildCommand    QString containing the command.
	 */
	void setCommand(const QString& buildCommand);

	/*! 
     *  Set the flags for the build command
	 *
	 *      @param [in] buildFlags      QString containing the flags
	 *      @param [in] replaceDefault  Should the default flags be replaced or appended.
	 *
	 */
    void setBuildFlags(QString const& buildFlags, QString const& replaceDefaultFlags);
	
	/*! 
     *  Get the file name, including the extension, if it exists. Excludes the path.
	 */
	QString getFileName() const;

private:
	
    /*!
     *  Parses the current fileName_ from path in name_.
     */
	void parseFileName();

    /*!
     *  Copy the build command from another file.
     */
    void copyBuildCommand(const File& other);

    /*!
     *  Copy the defines from another file.
     */
    void copyDefines(const File& other);

    /*!
     *  Copy the file types, exported names, dependencies and image types from another file.
     */
    void copyStringLists(const File& other);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! Contains the id.
	QString fileId_;

	//! Contains any additional attributes for the File-element.
	QMap<QString, QString> attributes_;
    
	//! Contains a path to a filename of a directory.
	QString name_;

	//! Contains the name of the file, including the extension, if it exists.
	QString fileName_;

    //! Contains the presence of the file.
    QString isPresent_;

	//! Contains the file types of the file.
	QSharedPointer<QStringList> fileTypes_;

    //! Holds whether the file contains a structural RTL or not.
    bool structural_;

	//! Declares if the file is an include file or not.
	bool includeFile_;

	//! Indicates if the include file is needed by users.
	bool externalDeclarations_;

	//! The logical name for the file or directory.
	QString logicalName_;

	//! Can another process override this name or not.
	bool logicalNameDefault_;

	//! Defines any names that can be referenced externally.
    QSharedPointer<QStringList> exportedNames_;

	//! Contains the buildCommand element.
	QSharedPointer<BuildCommand> buildCommand_;

	//! Contains paths to the directories containing dependency files.
    QSharedPointer<QStringList> dependencies_;

	//! Specifies the define symbols to use in the source file.
    QSharedPointer<QList<QSharedPointer<NameValuePair> > > defines_;

	//! Relates the file to a executable image type in the design.
    QSharedPointer<QStringList> imageTypes_;

	//! Details the file for the user.
	QString description_;

    //! The current hash that is pending for writing to disk.
    QString pendingHash_;
};

#endif // FILE_H
