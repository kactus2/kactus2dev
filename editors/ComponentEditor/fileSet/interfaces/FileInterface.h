//-----------------------------------------------------------------------------
// File: FileInterface.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 29.06.2020
//
// Description:
// Interface for editing files.
//-----------------------------------------------------------------------------

#ifndef FILEINTERFACE_H
#define FILEINTERFACE_H

#include <common/Global.h>

#include <editors/ComponentEditor/common/interfaces/ParameterizableInterface.h>
#include <editors/ComponentEditor/common/interfaces/NameGroupInterface.h>

class File;
class BuildCommand;
class FileValidator;

#include <QSharedPointer>
#include <QVector>
#include <QMap>

//-----------------------------------------------------------------------------
//! Interface for editing files.
//-----------------------------------------------------------------------------
class KACTUS2_API FileInterface: public ParameterizableInterface, public NameGroupInterface
{

public:

    /*!
     *  The constructor.
     *
     *      @param [in] validator               Validator for files.
     *      @param [in] expressionParser        Parser for expressions.
     *      @param [in] expressionFormatter     Formatter for expressions.
     */
    FileInterface(QSharedPointer<FileValidator> validator, QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<ExpressionFormatter> expressionFormatter);
	
	/*!
     *  The destructor.
     */
     virtual ~FileInterface() = default;

    /*!
     *  Set available files.
     *
     *      @param [in] newFiles    The new files.
     */
    void setFiles(QSharedPointer<QList<QSharedPointer<File> > > newFiles);

    /*!
     *  Get index of the selected item.
     *
     *      @param [in] itemName    Name of the selected item.
     *
     *      @return Index of the selected item.
     */
    virtual int getItemIndex(std::string const& itemName) const override final;

    /*!
     *  Get name of the indexed item.
     *
     *      @param [in] itemIndex   Index of the selected item.
     *
     *      @return Name of the selected item.
     */
    virtual std::string getIndexedItemName(int const& itemIndex) const override final;

    /*!
     *  Get the number of available items.
     *
     *      @return Number of available items.
     */
    virtual int itemCount() const override final;

    /*!
     *  Get the names of the available items.
     *
     *      @return Names of the available items.
     */
    virtual std::vector<std::string> getItemNames() const override final;

    /*!
     *  Set a new name for the selected item.
     *
     *      @param [in] currentName     Name of the selected item.
     *      @param [in] newName         New name for the item.
     *
     *      @return True, if successful, false otherwise.
     */
    virtual bool setName(std::string const& currentName, std::string const& newName) override final;

    /*!
     *  Get the description of the selected item.
     *
     *      @param [in] itemName    Name of the selected item.
     *
     *      @return Description of the selected item.
     */
    virtual std::string getDescription(std::string const& itemName) const override final;

    /*!
     *  Set a new description for the selected item.
     *
     *      @param [in] itemName        Name of the selected item.
     *      @param [in] newDescription  New description.
     *
     *      @return True, if successful, false otherwise.
     */
    virtual bool setDescription(std::string const& itemName, std::string const& newDescription) override final;

    /*!
     *  Validates the contained items.
     *
     *      @return True, if all the ports are valid, false otherwise.
     */
    virtual bool validateItems() const override final;

    /*!
     *  Check if the selected item has a valid name.
     *
     *      @param [in] itemName    Name of the selected item.
     *
     *      @return True, if the name is valid, false otherwise.
     */
    virtual bool itemHasValidName(std::string const& itemName) const override final;

    /*!
     *  Calculate all the references to the selected ID in the selected file.
     *
     *      @param [in] fileName    Name of the selected item.
     *      @param [in] valueID     The selected ID.
     *
     *      @return Number of references to the selected ID in the selected file.
     */
    virtual int getAllReferencesToIdInItem(const std::string& fileName, std::string const& valueID) const override
        final;

