//-----------------------------------------------------------------------------
// File: Bank.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 27.05.2026
//
// Description:
// Describes the ipxact:bank element.
//-----------------------------------------------------------------------------
#pragma once

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/common/NameGroup.h>
#include <IPXACTmodels/Component/MemoryBlockBase.h>
#include <IPXACTmodels/Component/AddressBlock.h>
#include <IPXACTmodels/Component/SubSpaceMap.h>
#include <IPXACTmodels/Component/AccessHandle.h>
#include <IPXACTmodels/Component/AccessPolicy.h>

class IPXACTMODELS_EXPORT Bank : public MemoryBlockBase
{
public:

    enum Alignment
    {
        PARALLEL,
        SERIAL,
        UNKNOWN
    };

    //! The constructor.
    Bank(QString const& name = QString());

    //! The copy constructor.
    Bank(Bank const& other);

    //! The assignment operator.
    Bank& operator=(Bank const& other);

    //! The destructor.
    virtual ~Bank() = default;

    /*!
     *  Clone this bank and return pointer to the copy.
     *
     *    @return Pointer to the cloned bank.
     */
    QSharedPointer<MemoryBlockBase> clone() const override;

    /*! 
     *  Set the bank alignment.
     *
     *    @param [in] alignment  The bank alignment.
     */
    void setAlignment(Alignment const& alignment);

    /*! 
     *  Get the bank alignment.
     *
     *    @return The bank alignment.
     */
    Alignment getAlignment() const;

    /*! 
     *  Get the address blocks contained in the bank.
     *
     *    @return Shared pointer to the list of address blocks.
     */
    QSharedPointer<QList<QSharedPointer<AddressBlock> > > getAddressBlocks() const;

    /*! 
     *  Set the address blocks contained in the bank.
     *
     *    @param [in] addressBlocks  Shared pointer to the address block list.
     */
    void setAddressBlocks(QSharedPointer<QList<QSharedPointer<AddressBlock> > > addressBlocks);

    /*! 
     *  Get the nested banks contained in this bank.
     *
     *    @return Shared pointer to the list of banks.
     */
    QSharedPointer<QList<QSharedPointer<Bank> > > getBanks() const;

    /*! 
     *  Set the nested banks contained in this bank.
     *
     *    @param [in] banks  Shared pointer to the bank list.
     */
    void setBanks(QSharedPointer<QList<QSharedPointer<Bank> > > banks);

    /*! 
     *  Get the subspace maps contained in the bank.
     *
     *    @return Shared pointer to the list of subspace maps.
     */
    QSharedPointer<QList<QSharedPointer<SubSpaceMap> > > getSubSpaceMaps() const;

    /*! 
     *  Set the subspace maps contained in the bank.
     *
     *    @param [in] subspaceMaps  Shared pointer to the subspace map list.
     */
    void setSubSpaceMaps(QSharedPointer<QList<QSharedPointer<SubSpaceMap> > > subspaceMaps);

    void setBankDefinitionRef(QString const& bankDefinitionRef);
    QString getBankDefinitionRef() const;

    void setTypeDefinitionsRef(QString const& typeDefinitionsRef);
    QString getTypeDefinitionsRef() const;

    static QString alignmentToString(Alignment alignment);
    static Alignment stringToAlignment(QString const& alignment);

private:

    // --------------------------------- 
    // Should contain one of the following:

    //! Address blocks contained in the bank.
    QSharedPointer<QList<QSharedPointer<AddressBlock> > > addressBlocks_ =
        QSharedPointer<QList<QSharedPointer<AddressBlock> > >(new QList<QSharedPointer<AddressBlock> >());

    //! Banks contained in the bank.
    QSharedPointer<QList<QSharedPointer<Bank> > > banks_ =
        QSharedPointer<QList<QSharedPointer<Bank> > >(new QList<QSharedPointer<Bank> >());

    //! Subspace maps contained in the bank.
    QSharedPointer<QList<QSharedPointer<SubSpaceMap> > > subspaceMaps_ =
        QSharedPointer<QList<QSharedPointer<SubSpaceMap> > >(new QList<QSharedPointer<SubSpaceMap> >());

    // ---------------------------------

    //! Bank definition reference.
    QString bankDefinitionRef_;
    QString typeDefinitionsRef_;
    
    //! The memory bank alignment.
    Alignment alignment_ = UNKNOWN;
};