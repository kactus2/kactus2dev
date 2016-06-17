//-----------------------------------------------------------------------------
// File: Field.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 24.09.2015
//
// Description:
// Describes the ipxact:field element.
//-----------------------------------------------------------------------------

#ifndef FIELD_H
#define FIELD_H

#include <IPXACTmodels/generaldeclarations.h>
#include <IPXACTmodels/common/AccessTypes.h>
#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/common/NameGroup.h>
#include <IPXACTmodels/common/Extendable.h>
#include <IPXACTmodels/common/BooleanValue.h>

#include <QString>
#include <QMap>
#include <QList>
#include <QSharedPointer>

class Parameter;
class EnumeratedValue;
class WriteValueConstraint;

//-----------------------------------------------------------------------------
//! Describes the ipxact:field element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT Field : public NameGroup, public Extendable
{

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] name    Name of the bit field.
	 */
	Field(QString const& name = QString());

	//! Copy constructor
	Field(const Field& other);

	//! Assignment operator
	Field& operator=(const Field& other);

	//! The destructor.
	virtual ~Field();
    
    /*!
     *  Get the id of the field.
     *
     *      @return The field's id.
     */
    QString getId() const;

    /*!
     *  Set the id for the field.
     *
     *      @param [in] newId   The id to set for the field.
     *
     */
    void setId(QString const& newId);

    /*!
     *  Get the isPresent value of the field.
     *
     *       @return The is present value.
     */
    QString getIsPresent() const;

    /*!
     *  Set the is present value for the field.
     *
     *      @param [in] newIsPresent    The new is present value.
     */
    void setIsPresent(QString const& newIsPresent);

	/*!
     *  Get the bit offset.
	 *
	 *      @return Bit offset.
	 */
    QString getBitOffset() const;

    /*!
     *  Set the bit offset.
     *
     *      @param [in] newBitOffset    The new bit offset.
     */
    void setBitOffset(QString const& newBitOffset);

    /*!
     *  Get the reset type reference.
     *
     *      @return The name of the referenced reset type.
     */
    QString getResetTypeReference() const;

    /*!
     *  Set the reset type reference.
     *
     *      @param [in] newResetTypeReference   The name of the referenced reset type.
     */
    void setResetTypeReference(QString const& newResetTypeReference);

    /*!
     *  Get the reset value of this field.
     *
     *      @return The reset value associated with this field.
     */
    QString getResetValue() const;
    
    /*!
     *  Set the reset value for this field.
     *
     *      @param [in] newResetValue   The new reset value.
     */
    void setResetValue(QString const& newResetValue);

    /*!
     *  Get the reset mask of this field.
     *
     *      @return The reset mask associated with this field.
     */
    QString getResetMask() const;

    /*!
     *  Set the reset mask for this field.
     *
     *      @param [in] newResetMask    The new reset mask.
     */
    void setResetMask(QString const& newResetMask);

    /*!
     *  Get the typeIdentifier.
     *
     *      @return The typeIdentifier.
     */
    QString getTypeIdentifier() const;

    /*!
     *  Set the typeIdentifier for the field.
     *
     *      @param [in] newTypeIdentifier   The new typeIdentifier.
     */
    void setTypeIdentifier(QString const& newTypeIdentifier);

    /*!
     *  Get the bit width.
     *
     *       @return Bit width.
     */
    QString getBitWidth() const;

    /*!
     *  Set the bit width for the field.
     *
     *      @param [in] newBitwidth     The new bit width.
     */
    void setBitWidth(QString const& newBitWidth);
    
	/*!
     *  Get the volatile state of the field.
	 *
	 *      @return The volatile state.
	 */
    BooleanValue getVolatile() const;

    /*!
     *  Set the volatile value to unspecified.
     */
    void clearVolatile();

	/*!
     *  Set the volatile state for the field.
	 *
	 *      @param [in] volatileValue   The new volatile state to set.
	 */
	void setVolatile(bool volatileValue);
    
	/*!
     *  Get the access type of the field.
	 *
	 *      @return The access type of the field.
	 */
	AccessTypes::Access getAccess() const;

	/*!
     *  Set the access type for the field.
	 *
	 *      @param [in] access  The access type to set.
	 */
	void setAccess(AccessTypes::Access access);
    
    /*!
     *  Get the list of the enumeratedValues.
     *
     *      @return Pointer to a list containing the enumerated values.
     */
    QSharedPointer<QList<QSharedPointer<EnumeratedValue> > > getEnumeratedValues() const;

    /*!
     *  Set the enumerated values for the field.
     *
     *      @param [in] newEnumeratedValues     Pointer to a list containing the enumerated values.
     */
    void setEnumeratedValues(QSharedPointer<QList<QSharedPointer<EnumeratedValue> > > newEnumeratedValues);

	/*!
     *  Get the modified write value setting of the field.
	 *
	 *      @return The modified write value setting.
	 */
	General::ModifiedWrite getModifiedWrite() const;

	/*!
     *  Set the modified write value setting for the field.
	 *
	 *      @param [in] newModWriteValue    The value to set.
	 *
	 */
	void setModifiedWrite(General::ModifiedWrite const& newModifiedWriteValue);
    
    /*!
     *  Get the modify attribute of the modified write value.
     *
     *      @return The modify attribute of the modified write value.
     */
    QString getModifiedWriteModify() const;

    /*!
     *  Set the modify attribute of the modified write value.
     *
     *      @param [in] newModify   The new modified attribute.
     */
    void setModifiedWriteModify(QString const& newModify);

	/*!
     *  Get the write constraint of the field.
	 *
	 *      @return Pointer to the write constraint.
	 */
	QSharedPointer<WriteValueConstraint> getWriteConstraint() const;

    /*!
     *  Set the write value constraint.
     *
     *      @param [in] newWriteValueConstraint     Pointer to the new write value constraint.
     */
    void setWriteConstraint(QSharedPointer<WriteValueConstraint> newWriteValueConstraint);
    
	/*!
     *  Get the read action setting of the field.
	 *
	 *      @return The read action setting.
	 */
	General::ReadAction getReadAction() const;

	/*!
     *  Set the read action setting for the field.
	 *
	 *      @param [in] readAction  The new read actionvalue.
	 */
	void setReadAction(General::ReadAction const& readAction);

    /*!
     *  Get the read action modify attribute.
     *
     *      @return The read action modify attribute.
     */
    QString getReadActionModify() const;

    /*!
     *  Set the read action modify attribute.
     *
     *      @param [in] newModify   The new modify attribute.
     */
    void setReadActionModify(QString const& newModify);
    
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
	void setTestConstraint(General::TestConstraint const& newTestContraint);

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
    void setReserved(QString const& newReserved);

    /*!
     *  Get a list of the parameters for the field.
     *
     *      @return Pointer to a list containing the parameters.
     */
    QSharedPointer<QList<QSharedPointer<Parameter> > > getParameters() const;

    /*!
     *  Set the parameters for the field.
     *
     *      @param [in] newParameters   Pointer to a list containing the new parameters.
     */
    void setParameters(QSharedPointer<QList<QSharedPointer<Parameter> > > newParameters);
    
