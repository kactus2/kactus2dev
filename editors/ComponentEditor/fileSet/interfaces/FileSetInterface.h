//-----------------------------------------------------------------------------
// File: FileSetInterface.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 01.09.2020
//
// Description:
// Interface for editing file sets.
//-----------------------------------------------------------------------------

#ifndef FILESETINTERFACE_H
#define FILESETINTERFACE_H

#include <common/Global.h>

#include <editors/ComponentEditor/common/interfaces/ParameterizableInterface.h>
#include <editors/ComponentEditor/common/interfaces/NameGroupInterface.h>

#include <QSharedPointer>
#include <QVector>
#include <QMap>

class FileSet;
class FileSetValidator;
class FileInterface;
class FileBuilderInterface;

//-----------------------------------------------------------------------------
//! Interface for editing filesets.
//-----------------------------------------------------------------------------
class KACTUS2_API FileSetInterface: public ParameterizableInterface, public NameGroupInterface
{

public:

    /*!
     *  The constructor.
     *
     *      @param [in] validator               Validator for files.
     *      @param [in] expressionParser        Parser for expressions.
     *      @param [in] expressionFormatter     Formatter for expressions.
     *      @param [in] fileInterface           Sub interface for files.
     *      @param [in] fileBuilderInterface    Sub interface for file builders.
     */
    FileSetInterface(QSharedPointer<FileSetValidator> validator,
        QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        FileInterface* fileInterface,
        FileBuilderInterface* fileBuilderInterface);

	/*!
     *  The destructor.
     */
    virtual ~FileSetInterface() = default;

    /*!
     *  Set available file sets.
     *
     *      @param [in] newFileSets     The new file sets.
     */
    void setFileSets(QSharedPointer<QList<QSharedPointer<FileSet> > > newFileSets);

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
     *  Get the groups of the selected file set.
     *
     *      @param [in] fileSetName     Name of the selected file set.
     *
     *      @return Groups of the selected file set.
     */
    std::vector<std::string> getGroups(std::string const& fileSetName) const;
    
    /*!
     *  Set new groups for the selected item.
     *
     *      @param [in] fileSetName     Name of the selected file set.
     *      @param [in] newGroups       The new groups.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setGroups(std::string const& fileSetName, std::vector<std::string> const& newGroups);

    /*!
     *  Get the dependencies of the selected file set.
     *
     *      @param [in] fileSetName     Name of the selected file set.
     *
     *      @return Dependencies of the selected file set.
     */
    std::vector<std::string> getDependencies(std::string const& fileSetName) const;

    /*!
     *  Set new dependencies for the selected item.
     *
     *      @param [in] fileSetName         Name of the selected file set.
     *      @param [in] newDependencies     The new dependencies.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setDependencies(std::string const& fileSetName, std::vector<std::string> const& newDependencies);

    /*!
     *  Add a new file set.
     *
     *      @param [in] row             Row of the new file set.
     *      @param [in] newFileSetName  Name of the new file set.
     */
    void addFileSet(int const& row, std::string const& newFileSetName = std::string(""));

    /*!
     *  Remove the selected file set.
     *
     *      @param [in] fileSetName     Name of the selected file set.
     *
     *      @return True, if successful, false otherwise.
     */
    bool removeFileSet(std::string const& fileSetName);

    /*!
     *  Get the interface for accessing files.
     *
     *      @return Interface for accessing files.
     */
    FileInterface* getFileInterface() const;

    /*!
     *  Get the interface for accessing file builders.
     *
     *      @return Interface for accessing file builders.
     */
    FileBuilderInterface* getFileBuilderInterface() const;

    /*!
     *  Calculate all the references to the selected ID in the selected item.
     *
     *      @param [in] itemName    Name of the selected item.
     *      @param [in] valueID     The selected ID.
     *
     *      @return Number of references to the selected ID in the selected item.
     */
    virtual int getAllReferencesToIdInItem(const std::string& itemName, std::string const& valueID) const override
        final;

    /*!
     *  Get the expressions in the selected file sets.
     *
     *      @param [in] fileSetNames    Names of the selected file sets.
     *
     *      @return The expressions of the selected file sets.
     */
    std::vector<std::string> getExpressionsInSelectedFileSets(std::vector<std::string> fileSetNames) const;

    /*!
     *  Check if the selected file set exists.
     *
     *      @param [in] fileSetName     Name of the selected file set.
     *
     *      @return True, if the file set exists, false otherwise.
     */
    bool fileSetExists(std::string const& fileSetName) const;

private:

    /*!
     *  Get the selected file set.
     *
     *      @param [in] fileSetName     Name of the selected file set.
     *
     *      @return The selected file set.
     */
    QSharedPointer<FileSet> getFileSet(std::string const& fileSetName) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! List of the contained file sets.
    QSharedPointer<QList<QSharedPointer<FileSet> > > fileSets_;

    //! Validator for file sets.
    QSharedPointer<FileSetValidator> validator_;

    //! Sub interface for accessing files.
    FileInterface* fileInterface_;

    //! Sub interface for accessing file builders.
    FileBuilderInterface* fileBuilderInterface_;
};

#endif // FILESETINTERFACE_H
