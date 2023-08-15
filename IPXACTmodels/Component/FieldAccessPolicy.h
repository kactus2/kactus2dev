//-----------------------------------------------------------------------------
// File: FieldAccessPolicy.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 28.7.2023
//
// Description:
// Describes the ipxact:fieldAccessPolicy element.
//-----------------------------------------------------------------------------

#ifndef FIELDACCESSPOLICY_H
#define FIELDACCESSPOLICY_H

#include "AccessPolicy.h"
#include "FieldReference.h"
#include "ModeReference.h"
#include "WriteValueConstraint.h"

#include <IPXACTmodels/ipxactmodels_global.h>
#include <IPXACTmodels/generaldeclarations.h>

#include <IPXACTmodels/common/BooleanValue.h>

class IPXACTMODELS_EXPORT FieldAccessPolicy : public AccessPolicy
{
public:

    //! Describes ipxact:accessRestriction
    struct IPXACTMODELS_EXPORT AccessRestriction
    {
        //! The operating modes for which the access restriction is active.
        QSharedPointer<QList<QSharedPointer<ModeReference> > > modeRefs_ = 
            QSharedPointer<QList<QSharedPointer<ModeReference> > >(new QList<QSharedPointer<ModeReference> >());

        //! Which bits of the field are readable.
        QString readAccessMask_;

        //! Which bits of the field are writable.
        QString writeAccessMask_;

        AccessRestriction() = default;

        AccessRestriction(AccessRestriction const& other);
    };

    FieldAccessPolicy();

    FieldAccessPolicy(FieldAccessPolicy const& other);
    
    virtual ~FieldAccessPolicy() = default;

    /*!
     *  Get the modified write value setting of the field.
     *
     *      @return The modified write value setting.
     */
    General::ModifiedWrite getModifiedWrite() const;

    /*!
     *  Set the modified write value setting for the field.
     *
     *      @param [in] modifiedWrite    The value to set.
     *
     */
    void setModifiedWrite(General::ModifiedWrite modifiedWrite);

    /*!
     *  Get the modify attribute of the modified write value.
     *
     *      @return The modify attribute of the modified write value.
     */
    QString getModifiedWriteModify() const;

    /*!
     *  Set the modify attribute of the modified write value.
     *
     *      @param [in] modify   The new modified attribute.
     */
    void setModifiedWriteModify(QString const& modify);

    /*!
     *  Get the write constraint of the field.
     *
     *      @return Pointer to the write constraint.
     */
    QSharedPointer<WriteValueConstraint> getWriteValueConstraint() const;

    /*!
     *  Set the write value constraint.
     *
     *      @param [in] constraint     Pointer to the new write value constraint.
     */
    void setWriteValueConstraint(QSharedPointer<WriteValueConstraint> constraint);

    /*!
     *  Get the read action setting of the field.
     *
     *      @return The read action setting.
     */
    General::ReadAction getReadAction() const;

    /*!
     *  Set the read action setting for the field.
     *
     *      @param [in] readAction  The new read action -value.
     */
    void setReadAction(General::ReadAction readAction);

    /*!
     *  Get the read action modify attribute.
     *
     *      @return The read action modify attribute.
     */
    QString getReadActionModify() const;

    /*!
     *  Set the read action modify attribute.
     *
     *      @param [in] readActionModify   The new modify attribute.
     */
    void setReadActionModify(QString const& readActionModify);

    /*!
     *  Get the testable setting of the field.
     *
     *      @return The testable setting.
     */
    BooleanValue getTestable() const;

    /*!
     *  Set the testable setting for the field.
     *
     *      @param [in] newTestable     The new testable value.
     */
    void setTestable(bool newTestable);

    /*!
     *  Clear the testable value of the field.
     */
    void clearTestable();

    /*!
     *	Get the read response of the field.
     *	    
     * 	    @return  The read response of the field.
     */
    QString getReadResponse() const;

    /*!
     *	Set the read response of the field.
     *  
     *      @param [in] response     Description
     */
    void setReadResponse(QString const& response);

