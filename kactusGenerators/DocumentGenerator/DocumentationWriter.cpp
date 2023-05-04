// File: DocumentationWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Anton Hagqvist
// Date: 12.4.2023
//
// Description:
// Base class for documentation writers
//-----------------------------------------------------------------------------


#include "DocumentationWriter.h"

#include <IPXACTmodels/Component/AddressBlock.h>
#include <IPXACTmodels/Component/Register.h>
#include <IPXACTmodels/Component/RegisterFile.h>
#include <IPXACTmodels/Component/MemoryMap.h>
#include <IPXACTmodels/Component/Field.h>

#include <KactusAPI/include/ExpressionFormatter.h>
#include <KactusAPI/include/LibraryInterface.h>
#include <KactusAPI/include/ListParameterFinder.h>

#include <QList>
#include <QString>
#include <QSharedPointer>
#include <QTextStream>

const QStringList DocumentationWriter::ADDRESS_BLOCK_HEADERS = {
    QStringLiteral("Usage"),
    QStringLiteral("Base address [AUB]"),
    QStringLiteral("Range [AUB]"),
    QStringLiteral("Width [AUB]"),
    QStringLiteral("Access"),
    QStringLiteral("Volatile")
};

const QStringList DocumentationWriter::DEFAULT_FILE_BUILDER_HEADERS = {
    QStringLiteral("File type"),
    QStringLiteral("Command"),
    QStringLiteral("Flags"),
    QStringLiteral("Replace default flags")
};

const QStringList DocumentationWriter::FIELD_HEADERS = {
    QStringLiteral("Field name"),
    QStringLiteral("Offset [bits]"),
    QStringLiteral("Width [bits]"),
    QStringLiteral("Volatile"),
    QStringLiteral("Access"),
    QStringLiteral("Resets"),
    QStringLiteral("Description")
};

const QStringList DocumentationWriter::FILE_HEADERS = {
    QStringLiteral("File name"),
    QStringLiteral("Logical name"),
    QStringLiteral("Build command"),
    QStringLiteral("Build flags"),
    QStringLiteral("Specified file types"),
    QStringLiteral("Description")
};

const QStringList DocumentationWriter::PARAMETER_HEADERS = {
    QStringLiteral("Name"),
    QStringLiteral("Type"),
    QStringLiteral("Value"),
    QStringLiteral("Resolve"),
    QStringLiteral("Bit vector left"),
    QStringLiteral("Bit vector right"),
    QStringLiteral("Array left"),
    QStringLiteral("Array right"),
    QStringLiteral("Description")
};

const QStringList DocumentationWriter::PORT_HEADERS = {
    QStringLiteral("Name"),
    QStringLiteral("Direction"),
    QStringLiteral("Left bound"),
    QStringLiteral("Right bound"),
    QStringLiteral("Port type"),
    QStringLiteral("Type definition"),
    QStringLiteral("Default value"),
    QStringLiteral("Array left"),
    QStringLiteral("Array right"),
    QStringLiteral("Description")
};

const QStringList DocumentationWriter::REGISTER_HEADERS = {
    QStringLiteral("Offset [AUB]"),
    QStringLiteral("Size [bits]"),
    QStringLiteral("Dimension"),
    QStringLiteral("Volatile"),
    QStringLiteral("Access")
};

const QStringList DocumentationWriter::REGISTER_FILE_HEADERS = {
    QStringLiteral("Description"),
    QStringLiteral("Offset [AUB]"),
    QStringLiteral("Range [AUB]"),
    QStringLiteral("Dimension"),
};

const QStringList DocumentationWriter::DESIGN_INSTANCE_HEADERS = {
    QStringLiteral("Instance name"),
    QStringLiteral("Component type"),
    QStringLiteral("Configurable values"),
    QStringLiteral("Active view"),
    QStringLiteral("Description")
};

//-----------------------------------------------------------------------------
// Function: DocumentationWriter::DocumentationWriter()
//-----------------------------------------------------------------------------
DocumentationWriter::DocumentationWriter(ExpressionFormatter* formatter, ExpressionFormatterFactory* expressionFormatterFactory) :
    expressionFormatter_(formatter),
    expressionFormatterFactory_(expressionFormatterFactory),
    targetPath_()
{
}

//-----------------------------------------------------------------------------
// Function: DocumentationWriter::setTargetPath()
//-----------------------------------------------------------------------------
void DocumentationWriter::setTargetPath(QString const& path)
{
    targetPath_ = path;
}

