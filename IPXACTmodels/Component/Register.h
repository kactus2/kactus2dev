//-----------------------------------------------------------------------------
// File: Register.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 28.09.2015
//
// Description:
// Describes the ipxact:register element.
//-----------------------------------------------------------------------------

#ifndef REGISTER_H
#define REGISTER_H

#include "RegisterDefinition.h"

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QString>
#include <QList>
#include <QSharedPointer>

class AlternateRegister;
class MemoryArray;

//-----------------------------------------------------------------------------
//! Describes the ipxact:register element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT Register: public RegisterDefinition
{

public:

	/*!
	 *  The constructor.
	 *
	 *    @param [in] name            Name of the register.
	 *    @param [in] addressOffset   Offset of the register.
	 *    @param [in] size            Size of the register.
	 */
	Register(QString const& name = QString(), QString const& addressOffset = QString(),
        QString const& size = QString());

	//! Copy constructor.
	Register(const Register& other);

	//! Assignment operator.
	Register& operator=(const Register& other);

	//! The destructor.
	virtual ~Register();

	/*!
     *  Clone the register and return pointer to the copy.
	 * 
	 *    @return Pointer to the cloned register.
	 */
    virtual QSharedPointer<RegisterBase> clone();
    

    /*!
     *  Get the alternate registers of this register.
     *
     *    @return Pointer to a list containing the alternate registers.
     */
    QSharedPointer<QList<QSharedPointer<AlternateRegister> > > getAlternateRegisters() const;

    /*!
     *  Set the alternate registers for this register.
     *
     *    @param [in] alternateRegisters  Pointer to a list containing the new alternateRegisters.
     */
    void setAlternateRegisters(QSharedPointer<QList<QSharedPointer<AlternateRegister> > > newAlternateRegisters);

	/*!
     *  Get the width of the register.
	 *
	 *    @return The width of the register in bits.
	 */
    QString getSize() const;

	/*!
     *  Set the width of the register.
	 *
	 *    @param [in] size    The width of the register in bits.
	 */
    void setSize(QString const& newSize);

    /*!
     *	Get the register definition reference.
     *	    
     * 	    @return The reference to a register definition.
     */
    QString getRegisterDefinitionReference() const;

    /*!
     *	Set the register definition reference.
     *  
     *    @param [in] registerDefinitionRef     The new register definition reference.
     */
    void setRegisterDefinitionReference(QString const& registerDefinitionRef);

    /*!
     *	Get the reference to the type definitions containing the register definition reference.
     *	    
     * 	    @return The reference to the type definitions containing the register definition.
     */
    QString getTypeDefinitionsReference() const;

    /*!
     *	Set the type definitions reference.
     *  
     *    @param [in] typeDefinitionsRef     The new type definitions reference to set.
     */
    void setTypeDefinitionsReference(QString const& typeDefinitionsRef);

private:

    /*!
     *  Copy the alternate registers.
     *
     *    @param [in] other   The register being copied.
     */
    void copyAlternateRegisters(const Register& other);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The width of the registers, in bits.
    QString size_;

	//! Contains the alternateRegisters for this register.
    QSharedPointer<QList<QSharedPointer<AlternateRegister> > > alternateRegisters_;

    //! The name of the referenced register definition.
    QString registerDefinitionReference_;

    //! The type definitions containing the referenced register definition.
    QString typeDefinitionsReference_;
};

Q_DECLARE_METATYPE(QSharedPointer<Register>);

#endif // REGISTER_H
