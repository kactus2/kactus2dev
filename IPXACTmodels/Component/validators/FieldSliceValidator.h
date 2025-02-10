//-----------------------------------------------------------------------------
// File: FieldSliceValidator.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 07.09.2023
//
// Description:
// Validator for ipxact:fieldSlice.
//-----------------------------------------------------------------------------

#ifndef FIELDSLICEVALIDATOR_H
#define FIELDSLICEVALIDATOR_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/Component/FieldSlice.h>
#include <IPXACTmodels/Component/Field.h>
#include <IPXACTmodels/Component/Component.h>

#include <IPXACTmodels/utilities/Search.h>

#include <QSharedPointer>
#include <QString>
#include <QVector>

class ExpressionParser;

//-----------------------------------------------------------------------------
//! Validator for ipxact:portSlice.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT FieldSliceValidator
{
public:

	/*!
     *  The constructor.
     * 
     *      @param [in] component      The component containing the FieldSlices.
     *      @param [in] FieldSlice           The expression parser for solving expressions in FieldSlices.
	 *
	 */
    FieldSliceValidator(QSharedPointer<Component> component, QSharedPointer<ExpressionParser> expressionParser);

	//! The destructor.
	virtual ~FieldSliceValidator();
    
	// Disable copying.
	FieldSliceValidator(FieldSliceValidator const& rhs) = delete;
	FieldSliceValidator& operator=(FieldSliceValidator const& rhs) = delete;

    /*!
     *  Validates the given port slice.
     *
     *      @param [in] FieldSlice           The port slice to validate.
     *
     *      @return True, if the FieldSlice is valid IP-XACT, otherwise false.
     */
    bool validate(QSharedPointer<FieldSlice> portSlice) const;

    /*!
     *  Validates the port reference in the given port slice.
     *
     *      @param [in] portSlice           The port slice to validate.
     *
     *      @return True, if the portSlice is has a valid port reference, otherwise false.
     */
    bool hasValidFieldReference(QSharedPointer<FieldSlice> portSlice) const;

    /*!
     *  Validates the left range in the given port slice.
     *
     *      @param [in] portSlice           The port slice to validate.
     *
     *      @return True, if the portSlice is has a valid left range, otherwise false.
     */
    bool hasValidLeftRange(QSharedPointer<FieldSlice> portSlice) const;

    /*!
     *  Validates the right range in the given port slice.
     *
     *      @param [in] portSlice           The port slice to validate.
     *
     *      @return True, if the portSlice is has a valid right range, otherwise false.
     */
    bool hasValidRightRange(QSharedPointer<FieldSlice> portSlice) const;

    /*!
     *  Check if the portSlice name is valid.
     *
     *      @param [in] name    The name to be evaluated.
     *
     *      @return True, if the name is valid, otherwise false.
     */
    bool hasValidName(QString const& name) const;

    /*!
     *  Finds possible errors in a FieldSlice and creates a list of them.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] FieldSlice        The FieldSlice whose errors to find.
     *      @param [in] context     Context to help locate the errors.
     */
    void findErrorsIn(QVector<QString>& errors, QSharedPointer<FieldSlice> portSlice, QString const& context) const;

    /*!
     *  Change the active component.
     *
     *      @param [in] newComponent    List of found errors.
     */
    void componentChange(QSharedPointer<Component> newComponent);

    /*!
     *	Set the expression parser to be used.
     *
     *      @param [in] expressionParser     The expression parser to set.
     */
    void setExpressionParser(QSharedPointer<ExpressionParser> expressionParser);

private:

    /* Check if the given value is in the given range.
     *
     *      @param [in] value        The value to be evaluated.
     *      @param [in] leftBound    The left bound of the range.
     *      @param [in] rightBound   The right bound of the range.
     *
     *      @return True, if the value is in the range, otherwise false.
     */
    bool isInValidRange(QString const& value, int leftBound, int rightBound) const;

    /*!
     *  Find the field referenced in the field slice.
     *
     *      @param [in] fieldSlice  The field slice whose referenced field to find.
     *
     *      @return The referenced field.
     */
    QSharedPointer<Field> findField(QSharedPointer<FieldSlice> fieldSlice) const;


    /*!
     *  Find a referenced item.
     *
     *      @param [in] reference   The reference to the item.
     *      @param [in] container   The container to search in.
     *
     *      @return The referenced item, if found.
     */
    template <typename T>
    QSharedPointer<T> findItem(QSharedPointer<FieldReference::IndexedReference> reference,
        QSharedPointer<QList<QSharedPointer<T> > > container) const
    {
        if (reference.isNull() || reference->reference_.isEmpty())
        {
            return nullptr;
        }

        return Search::findByName(reference->reference_, *container);
    }

    //! The component containing the FieldSlices.
    QSharedPointer<Component> component_ = nullptr;

    //! The expression parser to use.
    QSharedPointer<ExpressionParser> expressionParser_;

};

#endif // FIELDSLICEVALIDATOR_H