    /*!
     *  Get the expressions in the selected files.
     *
     *      @param [in] fileNames   Names of the selected files.
     *
     *      @return The expressions of the selected files.
     */
    std::vector<std::string> getExpressionsInSelectedFiles(std::vector<std::string> fileNames) const;

    /*!
     *  Add a new file.
     *
     *      @param [in] row             Row of the new file.
     *      @param [in] newFileName     Name of the new file.
     */
    void addFile(int const& row, std::string const& newFileName = std::string(""));

    /*!
     *  Remove the selected file.
     *
     *      @param [in] fileName    Name of the selected file.
     *
     *      @return True, if successful, false otherwise.
     */
    bool removeFile(std::string const& fileName);

    /*!
     *  Swap the positions of the selected files.
     *
     *      @param [in] firstIndex      Index of the first file.
     *      @param [in] secondIndex     Index of the second file.
     */
    void swapFiles(int const& firstIndex, int const& secondIndex);

    /*!
     *  Get the logical name of a file.
     *
     *      @param [in] fileName    Name of the selected file.
     *
     *      @return Logical name of the selected file.
     */
    std::string getLogicalName(std::string const& fileName) const;

    /*!
     *  Set a new logical name.
     *
     *      @param [in] fileName        Name of the selected file.
     *      @param [in] newLogicalName  The new logical name.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setLogicalName(std::string const& fileName, std::string newLogicalName);

    /*!
     *  Get the default-attribute value of the logicalName element.
     *
     *      @param [in] fileName    Name of the selected file.
     *
     *      @return True of the logical name is default, otherwise false.
     */
    bool isLogicalNameDefault(std::string const& fileName) const;

    /*!
     *  Set the logical name attribute default.
     *
     *      @param [in] fileName            Name of the selected file.
     *      @param [in] newLogicalDefault   Boolean value of the attribute.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setLogicalNameDefault(std::string const& fileName, bool newLogicalDefault);

    /*!
     *  Check if the file contains a structural RTL or not.
     *
     *      @param [in] fileName    Name of the selected file.
     *
     *      @return True, if the file contains a structural RTL, false otherwise.
     */
    bool isStructural(std::string const& fileName) const;

    /*!
     *  Set the file to contain structural RTL.
     *
     *      @param [in] fileName            Name of the selected file.
     *      @param [in] structuralStatus    The new structural status.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setStructural(std::string const& fileName, bool structuralStatus);
    
    /*!
     *  Get the isIncludeFile setting.
	 *
     *      @param [in] fileName    Name of the selected file.
     *
	 *      @return True if the file is an include file, otherwise false.
	 */
	bool isIncludeFile(std::string const& fileName) const;

	/*! 
     *  Set the isIncludeFile setting for this file.
	 *
     *      @param [in] fileName            Name of the selected file.
	 *      @param [in] includeFileStatus   Boolean value to be set.
     *
     *      @return True, if successful, false otherwise.
     */
	bool setIncludeFile(std::string const& fileName, bool includeFileStatus);
    
    /*! 
     *  Get attribute value of external declarations.
	 *
     *      @param [in] fileName    Name of the selected file.
     *
	 *      @return True of the file has external declarations, otherwise false.
	 */
	bool hasExternalDeclarations(std::string const& fileName) const;

	/*! 
     *  Set the externalDeclarations setting
	 *
     *      @param [in] fileName                    Name of the selected file.
	 *      @param [in] externalDeclarationsStatus  Boolean value to be set.
     *
     *      @return True, if successful, false otherwise.
	 */
	bool setExternalDeclarations(std::string const& fileName, bool externalDeclarationsStatus);

    /*!
     *  Clear all the fileTypes and userFileTypes.
     *
     *      @param [in] fileName    Name of the selected file.
     */
    void clearFileTypes(std::string const& fileName);

