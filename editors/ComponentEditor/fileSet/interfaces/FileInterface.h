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

#include <editors/ComponentEditor/common/interfaces/NameGroupInterface.h>

class File;
class FileValidator;

#include <QSharedPointer>
#include <QVector>
#include <QMap>

//-----------------------------------------------------------------------------
//! Interface for editing files.
//-----------------------------------------------------------------------------
class FileInterface: public NameGroupInterface
{

public:

    /*!
     *  The constructor.
     *
     *      @param [in] validator   Validator for files.
     */
    FileInterface(QSharedPointer<FileValidator> validator);
	
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
     *  Add a new field.
     *
     *      @param [in] row             Row of the new field.
     *      @param [in] newFieldName    Name of the new field.
     */
    void addFile(int const& row, std::string const& newFileName = std::string(""));

    /*!
     *  Remove the selected field.
     *
     *      @param [in] FieldName   Name of the selected field.
     *
     *      @return True, if successful, false otherwise.
     */
    bool removeFile(std::string const& fileName);

private:

    /*!
     *  Get the selected field.
     *
     *      @param [in] fieldName   Name of the selected field.
     *
     *      @return The selected field.
     */
    QSharedPointer<File> getFile(std::string const& fileName) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! List of the contained files.
    QSharedPointer<QList<QSharedPointer<File> > > files_;

    //! Validator for files.
    QSharedPointer<FileValidator> validator_;
};

#endif // FILEINTERFACE_H
