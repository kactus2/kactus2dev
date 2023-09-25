//-----------------------------------------------------------------------------
// File: AddressBlock.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 29.09.2015
//
// Description:
// Describes the ipxact:addressBlock element.
//-----------------------------------------------------------------------------

#ifndef ADDRESSBLOCK_H
#define ADDRESSBLOCK_H

#include "MemoryBlockBase.h"

#include <IPXACTmodels/common/BooleanValue.h>
#include <IPXACTmodels/ipxactmodels_global.h>
#include <IPXACTmodels/generaldeclarations.h>
#include <IPXACTmodels/common/AccessTypes.h>

#include <QString>
#include <QMap>
#include <QSharedPointer>

class RegisterBase;
class AccessPolicy;

//-----------------------------------------------------------------------------
//! Describes the ipxact:addressBlock element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT AddressBlock: public MemoryBlockBase
{

public:

    /*!
     *  The constructor.
     *
     *      @param [in] name            Name of the address block.
     *      @param [in] baseAddress     The base address.
     */
    AddressBlock(QString const& name = QString(), QString const& baseAddress = QString());
	
    //! The copy constructor.
    AddressBlock(const AddressBlock &other);

	//! The assignment operator.
	AddressBlock &operator=(const AddressBlock &other);

	//! The destructor.
	~AddressBlock();

    /*!
     *  Clone this address block and return pointer to the copy.
     *
     *      @return Pointer to the cloned address block.
     */
    virtual QSharedPointer<MemoryBlockBase> clone() const;

    /*!
     *  Get the type identifier.
     *
     *      @return Address block type identifier.
     */
    QString getTypeIdentifier() const;

    /*!
     *  Set the type identifier.
     *
     *      @param [in] newTypeIdentifier   The new type identifier.
     */
    void setTypeIdentifier(QString const& newTypeIdentifier);

	/*!
     *  Get the address range of the address block.
	 *
	 *      @return The address range.
	 */
	QString getRange() const;

	/*!
     *  Set the address range of the address block.
	 *
	 *      @param [in] newRange    The new range value.
	 */
	void setRange(QString const& newRange);

	/*!
     *  Get the attributes linked to range element.
	 *
	 *      @return The range attributes.
	 */
    QMap<QString, QString> getRangeAttributes() const;
    
	/*!
     *  Set the attributes linked to range element.
	 *
	 *      @param [in] newRangeAttributes      The new range attributes.
	 */
    void setRangeAttributes(QMap<QString, QString> const& newRangeAttributes);

	/*!
     *  Get the bit width of a row in the address block.
	 *
	 *      @return The width of the address block.
	 */
    QString getWidth() const;

	/*!
     *  Set the bit width of a row in the address block.
	 *
	 *      @param [in] newWidth    The new width.
	 */
    void setWidth(QString const& newWidth);

	/*!
     *  Get the attributes linked to the width element.
	 *
	 *      return The attributes of the width element.
	 */
    QMap<QString, QString> getWidthAttributes() const;

	/*!
     *  Set the attributes linked to the width element.
	 *
	 *      @param [in] newWidthAttributes  The new attributes for the width element.
	 */
    void setWidthAttributes(QMap<QString, QString> const& newWidthAttributes);

	/*!
     *  Get the type of usage for the address block.
	 *
	 *      @return Usage.
	 */
	General::Usage getUsage() const;

	/*!
     *  Set the type of usage for the address block.
	 *
	 *      @param [in] newUsage    The new usage type.
	 */
	void setUsage(General::Usage newUsage);

	/*!
     *  Get the volatile information.
	 *
	 *      return Volatile information. Empty string means unspecified volatile state.
	 */
    QString getVolatile() const;

	/*!
     *  Set the volatile setting.
	 *
	 *      @param [in] newVolatileSetting  True for volatile address block, false otherwise.
	 */
    void setVolatile(bool newVolatileValue);

    /*!
     *  Set the volatile value to unspecified.
     */
    void clearVolatile();

	/*!
     *  Get the access information.
	 *
	 *      @return Accessibility of the address block data.
	 */
    AccessTypes::Access getAccess() const;

	/*!
     *  Set the accessibility of the address block.
	 *
	 *      @param [in] newAccess   The new access type of the address block.
	 */
    void setAccess(AccessTypes::Access newAccess);

	/*!
     *  Get the list containing the register elements of address block.
	 *
	 *      @return Pointer to a list containing the register elements.
 	 */
    QSharedPointer<QList<QSharedPointer<RegisterBase> > > getRegisterData() const;

    /*!
     *  Set the register elements.
     *
     *      @param [in] newRegisterData     The new register elements.
     */
    void setRegisterData(QSharedPointer<QList<QSharedPointer<RegisterBase> > > newRegisterData);
    
    /*!
     *  Get offsets from each of the address block's registers.
     *
     *      @return     A list of offsets from all the registers.
     */
    QStringList getAllRegisterOffsets() const;

    /*!
     *	Get the address block memory array.
     *  
     * 	    @return Pointer to the memory array of the address block.
     */
    QSharedPointer<MemoryArray> getMemoryArray() const;

    /*!
     *	Set the memory array of the address block.
     *  
     *      @param [in] newMemoryArray     The memory array to set.
     */
    void setMemoryArray(QSharedPointer<MemoryArray> newMemoryArray);

    /*!
     *	Get the misalignment allowed attribute.
     *	    
     * 	    @return The misalignment attribute.
     */
    QString getMisalignmentAllowed() const;

    /*!
     *	Set the misalignment allowed attribute.
     *  
     *      @param [in] newMisalignmentAllowed     New misalignment allowed value.
     */
    void setMisalignmentAllowed(bool newMisalignmentAllowed);

    /*!
     *	Get the address block definition reference.
     *  
     * 	    @return The address block definition reference.
     */
    QString getAddressBlockDefinitionRef() const;

    /*!
     *	Set the address block definition reference.
     *  
     *      @param [in] newAddressBlockDefinitionRef     The definition reference to set.
     */
    void setAddressBlockDefinitionRef(QString const& newAddressBlockDefinitionRef);

    /*!
     *	Get the type definitions reference.
     *  
     * 	    @return The referenced type definitions.
     */
    QString getTypeDefinitionsRef() const;

    /*!
     *	Set the type definitions reference.
     *  
     *      @param [in] newTypeDefinitionsRef     The type definitions reference to set.
     */
    void setTypeDefinitionsRef(QString const& newTypeDefinitionsRef);

    /*!
     *	Get the access policies of the address block.
     *	    
     * 	    @return Pointer to a list of access policies contained within the address block.
     */
    QSharedPointer<QList<QSharedPointer<AccessPolicy> > > getAccessPolicies() const;

    /*!
     *	Set the access policies of the address block.
     *  
     *      @param [in] newAccessPolicies     The access policies to set.
     */
    void setAccessPolicies(QSharedPointer<QList<QSharedPointer<AccessPolicy> > > newAccessPolicies);

private:

    /*!
     *  Copy the register elements.
     *
     *      @param [in] other   Address block being copied.
     */
    void copyRegisterData(const AddressBlock& other);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The attribute allowing or disallowing register address offset misalignment in the address block.
    BooleanValue misalignmentAllowed_;

    //! Contains the memory array of the address block.
    QSharedPointer<MemoryArray> memoryArray_;

    //! The address block definition reference.
    QString addressBlockDefinitionRef_;

    //! The type definitions reference.
    QString typeDefinitionsRef_;

    //! Indicates the type of the address block.
    QString typeIdentifier_;

	//! Range of an address block.
	QString range_;

	//! The attributes linked to the range element in IP-Xact document.
	QMap<QString, QString> rangeAttributes_;

	//! The bit width of a row in the address block.
    QString width_;

	//! The attributes linked to the range element in IP-Xact document.
	QMap<QString, QString> widthAttributes_;

    //! Specified the type of usage.
    General::Usage usage_;

    //! Indicates the volatility of the address block.
    BooleanValue volatile_;

    //! Specifies the accessibility of the address block data.
    AccessTypes::Access access_;

    //! The access poclicies of the address block.
    QSharedPointer<QList<QSharedPointer<AccessPolicy> > > accessPolicies_ = 
        QSharedPointer<QList<QSharedPointer<AccessPolicy> > >(new QList<QSharedPointer<AccessPolicy> >());

	//! Contains information about the grouping of bits into registers and fields.
    QSharedPointer<QList<QSharedPointer<RegisterBase> > > registerData_ = 
        QSharedPointer<QList<QSharedPointer<RegisterBase> > >(new QList<QSharedPointer<RegisterBase> > ());
};

Q_DECLARE_METATYPE(QSharedPointer<AddressBlock>);

#endif // ADDRESSBLOCK_H