private:

    /*!
     *  Copy the enumerated values.
     *
     *      @param [in] other   The copied field.
     */
    void copyEnumeratedValues(const Field& other);

    /*!
     *  Copy the parameters.
     *
     *      @param [in] other   The copied field.
     */
    void copyParameters(const Field& other);

    /*!
     *  Copy the write value constraint.
     *
     *      @param [in] other   The copied field.
     */
    void copyWriteValueConstraint(const Field& other);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! A unique id.
	QString id_;

    //! The presence of the field.
    QString isPresent_;

	//! Describes the offset where this bit field starts.
    QString bitOffset_;

    //! Identifies the type of the reset being defined.
    QString resetTypeReference_;

    //! Contains the actual reset value.
    QString resetValue_;

    //! Defines which bits of the register field have a known reset value.
    QString resetMask_;

	// Field elements with the same identifier contain the same information for the field definition group.
	QString typeIdentifier_;

	//! Width of the field.
    QString bitWidth_;

    //! Contains the volatile value for the field.
    BooleanValue volatile_;

    //! Contains the access type of the field.
    AccessTypes::Access access_;

	//! Pointer to a list containing the enumerated values.
	QSharedPointer<QList<QSharedPointer<EnumeratedValue> > > enumeratedValues_;

	//! Contains the modified write setting for the field.
	General::ModifiedWrite modifiedWrite_;

    //! User defined value for a modify modified write.
    QString modifiedWriteModify_;

    //! Pointer to the write value constraint.
    QSharedPointer<WriteValueConstraint> writeValueConstraint_;

	//! Contains the read action setting for the field.
	General::ReadAction readAction_;

    //! User defined additional information for a modify read action.
    QString readActionModify_;

	//! Contains the testable setting for the field.
    BooleanValue testable_;

	//! Contains the test constraint setting for the field.
	General::TestConstraint testConstraint_;

    //! Indicates whether this field is reserved or not.
    QString reserved_;

	//! Contains the parameters.
    QSharedPointer<QList<QSharedPointer<Parameter> > > parameters_;
};

Q_DECLARE_METATYPE(QSharedPointer<Field>);

#endif // FIELD_H
