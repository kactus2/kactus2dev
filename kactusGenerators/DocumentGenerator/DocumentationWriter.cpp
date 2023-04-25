#include "DocumentationWriter.h"

#include <IPXACTmodels/Component/AddressBlock.h>
#include <IPXACTmodels/Component/Register.h>
#include <IPXACTmodels/Component/MemoryMap.h>
#include <IPXACTmodels/Component/Field.h>

#include <KactusAPI/include/ExpressionFormatter.h>

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

const QStringList DocumentationWriter::DESIGN_INSTANCE_HEADERS = {
    QStringLiteral("Instance name"),
    QStringLiteral("Component type"),
    QStringLiteral("Configurable values"),
    QStringLiteral("Active view"),
    QStringLiteral("Description")
};

DocumentationWriter::DocumentationWriter(ExpressionFormatter* formatter, ExpressionFormatterFactory* expressionFormatterFactory) :
    expressionFormatter_(formatter),
    expressionFormatterFactory_(expressionFormatterFactory)
{
}

void DocumentationWriter::setTargetPath(QString const& path)
{
    targetPath_ = path;
}

QString DocumentationWriter::getTargetPath() const
{
    return targetPath_;
}

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

QList<QSharedPointer<Register>> DocumentationWriter::getAddressBlockRegisters(QSharedPointer<AddressBlock> addressBlock) const
{
    QList <QSharedPointer <Register> > registers;
    for (auto const& registerModelItem : *addressBlock->getRegisterData())
    {
        QSharedPointer <Register> registerItem = registerModelItem.dynamicCast<Register>();

        if (registerItem)
        {
            registers.append(registerItem);
        }
    }

    return registers;
}

QString DocumentationWriter::getFieldResetInfo(QSharedPointer<Field> field) const
{
    QString resetInfo = "";

    for (auto const& singleRest : *field->getResets())
    {
        if (singleRest != field->getResets()->first())
        {
            resetInfo.append("<br>");
        }

        QString resetTypeReference = singleRest->getResetTypeReference();
        if (resetTypeReference.isEmpty())
        {
            resetTypeReference = QLatin1String("HARD");
        }

        QString resetValue = expressionFormatter_->formatReferringExpression(singleRest->getResetValue());

        resetInfo.append(resetTypeReference + " : " + resetValue);
    }

    return resetInfo;
}

QSharedPointer<ExpressionFormatter> DocumentationWriter::createDesignInstanceFormatter(
    QSharedPointer<Design> design, QSharedPointer<Component> component)
{
    return QSharedPointer<ExpressionFormatter>(
        expressionFormatterFactory_->createDesignInstanceFormatter(component, design));
}