    /*!
     *	Get the broadcasts of the field.
     *  
     * 	    @return The broadcasts of the field.
     */
    QSharedPointer<QList<QSharedPointer<FieldReference> > > getBroadcasts() const;

    /*!
     *	Set the broadcasts of the field.
     *  
     *      @param [in] broadcasts     Description
     */
    void setBroadcasts(QSharedPointer<QList<QSharedPointer<FieldReference> > > broadcasts);

    /*!
     *  Get the test constraint setting of the field.
     *
     *      @return The test constraint of the field.
     */
    General::TestConstraint getTestConstraint() const;

    /*!
     *  Set the test constraint for the field.
     *
     *      @param [in] testContraint   The new test constraint.
     */
    void setTestConstraint(General::TestConstraint constraint);

    /*!
     *  Get the reserved value.
     *
     *      @return The reserved value.
     */
    QString getReserved() const;

    /*!
     *  Set the reserved value.
     *
     *      @param [in] newReserved     The new reserved value.
     */
    void setReserved(QString const& reserved);

    /*!
     *	Get the field access policy definition reference.
     *  
     * 	    @return The defintion reference.
     */
    QString getFieldAccessPolicyDefinitionRef() const;

    /*!
     *	Set the field access policy definition reference.
     *  
     *      @param [in] definitionRef     The definition reference to set.
     */
    void setFieldAccessPolicyDefinitionRef(QString const& definitionRef);

    /*!
     *	Get the field access policy type definitions reference.
     *	    
     * 	    @return The type definitions reference.
     */
    QString getFieldAccessPolicyTypeDefinitionRef() const;

    /*!
     *	Set the field access policy type definitions reference.
     *  
     *      @param [in] typeDefRef     The type definitions reference to set.
     */
    void setFieldAccessPolicyTypeDefinitionRef(QString const& typeDefRef);

    /*!
     *	Get the access restrictions of the field access policy.
     *  
     * 	    @return The list of access restrictions.
     */
    QSharedPointer<QList<QSharedPointer<AccessRestriction> > > getAccessRestrictions() const;

    /*!
     *	Set the access restrictions of the field access policy.
     *  
     *      @param [in] newAccessRestrictions     The access restrictions to set.
     */
    void setAccessRestrictions(QSharedPointer<QList<QSharedPointer<AccessRestriction> > > newAccessRestrictions);

private:

    //! Reference to a field access policy definition.
    QString fieldAccessPolicyDefinitionRef_;

    //! Reference to a typeDefinitions element through an external type definition.
    QString fieldAccessPolicyTypeDefinitionRef_;

    //! Contains the modified write setting for the field.
    General::ModifiedWrite modifiedWrite_ = General::MODIFIED_WRITE_COUNT;

    //! User defined value for a modify modified write.
    QString modifiedWriteModify_;

    //! Pointer to the write value constraint.
    QSharedPointer<WriteValueConstraint> writeValueConstraint_;

    //! Contains the read action setting for the field.
    General::ReadAction readAction_ = General::READ_ACTION_COUNT;

    //! User defined additional information for a modify read action.
    QString readActionModify_;

    //! The read response value.
    QString readResponse_;

    //! The fields that write operations to this field are broadcast to.
    QSharedPointer<QList<QSharedPointer<FieldReference> > > broadcasts_ = 
        QSharedPointer<QList<QSharedPointer<FieldReference> > >(new QList<QSharedPointer<FieldReference> >());

    //! Describes which bits can be read from and written to for different modes.
    QSharedPointer<QList<QSharedPointer<AccessRestriction> > > accessRestrictions_ = 
        QSharedPointer<QList<QSharedPointer<AccessRestriction> > >(new QList<QSharedPointer<AccessRestriction> >());

    //! Contains the testable setting for the field.
    BooleanValue testable_;

    //! Contains the test constraint setting for the field.
    General::TestConstraint testConstraint_ = General::TESTCONSTRAINT_COUNT; 

    //! Indicates whether this field is reserved or not.
    QString reserved_;

};

#endif // FIELDACCESSPOLICY_H
