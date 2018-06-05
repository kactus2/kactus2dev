//-----------------------------------------------------------------------------
// File: RegisterDefinition.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 28.09.2015
//
// Description:
// Describes the base definition for ipxact:register and ipxact:alternateRegister.
//-----------------------------------------------------------------------------

#ifndef REGISTERDEFINITION_H
#define REGISTERDEFINITION_H

#include "RegisterBase.h"

#include <IPXACTmodels/common/BooleanValue.h>

#include <IPXACTmodels/common/AccessTypes.h>
#include <IPXACTmodels/ipxactmodels_global.h>

#include <QString>
#include <QList>
#include <QSharedPointer>

class Field;

//-----------------------------------------------------------------------------
//! Describes the base definition for ipxact:register and ipxact:alternateRegister.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT RegisterDefinition : public RegisterBase
{

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] name    Name of the register definition.
	 */
	RegisterDefinition(QString const& name = QString());

	//! Copy constructor.
	RegisterDefinition(const RegisterDefinition& other);

	//! Assignment operator.
	RegisterDefinition& operator=(const RegisterDefinition& other);

	//! The destructor.
	virtual ~RegisterDefinition();

    /*!
     *  Get the volatile value.
     *
     *      @return The volatile value in string format. Empty string means an unspecified volatility.
     */
    QString getVolatile() const;

    /*!
     *  Set the volatileValue.
     *
     *      @param [in] newVolatile     The new volatile value.
     */
    void setVolatile(bool newVolatile);

    /*!
     *  Sets the volatile value to unspecified.
     */
    void clearVolatile();

	/*!
     *  Get the access.
	 *
	 *      @return Access value.
	 */
    AccessTypes::Access getAccess() const;

    /*!
     *  Set the access.
     *
     *      @param [in] access  The new access value.
     */
    void setAccess(AccessTypes::Access newAccessValue);

	/*!
     *  Get the list of contained fields.
     *
     *      @return Pointer to a list containing the fields.
     */
    QSharedPointer<QList<QSharedPointer<Field> > > getFields() const;

    /*!
     *  Set the new fields for the register.
     *
     *      @param [in] fields  Pointer to a list containing the fields.
     */
    void setFields(QSharedPointer<QList<QSharedPointer<Field> > > newFields);

private:

    /*!
     *  Copy the fields.
     *
     *      @param [in] other   The register definition being copied.
     */
    void copyFields(const RegisterDefinition& other);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! Indicates whether the contained data is volatile.
    BooleanValue volatile_;

	//! Indicates the accessibility of the data.
    AccessTypes::Access access_;

	//! Contains the field-elements.
    QSharedPointer<QList<QSharedPointer<Field> > > fields_;
};

#endif // REGISTERDEFINITION_H
