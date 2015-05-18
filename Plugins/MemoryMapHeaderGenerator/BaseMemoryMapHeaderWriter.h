//-----------------------------------------------------------------------------
// File: BaseMemoryMapHeaderWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 07.05.2015
//
// Description:
// The base implementation for memory map header writer.
//-----------------------------------------------------------------------------

#ifndef BASEMEMORYMAPHEADERWRITER_H
#define BASEMEMORYMAPHEADERWRITER_H

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/addressblock.h>
#include <IPXACTmodels/register.h>
#include <IPXACTmodels/memorymap.h>

#include <QFileInfo>
#include <QObject>
#include <QSharedPointer>
#include <QString>
#include <QFileInfo>
#include <QList>
#include <QTextStream>
#include <QDir>

class IPluginUtility;
class PluginUtilityAdapter;
class ExpressionParser;
class ExpressionFormatter;
class ParameterFinder;

//-----------------------------------------------------------------------------
//! The base implementation for memory map header writer.
//-----------------------------------------------------------------------------
class BaseMemoryMapHeaderWriter
{

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] utility     Interface which offers core utilities for plugins.
	 */
	BaseMemoryMapHeaderWriter(IPluginUtility* utility);

    /*!
     *  The destructor.
     */
    virtual ~BaseMemoryMapHeaderWriter();

protected:

    /*!
     *  Check that the directory structure exists for the file.
     *
     *      @param [in] dir     The directory for the file.
     */
    void checkDirectoryStructure(QDir dir);

    /*!
     *  Display an error message when opening a file.
     *
     *      @param [in] absoluteFilePath    The file path of the file that couldn't be opened.
     */
    void openFileErrorMessage(QString const& absoluteFilePath);

    /*!
     *  Write the top part of the header file.
     *
     *      @param [in] stream          The text stream used for writing.
     *      @param [in] fileName        The name of the file being written.
     *      @param [in] header          The header guard of the file.
     *      @param [in] description     The description of the file.
     */
    void writeTopOfHeaderFile(QTextStream& stream, QString const& fileName, QString const& header,
        QString const& description);

	/*!
	 *  Add a generated file to the file sets of a component.
	 *
	 *      @param [in] component       Pointer to the component where the file is added to.
	 *      @param [in] fileInfo        Contains the info on the generated file.
	 *      @param [in] filesetName     The name of the file set to add the header file to.
	 *      @param [in] swViewNames     Contains the names of the sw views to add the file set reference to.
	 *      @param [in] instanceId      The id of the instance which is used when generating global headers.
	 */
	void addHeaderFile(QSharedPointer<Component> component, const QFileInfo& fileInfo, const QString& filesetName,
		const QStringList& swViewNames = QStringList(), const QString& instanceId = QString()) const;

    /*!
     *  Display the created memory map header file for the user.
     *
     *      @param [in] filePath    The path to the created file.
     */
    virtual void displayMemoryMapHeader(QString const& filePath) const;

    /*!
     *  Write the registers contained in a memory map.
     *
     *      @param [in] finder                  The parameter finder containing the referenced parameters.
     *      @param [in] containingMemoryMap     The memory map containing the registers.
     *      @param [in] stream                  The text stream to write into.
     *      @param [in] useAddressBlockID       Tells if an address block id is needed for the register name.
     *      @param [in] offset                  The current offset.
     *      @param [in] idString                Helps to identify non-unique registers.
     */
    void writeRegisterFromMemoryMap(QSharedPointer<ParameterFinder> finder,
        QSharedPointer<MemoryMap> containingMemoryMap, QTextStream& stream, bool useAddressBlockID, quint64 offset,
        QString const& idString = QString());

    /*!
     *  Write registers contained in an address block.
     *
     *      @param [in] expressionParser        The expression parser.
     *      @param [in] formatter               The expression formatter, for changing parameter ids into names.
     *      @param [in] currentAddressBlock     Pointer to the address block.
     *      @param [in] stream                  The text stream to write into.
     *      @param [in] useAddressBlockID       Tells if an address block id is needed for the register name.
     *      @param [in] offset                  The current offset.
     *      @param [in] idString                Helps to identify non-unique registers.
     */
    void writeRegistersFromAddressBlock(QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<ExpressionFormatter> formatter, QSharedPointer<AddressBlock> currentAddressBlock,
        QTextStream& stream, bool useAddressBlockID, quint64 offset, QString const& idString = QString());

    /*!
     *  Write the register information to a text stream.
     *
     *      @param [in] expressionParser    The expression parser.
     *      @param [in] formatter           The expression formatter, used to change parameter ids into names.
     *      @param [in] currentRegister     The current register.
     *      @param [in] stream              The text stream to write into.
     *      @param [in] addressBlockOffset  The current offset.
     *      @param [in] idString            Helps to identify non-unique registers.
     */
    void writeRegister(QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<ExpressionFormatter> formatter, QSharedPointer<Register> currentRegister, QTextStream& stream,
        quint64 addressBlockOffset, QString const& idString = QString());

    /*!
     *  Write the memory names and given addresses.
     *
     *      @param [in] finder              The parameter finder containing the referenced parameters.
     *      @param [in] targetMemoryMap     The used memory map.
     *      @param [in] stream              The text stream to write into.
     *      @param [in] offset              The current offset.
     *      @param [in] idString            Helps to identify non-unique elements.
     */
    void writeMemoryAddresses(QSharedPointer<ParameterFinder> finder, QSharedPointer<MemoryMap> targetMemoryMap,
        QTextStream& stream, quint64 offset, QString const& idString = QString()) const;

    /*!
     *  Gets the ending address of an address block.
     *
     *      @param [in] targetAddressBlock  The address block being examined.
     *      @param [in] expressionParser    The expression parser.
     *
     *      @return The ending address of the address block.
     */
    QString getAddressBlockLastAddress(QSharedPointer<AddressBlock> targetAddressBlock,
        QSharedPointer<ExpressionParser> expressionParser) const;
    
    /*!
     *  Get the parsed value for a given expression.
     *
     *      @param [in] expression  The expression to be parsed.
     *      @param [in] finder      The parameter finder used in the parser.
     *
     *      @return The parsed value for the given expression.
     */
    QString parsedValueFor(QString const& expression, QSharedPointer<ParameterFinder> finder) const;

    /*!
     *  Inform of the start of the generator.
     */
    void informStartOfGeneration() const;

    /*!
     *  Inform when the writing has been finished.
     *
     *      @param [in] outputFile  The name of the output file.
     */
    void informWritingFinished(QString const& outputFile) const;

    /*!
     *  Inform of the generation finish.
     */
    void informGenerationComplete() const;

    /*!
     *  Inform of the generation abort.
     */
    void informGenerationAbort() const;

    //! The interface which offers core utilities for plugins.
    IPluginUtility* utility_;

private:

	//! No copying
    BaseMemoryMapHeaderWriter(const BaseMemoryMapHeaderWriter& other);

	//! No assignment
    BaseMemoryMapHeaderWriter& operator=(const BaseMemoryMapHeaderWriter& other);

};

#endif // BASEMEMORYMAPHEADERWRITER_H