    /*!
     *  Set file types for the selected file.
     *
     *      @param [in] fileName        Name of the selected file.
     *      @param [in] newFileTypes    The new file types.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setFileTypes(std::string const& fileName, std::vector<std::string> const newFileTypes);

    /*!
     *  Add the selected file type to the selected file.
     *
     *      @param [in] fileName        Name of the selected file.
     *      @param [in] newFileType     The new file type.
     *
     *      @return True, if successful, false otherwise.
     */
    bool addFileType(std::string const& fileName, std::string const newFileType);

    /*!
     *  Add multiple file type to the selected file.
     *
     *      @param [in] fileName        Name of the selected file.
     *      @param [in] newFileTypes    The new file types.
     *
     *      @return True, if successful, false otherwise.
     */
    bool addMultipleFileTypes(std::string const& fileName, std::vector<std::string> const newFileTypes);

    /*!
     *  Get the dependencies of a file.
     *
     *      @param [in] fileName    Name of the selected file.
     *
     *      @return Dependencies of the selected file.
     */
    std::vector<std::string> getDependencies(std::string const& fileName) const;

    /*!
     *  Clear all the dependencies on the selected file.
     *
     *      @param [in] fileName    Name of the selected file.
     */
    void clearDependencies(std::string const& fileName);

    /*!
     *  Add the selected dependency to the selected file.
     *
     *      @param [in] fileName        Name of the selected file.
     *      @param [in] newDependency   The new dependency.
     *
     *      @return True, if successful, false otherwise.
     */
    bool addDependency(std::string const& fileName, std::string const newDependency);

    /*!
     *  Add multiple dependencies to the selected file.
     *
     *      @param [in] fileName            Name of the selected file.
     *      @param [in] newDependencies     The new dependencies.
     *
     *      @return True, if successful, false otherwise.
     */
    bool addMultipleDependencies(std::string const& fileName, std::vector<std::string> const newDependencies);

    /*!
     *  Get the exported names of a file.
     *
     *      @param [in] fileName    Name of the selected file.
     *
     *      @return Exported names of the selected file.
     */
    std::vector<std::string> getExportedNames(std::string const& fileName) const;

    /*!
     *  Clear all the exported names of the selected file.
     *
     *      @param [in] fileName    Name of the selected file.
     */
    void clearExportedNames(std::string const& fileName);

    /*!
     *  Add the selected exported name to the selected file.
     *
     *      @param [in] fileName            Name of the selected file.
     *      @param [in] newExportedName     The new exported name.
     *
     *      @return True, if successful, false otherwise.
     */
    bool addExportedName(std::string const& fileName, std::string const newExportedName);

    /*!
     *  Add multiple exported names to the selected file.
     *
     *      @param [in] fileName            Name of the selected file.
     *      @param [in] newExportedNames    The new exported names.
     *
     *      @return True, if successful, false otherwise.
     */
    bool addMultipleExportedNames(std::string const& fileName, std::vector<std::string> const newExportedNames);

    /*!
     *  Get the image types of a file.
     *
     *      @param [in] fileName    Name of the selected file.
     *
     *      @return Image types of the selected file.
     */
    std::vector<std::string> getImageTypes(std::string const& fileName) const;

    /*!
     *  Clear all the image types on the selected file.
     *
     *      @param [in] fileName    Name of the selected file.
     */
    void clearImageTypes(std::string const& fileName);

    /*!
     *  Add the selected image type to the selected file.
     *
     *      @param [in] fileName        Name of the selected file.
     *      @param [in] newImageType    The new image type.
     *
     *      @return True, if successful, false otherwise.
     */
    bool addImageType(std::string const& fileName, std::string const newImageType);

    /*!
     *  Add multiple image types to the selected file.
     *
     *      @param [in] fileName        Name of the selected file.
     *      @param [in] newImageTypes   The new image types.
     *
     *      @return True, if successful, false otherwise.
     */
    bool addMultipleImageTypes(std::string const& fileName, std::vector<std::string> const newImageTypes);

    /*!
     *   Get the file types of the selected file.
     *
     *      @param [in] fileName    Name of the selected file.
     *
     *      @return List containing the file types.
     */
    std::vector<std::string> getFileTypes(std::string const& fileName) const;