//-----------------------------------------------------------------------------
// Function: DocumentationWriter::getTargetPath()
//-----------------------------------------------------------------------------
QString DocumentationWriter::getTargetPath() const
{
    return targetPath_;
}

//-----------------------------------------------------------------------------
// Function: DocumentationWriter::getMemoryMapAddressBlocks()
//-----------------------------------------------------------------------------
QList<QSharedPointer<AddressBlock>> DocumentationWriter::getMemoryMapAddressBlocks(QSharedPointer<MemoryMap> memoryMap) const
{
    QList<QSharedPointer <AddressBlock> > addressBlocks;
    for (auto const& memoryMapItem : *memoryMap->getMemoryBlocks())
    {
        QSharedPointer<AddressBlock> addressItem = memoryMapItem.dynamicCast<AddressBlock>();

        if (addressItem)
        {
            addressBlocks.append(addressItem);
        }
    }

    return addressBlocks;
}

//-----------------------------------------------------------------------------
// Function: DocumentationWriter::getAddressBlockRegisters()
//-----------------------------------------------------------------------------
QList<QSharedPointer<Register> > DocumentationWriter::getRegisters(
    QSharedPointer<QList<QSharedPointer<RegisterBase> > > registerData) const
{
    QList <QSharedPointer <Register> > registers;
    for (auto const& registerModelItem : *registerData)
    {
        QSharedPointer <Register> registerItem = registerModelItem.dynamicCast<Register>();

        if (registerItem)
        {
            registers.append(registerItem);
        }
    }

    return registers;
}

QList<QSharedPointer<RegisterFile> > DocumentationWriter::getRegisterFiles(
    QSharedPointer<QList<QSharedPointer<RegisterBase> > > registerData) const
{
    QList<QSharedPointer<RegisterFile > > registerFiles;

    for (auto const& registerDataItem : *registerData)
    {
        if (auto registerFileItem = registerDataItem.dynamicCast<RegisterFile>(); registerFileItem)
        {
            registerFiles.append(registerFileItem);
        }
    }
    
    return registerFiles;
}

//-----------------------------------------------------------------------------
// Function: DocumentationWriter::getFieldResetInfo()
//-----------------------------------------------------------------------------
QString DocumentationWriter::getFieldResetInfo(QSharedPointer<Field> field, QString const& separator) const
{
    QString resetInfo = "";

    for (auto const& singleReset : *field->getResets())
    {
        if (singleReset != field->getResets()->first())
        {
            resetInfo.append(separator);
        }

        QString resetTypeReference = singleReset->getResetTypeReference();
        if (resetTypeReference.isEmpty())
        {
            resetTypeReference = QLatin1String("HARD");
        }

        QString resetValue = expressionFormatter_->formatReferringExpression(singleReset->getResetValue());

        resetInfo.append(resetTypeReference + " : " + resetValue);
    }

    return resetInfo;
}

//-----------------------------------------------------------------------------
// Function: DocumentationWriter::createDesignInstanceFormatter()
//-----------------------------------------------------------------------------
QSharedPointer<ExpressionFormatter> DocumentationWriter::createDesignInstanceFormatter(
    QSharedPointer<Design> design, QSharedPointer<Component> component)
{
    return QSharedPointer<ExpressionFormatter>(
        expressionFormatterFactory_->createDesignInstanceFormatter(component, design));
}

void DocumentationWriter::writeReferencedComponentInstantiation(QTextStream& stream, QSharedPointer<ComponentInstantiation> instantiation, QSharedPointer<ExpressionFormatter> instantiationFormatter, ParameterList moduleParameters, ParameterList parameters)
{
    writeImplementationDetails(stream, instantiation);
    writeFileSetReferences(stream, instantiation);
    writeFileBuildCommands(stream, instantiation, instantiationFormatter.data());
    writeParameterTable(stream, QString("Module parameters:"),
        moduleParameters, instantiationFormatter.data());
    writeParameterTable(stream, QString("Parameters:"), parameters,
        instantiationFormatter.data());
}

