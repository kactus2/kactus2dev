//-----------------------------------------------------------------------------
// File: RegisterBase.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 28.09.2015
//
// Description:
// Describes the base class for ipxact:register, ipxact:registerFile and ipxact:alternateRegister.
//-----------------------------------------------------------------------------

#ifndef REGISTERBASE_H
#define REGISTERBASE_H

#include <IPXACTmodels/ipxactmodels_global.h>
#include <IPXACTmodels/common/NameGroup.h>
#include <IPXACTmodels/common/Extendable.h>

#include <QList>
#include <QString>
#include <QSharedPointer>

class Parameter;
class AccessPolicy;
class MemoryArray;

//-----------------------------------------------------------------------------
//! Describes the base class for ipxact:register, ipxact:registerFile and ipxact:alternateRegister.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT RegisterBase : public NameGroup, public Extendable
{

public:

    /*!
     *  The constructor.
     *
     *    @param [in] name    The register name.
     */
    RegisterBase(QString const& name = QString());
    RegisterBase(QString const& name, QString const& addressOffset);

    //! The copy constructor.
    RegisterBase(const RegisterBase& other);

	//! The assignment operator.
    RegisterBase& operator=(const RegisterBase& other);

	//! The destructor.
    virtual ~RegisterBase();

	/*!
     *  Clone the register model and return pointer to the copy.
	 * 
	 *    @return Pointer to the cloned register model.
	 */
    virtual QSharedPointer<RegisterBase> clone() = 0;

    /*!
     *  Get the presence value.
     *
     *    @return The isPresent value.
     */
    QString getIsPresent() const;

    /*!
     *  Set the is present value.
     *
     *    @param [in] newIsPresent    The new value for is present.
     */
    void setIsPresent(QString const& newIsPresent);
    /*!
     *  Get the dimension of the register.
     *
     *    @return The first dimension defined for the register.
     */
    QString getDimension() const;

    /*!
     *  Set the first dimension of the register base.
     *
     *    @param [in] newDimension    The new value for the first dim-element.
     */
    void setDimension(QString const& newDimension);

    /*!
     *	Get the register's memory array.
     *
     * 	    @return Pointer to the memory array of the register.
     */
    QSharedPointer<MemoryArray> getMemoryArray() const;

    /*!
     *	Set the memory array of the register.
     *
     *    @param [in] newMemArray     Pointer to the memory array to set.
     */
    void setMemoryArray(QSharedPointer<MemoryArray> newMemArray);

    /*!
     *  Get the address offset of the registerFile.
     *
     *    @return The offset.
     */
    QString getAddressOffset() const;

    /*!
     *  Set the address offset.
     *
     *    @param [in] newAddressOffset    The new offset.
     */
    void setAddressOffset(QString const& newAddressOffset);

    /*!
     *  Get the register type identifier.
     *
     *    @return The type identifier of the register.
     */
    QString getTypeIdentifier() const;

    /*!
     *  Set the register type identifier.
     *
     *    @param [in] newTypeIdentifier   The new register type identifier.
     */
    void setTypeIdentifier(QString const& newTypeIdentifier);

    /*!
     *  Get the parameters of the register.
     *
     *    @return Pointer to a list containing the parameters.
     */
    QSharedPointer<QList<QSharedPointer<Parameter> > > getParameters() const;

    /*!
     *  Set the parameters for the register.
     *
     *    @param [in] newParameters   Pointer to a list containing the new parameters
     */
    void setParameters(QSharedPointer<QList<QSharedPointer<Parameter> > > newParameters);

    /*!
     *	Get the access policies of the register.
     *  
     * 	    @return Pointer to a list containing the access policies.
     */
    QSharedPointer<QList<QSharedPointer<AccessPolicy> > > getAccessPolicies() const;

    /*!
     *	Set the access policies of the register.
     *  
     *    @param [in] newAccessPolicies     The new access policies to set.
     */
    void setAccessPolicies(QSharedPointer<QList<QSharedPointer<AccessPolicy> > > newAccessPolicies);

protected:

    //! Offset from the base address.
    QString addressOffset_;

private:

    /*!
     *  Copy the parameters.
     *
     *    @param [in] other   The register base class being copied.
     */
    void copyParameters(const RegisterBase& other);

    /*!
     *  Copy the access policies.
     *
     *    @param [in] other   The register base class being copied.
     */
    void copyAccessPolicies(const RegisterBase& other);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The presence value.
    QString isPresent_;

    //! The type identifier.
    QString typeIdentifier_;

    //! The memory array.
    QSharedPointer<MemoryArray> memoryArray_;

	//! Contains the parameters.
    QSharedPointer<QList<QSharedPointer<Parameter> > > parameters_ = 
        QSharedPointer<QList<QSharedPointer<Parameter> > >(new QList<QSharedPointer<Parameter> > ());

    //! The access policies.
    QSharedPointer<QList<QSharedPointer<AccessPolicy> > > accessPolicies_ =
        QSharedPointer<QList<QSharedPointer<AccessPolicy> > >(new QList<QSharedPointer<AccessPolicy> >());;
};

Q_DECLARE_METATYPE(QSharedPointer<RegisterBase>);

#endif // REGISTERBASE_H
