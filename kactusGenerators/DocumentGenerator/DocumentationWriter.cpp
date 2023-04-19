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

DocumentationWriter::DocumentationWriter(ExpressionFormatter* formatter) :
    expressionFormatter_(formatter)
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