void DocumentationWriter::writeReferencedDesignConfigurationInstantiation(QTextStream& stream,
    QSharedPointer<ListParameterFinder> configurationFinder,
    QSharedPointer<DesignConfigurationInstantiation> instantiation,
    QSharedPointer<ExpressionFormatter> instantiationFormatter, LibraryInterface* libraryHandler)
{
    if (auto const& configurationVLNV = instantiation->getDesignConfigurationReference(); configurationVLNV)
    {
        QSharedPointer<Document> configurationDocument = libraryHandler->getModel(*configurationVLNV);
        if (configurationDocument)
        {
            QSharedPointer<DesignConfiguration> configuration =
                configurationDocument.dynamicCast<DesignConfiguration>();

            if (configuration)
            {
                configurationFinder->setParameterList(configuration->getParameters());

                QString header = QString("Parameters of the referenced design configuration %1:").
                    arg(configurationVLNV->toString());
                QSharedPointer<ExpressionFormatter> configurationFormatter(new ExpressionFormatter(configurationFinder));

                writeParameterTable(stream, header, configuration->getParameters(), configurationFormatter.data());
                writeConfigurableElementValues(stream,
                    instantiation->getDesignConfigurationReference(), instantiationFormatter.data());
            }
        }
    }

    writeParameterTable(stream, QString("Design configuration instantiation parameters:"),
        instantiation->getParameters(), instantiationFormatter.data());
}

void DocumentationWriter::writeReferencedDesignInstantiation(QTextStream& stream,
    QSharedPointer<ConfigurableVLNVReference> designVLNV, QSharedPointer<Design> instantiatedDesign,
    ExpressionFormatter* designFormatter, QSharedPointer<ExpressionFormatter> instantiationFormatter)
{
    QString header = QString("Parameters of the referenced design %1:").arg(designVLNV->toString());
    writeParameterTable(stream, header, instantiatedDesign->getParameters(), designFormatter);

    writeConfigurableElementValues(stream, designVLNV, instantiationFormatter.data());
}

void DocumentationWriter::writeAddressBlockInfo(QTextStream& stream, QSharedPointer<AddressBlock> addressBlock)
{
    QStringList addressBlockInfoValues(QStringList()
        << addressBlock->description()
        << General::usage2Str(addressBlock->getUsage())
        << expressionFormatter_->formatReferringExpression(addressBlock->getBaseAddress())
        << expressionFormatter_->formatReferringExpression(addressBlock->getRange())
        << expressionFormatter_->formatReferringExpression(addressBlock->getWidth())
        << AccessTypes::access2Str(addressBlock->getAccess())
        << addressBlock->getVolatile()
    );

    QStringList addressBlockInfoNames(QStringList()
        << "Description" << ADDRESS_BLOCK_HEADERS
    );

    writeInfoParagraph(stream, addressBlockInfoNames, addressBlockInfoValues);
}

void DocumentationWriter::writeSingleRegister(QTextStream& stream, QSharedPointer<Register> reg, QList<int> subHeaderNumbers, int& registerDataNumber)
{
    writeSubHeader(stream, subHeaderNumbers, QStringLiteral("Register ") + reg->name(), 3);

    QStringList registerInfoValues(QStringList()
        << reg->description()
        << expressionFormatter_->formatReferringExpression(reg->getAddressOffset())
        << expressionFormatter_->formatReferringExpression(reg->getSize())
        << expressionFormatter_->formatReferringExpression(reg->getDimension())
        << reg->getVolatile()
        << AccessTypes::access2Str(reg->getAccess())
    );

    QStringList registerInfoNames(QStringList()
        << QStringLiteral("Description")
        << REGISTER_HEADERS
    );

    writeInfoParagraph(stream, registerInfoNames, registerInfoValues);

    writeFields(stream, reg, subHeaderNumbers);
    ++registerDataNumber;
}

void DocumentationWriter::writeRegisterFileInfo(QTextStream& stream, QSharedPointer<RegisterFile> registerFile)
{
    QStringList registerInfoValues(QStringList()
        << registerFile->description()
        << expressionFormatter_->formatReferringExpression(registerFile->getAddressOffset())
        << expressionFormatter_->formatReferringExpression(registerFile->getRange())
        << expressionFormatter_->formatReferringExpression(registerFile->getDimension())
    );

    writeInfoParagraph(stream, REGISTER_FILE_HEADERS, registerInfoValues);
}

void DocumentationWriter::writeSingleField(QTextStream& stream, QSharedPointer<Field> field)
{
    QStringList fieldInfoTextValues(QStringList()
        << expressionFormatter_->formatReferringExpression(field->getBitOffset())
        << expressionFormatter_->formatReferringExpression(field->getBitWidth())
        << field->getVolatile().toString()
        << AccessTypes::access2Str(field->getAccess())
        << getFieldResetInfo(field, ", ")
        << field->description()
    );

    QStringList fieldInfoTextNames = FIELD_HEADERS;
    fieldInfoTextNames.pop_front();

    writeInfoParagraph(stream, fieldInfoTextNames, fieldInfoTextValues);

    writeFieldEnumerations(stream, field);
}