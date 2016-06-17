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

//-----------------------------------------------------------------------------
//! Describes the ipxact:register element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT Register: public RegisterDefinition
{

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] name            Name of the register.
	 *      @param [in] addressOffset   Offset of the register.
	 *      @param [in] size            Size of the register.
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
	 *      @return Pointer to the cloned register.
	 */
    virtual QSharedPointer<RegisterBase> clone();
    
    /*!
     *  Get the dimension of the register.
     *
     *      @return The value of the dim-element.
     */
    QString getDimension() const;

    /*!
     *  Set the dimension of the register.
     *
     *      @param [in] newDimension    The new value for the dim-element.
     */
    void setDimension(QString const& newDimension);

	/*!
     *  Get the addressOffset of the register.
	 *
	 *      @return The address offset.
	 */
    QString getAddressOffset() const;

    /*!
     *  Set the addressOffset for this register.
     *
     *      @param [in] addressOffset   The new addressOffset.
     */
    void setAddressOffset(QString const& addressOffset);

    /*!
     *  Get the alternate registers of this register.
     *
     *      @return Pointer to a list containing the alternate registers.
     */
    QSharedPointer<QList<QSharedPointer<AlternateRegister> > > getAlternateRegisters() const;

    /*!
     *  Set the alternate registers for this register.
     *
     *      @param [in] alternateRegisters  Pointer to a list containing the new alternateRegisters.
     */
    void setAlternateRegisters(QSharedPointer<QList<QSharedPointer<AlternateRegister> > > newAlternateRegisters);

	/*!
     *  Get the width of the register.
	 *
	 *      @return The width of the register in bits.
	 */
    QString getSize() const;

	/*!
     *  Set the width of the register.
	 *
	 *      @param [in] size    The width of the register in bits.
	 */
    void setSize(QString const& newSize);

private:

    /*!
     *  Copy the alternate registers.
     *
     *      @param [in] other   The register being copied.
     */
    void copyAlternateRegisters(const Register& other);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! Assigns an unbounded dimension to the register.
    QString dimension_;

	//! Offset from the base address.
	QString addressOffset_;

    //! The width of the registers, in bits.
    QString size_;

	//! Contains the alternateRegisters for this register.
    QSharedPointer<QList<QSharedPointer<AlternateRegister> > > alternateRegisters_;
};

Q_DECLARE_METATYPE(QSharedPointer<Register>);

#endif // REGISTER_H
