//-----------------------------------------------------------------------------
// File: Bank.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 27.05.2026
//
// Description:
// Describes the ipxact:bank element.
//-----------------------------------------------------------------------------

#include "Bank.h"

#include <IPXACTmodels/utilities/Copy.h>

Bank::Bank(QString const& name) :
    MemoryBlockBase(name)
{
}

Bank::Bank(Bank const& other) :
    MemoryBlockBase(other),
    alignment_(other.alignment_)
{
    Copy::copyList(other.addressBlocks_, addressBlocks_);
    Copy::copyList(other.banks_, banks_);
    Copy::copyList(other.subspaceMaps_, subspaceMaps_);
}

Bank & Bank::operator=(Bank const& other)
{
    if (this != &other)
    {
        MemoryBlockBase::operator=(other);

        addressBlocks_->clear();
        banks_->clear();
        subspaceMaps_->clear();

        Copy::copyList(other.addressBlocks_, addressBlocks_);
        Copy::copyList(other.banks_, banks_);
        Copy::copyList(other.subspaceMaps_, subspaceMaps_);

        alignment_ = other.alignment_;
    }

    return *this;
}

QSharedPointer<MemoryBlockBase> Bank::clone() const
{
    return QSharedPointer<MemoryBlockBase>(new Bank(*this));
}

void Bank::setAlignment(Alignment const& alignment)
{
    alignment_ = alignment;
}

Bank::Alignment Bank::getAlignment() const
{
    return alignment_;
}

QSharedPointer<QList<QSharedPointer<AddressBlock> > > Bank::getAddressBlocks() const
{
    return addressBlocks_;
}

void Bank::setAddressBlocks(QSharedPointer<QList<QSharedPointer<AddressBlock> > > addressBlocks)
{
    addressBlocks_ = addressBlocks;
}

QSharedPointer<QList<QSharedPointer<Bank> > > Bank::getBanks() const
{
    return banks_;
}

void Bank::setBanks(QSharedPointer<QList<QSharedPointer<Bank> > > banks)
{
    banks_ = banks;
}

QSharedPointer<QList<QSharedPointer<SubSpaceMap> > > Bank::getSubSpaceMaps() const
{
    return subspaceMaps_;
}

void Bank::setSubSpaceMaps(QSharedPointer<QList<QSharedPointer<SubSpaceMap> > > subspaceMaps)
{
    subspaceMaps_ = subspaceMaps;
}

void Bank::setBankDefinitionRef(QString const& bankDefinitionRef)
{
    bankDefinitionRef_ = bankDefinitionRef;
}

QString Bank::getBankDefinitionRef() const
{
    return bankDefinitionRef_;
}

void Bank::setTypeDefinitionsRef(QString const& typeDefinitionsRef)
{
    typeDefinitionsRef_ = typeDefinitionsRef;
}

QString Bank::getTypeDefinitionsRef() const
{
    return typeDefinitionsRef_;
}

QString Bank::alignmentToString(Alignment alignment)
{
    switch (alignment)
    {
        case PARALLEL:
            return QStringLiteral("parallel");
        case SERIAL:
            return QStringLiteral("serial");
        default:
            return QString();
    }
}

Bank::Alignment Bank::stringToAlignment(QString const& alignment)
{
    if (alignment.toLower() == QStringLiteral("parallel"))
    {
        return Alignment::PARALLEL;
    }
    else if (alignment.toLower() == QStringLiteral("serial"))
    {
        return Alignment::SERIAL;
    }
    else
    {
        return Alignment::UNKNOWN;
    }
}
