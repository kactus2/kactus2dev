//-----------------------------------------------------------------------------
// File: FileBuilderInterface.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 03.09.2020
//
// Description:
// Interface for editing file builders.
//-----------------------------------------------------------------------------

#ifndef FILEBUILDERINTERFACE_H
#define FILEBUILDERINTERFACE_H

#include <editors/ComponentEditor/common/interfaces/ParameterizableInterface.h>
#include <editors/ComponentEditor/common/interfaces/CommonInterface.h>

#include <QSharedPointer>
#include <QVector>
#include <QMap>

class FileBuilder;

//-----------------------------------------------------------------------------
//! Interface for editing file builders.
//-----------------------------------------------------------------------------
class FileBuilderInterface : public ParameterizableInterface, public CommonInterface
{

public:

    /*!
     *  The constructor.
     *
     *      @param [in] expressionParser        Parser for expressions.
     *      @param [in] expressionFormatter     Formatter for expressions.
     */
    FileBuilderInterface(QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<ExpressionFormatter> expressionFormatter);

	/*!
     *  The destructor.
     */
    virtual ~FileBuilderInterface() = default;

    /*!
     *  Set available file builders.
     *
     *      @param [in] newFileBuilders     The new file builders.
     */
    void setFileBuilders(QSharedPointer<QList<QSharedPointer<FileBuilder> > > newFileBuilders);

    /*!
     *  Get the number of available items.
     *
     *      @return Number of available items.
     */
    virtual int itemCount() const override final;

    /*!
     *  Validates the contained items.
     *
     *      @return True, if all the items are valid, false otherwise.
     */
    virtual bool validateItems() const;

    /*!
     *  Get the names of the available items.
     *
     *      @return Names of the available items.
     */
    virtual std::vector<std::string> getItemNames() const override final;

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
     *  Get the expressions in the selected file builders.
     *
     *      @param [in] fileBuilderNames    Names of the selected file builders.
     *
     *      @return The expressions of the selected file builders.
     */
    std::vector<std::string> getExpressionsInSelectedFileBuilders(std::vector<std::string> fileBuilderNames) const;

    /*!
     *  Add a new file builder.
     *
     *      @param [in] row                 Row of the new file builder.
     *      @param [in] newFileBuilderName  Name of the new file builder.
     */
    void addFileBuilder(int const& row, std::string const& newFileBuilderName = std::string(""));

    /*!
     *  Remove the selected file builder.
     *
     *      @param [in] fileBuilderName     Name of the selected file builder.
     *
     *      @return True, if successful, false otherwise.
     */
    bool removeFileBuilder(std::string const& fileBuilderName);

    /*!
     *  Get the file type of the indexed file builder.
     *
     *      @param [in] itemIndex   Index of the selected file builder.
     *
     *      @return File type of the selected file builder.
     */
    std::string getIndexedFileType(int const& itemIndex);

    /*!
     *  Set a new file type for the selected item.
     *
     *      @param [in] currentFileType     File type of the selected item.
     *      @param [in] newFileType         The new file type for the item.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setFileType(std::string const& currentFileType, std::string const& newFileType);

    /*!
     *  Get the command of the selected item.
     *
     *      @param [in] fileType    File type of the selected item.
     *
     *      @return Command of the selected item.
     */
    std::string getCommand(std::string const& fileType) const;

    /*!
     *  Set a new command for the selected item.
     *
     *      @param [in] fileType    File type of the selected item.
     *      @param [in] newCommand  The new command for the item.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setCommand(std::string const& fileType, std::string const& newCommand);

    /*!
     *  Get the flags of the selected item.
     *
     *      @param [in] fileType    File type of the selected item.
     *
     *      @return Flags of the selected item.
     */
    std::string getFlags(std::string const& fileType) const;

    /*!
     *  Set new flags value for the selected item.
     *
     *      @param [in] fileType    File type of the selected item.
     *      @param [in] newFlags    The new flags value.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setFlags(std::string const& fileType, std::string const& newFlags);

    /*!
     *  Get the calculated replace default flags value of the selected item.
     *
     *      @param [in] fileType    The selected file type.
     *      @param [in] baseNumber  Base for displaying the value.
     *
     *      @return Calculated replace default flags value of the selected item.
     */
    std::string getReplaceDefaultFlagsValue(std::string const& fileType, int const& baseNumber = 0) const;

    /*!
     *  Get the formatted replace default flags expression of the selected item.
     *
     *      @param [in] fileType    The selected file type.
     *
     *      @return Formatted replace default flags expression of the selected item.
     */
    std::string getReplaceDefaultFlagsFormattedExpression(std::string const& fileType) const;

    /*!
     *  Get the replace default flags expression of the selected item.
     *
     *      @param [in] fileType    The selected file type.
     *
     *      @return Replace default flags expression of the selected file.
     */
    std::string getReplaceDefaultFlagsExpression(std::string const& fileType) const;

    /*!
     *  Set a new replace default flags value for the selected item.
     *
     *      @param [in] fileType                The selected file type.
     *      @param [in] newReplaceDefaultFlags  New replace default flags value.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setReplaceDefaultFlags(std::string const& filetype, std::string const& newReplaceDefaultFlags);

private:

    /*!
     *  Get the selected file builder.
     *
     *      @param [in] fileBuilderName     Name of the selected file builder.
     *
     *      @return The selected file builder.
     */
    QSharedPointer<FileBuilder> getFileBuilder(std::string const& fileBuilderName) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! List of the contained file builders.
    QSharedPointer<QList<QSharedPointer<FileBuilder> > > fileBuilders_;
};

#endif // FILEBUILDCOMMANDINTERFACE_H