    /*!
     *   Get the build command of selected file.
     *
     *      @param [in] fileName    Name of the selected file.
     *
     *      @return Build command of the selected file.
     */
    std::string getBuildCommandText(std::string const& fileName) const;

   	/*! 
     *  Set the build command for the selected file.
	 *
     *      @param [in] fileName            Name of the selected file.
	 *      @param [in] newBuildCommand     The new build command.
     *
     *      @return True, if successful, false otherwise.
	 */
	bool setBuildCommand(std::string const& fileName, std::string const& newBuildCommand);

    /*!
     *   Get the build command flags of selected file.
     *
     *      @param [in] fileName    Name of the selected file.
     *
     *      @return Build command flags of the selected file.
     */
    std::string getBuildCommandFlags(std::string const& fileName) const;

    /*! 
     *  Set the build flags for the selected file.
	 *
     *      @param [in] fileName    Name of the selected file.
	 *      @param [in] newFlags    The new flags.
     *
     *      @return True, if successful, false otherwise.
	 */
	bool setBuildCommandFlags(std::string const& fileName, std::string const& newFlags);

    /*!
     *  Get the calculated build command replace default flags value of the selected file.
     *
     *      @param [in] fileName    Name of the selected file.
     *      @param [in] baseNumber  Base for displaying the value.
     *
     *      @return Calculated build command replace default flags value of the selected file.
     */
    std::string getBuildCommandReplaceDefaultFlagsValue(std::string const& fileName, int const& baseNumber = 0)
        const;

    /*!
     *  Get the formatted build command replace default flags expression of the selected file.
     *
     *      @param [in] fileName    Name of the selected file.
     *
     *      @return Formatted build command replace default flags expression of the selected file.
     */
    std::string getBuildCommandReplaceDefaultFlagsFormattedExpression(std::string const& fileName) const;

    /*!
     *  Get the build command replace default flags expression of the selected file.
     *
     *      @param [in] fileName    Name of the selected file.
     *
     *      @return Build command replace default flags expression of the selected file.
     */
    std::string getBuildCommandReplaceDefaultFlagsExpression(std::string const& fileName) const;

    /*!
     *  Set a new build command replace default flags value for the selected file.
     *
     *      @param [in] fileName                Name of the selected file.
     *      @param [in] newReplaceDefaultFlags  New build command replace default flags value.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setbuildCommandReplaceDefaultFlags(std::string const& fileName,
        std::string const& newReplaceDefaultFlags);

    /*!
     *   Get the build command target of selected file.
     *
     *      @param [in] fileName    Name of the selected file.
     *
     *      @return Build command target of the selected file.
     */
    std::string getBuildCommandTarget(std::string const& fileName) const;

    /*!
     *  Set the build target for the selected file.
     *
     *      @param [in] fileName    Name of the selected file.
     *      @param [in] newTarget   The new target.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setBuildCommandTarget(std::string const& fileName, std::string const& newTarget);

private:

    /*!
     *  Get the selected file.
     *
     *      @param [in] fileName    Name of the selected file.
     *
     *      @return The selected file.
     */
    QSharedPointer<File> getFile(std::string const& fileName) const;

    /*!
     *  Get the build command of the selected file.
     *
     *      @param [in] fileName    Name of the selected file.
     *
     *      @return The build command of the selected file.
     */
    QSharedPointer<BuildCommand> getBuildCommand(std::string const& fileName) const;

    /*!
     *  Remove an empty build command from the selected file.
     *
     *      @param [in] containingFile  The selected file.
     */
    void removeEmptyBuildCommand(QSharedPointer<File> containingFile) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! List of the contained files.
    QSharedPointer<QList<QSharedPointer<File> > > files_;

    //! Validator for files.
    QSharedPointer<FileValidator> validator_;
};

#endif // FILEINTERFACE_H
