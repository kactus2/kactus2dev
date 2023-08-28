//-----------------------------------------------------------------------------
// File: PortSliceValidator.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 17.08.2023
//
// Description:
// Validator for ipxact:portSlice.
//-----------------------------------------------------------------------------

#ifndef PORTSLICEVALIDATOR_H
#define PORTSLICEVALIDATOR_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/Component/PortSlice.h>
#include <IPXACTmodels/Component/Component.h>


#include <QSharedPointer>
#include <QString>
#include <QVector>

class ExpressionParser;

//-----------------------------------------------------------------------------
//! Validator for ipxact:portSlice.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT PortSliceValidator
{
public:

	/*!
     *  The constructor.
     * 
     *      @param [in] component      The component containing the PortSlices.
     *      @param [in] PortSlice           The expression parser for solving expressions in PortSlices.
	 *
	 */
    PortSliceValidator(QSharedPointer<Component> component, QSharedPointer<ExpressionParser> expressionParser);

	//! The destructor.
	virtual ~PortSliceValidator();
    
	// Disable copying.
	PortSliceValidator(PortSliceValidator const& rhs) = delete;
	PortSliceValidator& operator=(PortSliceValidator const& rhs) = delete;

    /*!
     *  Validates the given port slice.
     *
     *      @param [in] PortSlice           The port slice to validate.
     *
     *      @return True, if the PortSlice is valid IP-XACT, otherwise false.
     */
    bool validate(QSharedPointer<PortSlice> portSlice) const;

    /*!
     *  Validates the port reference in the given port slice.
     *
     *      @param [in] portSlice           The port slice to validate.
     *
     *      @return True, if the portSlice is has a valid port reference, otherwise false.
     */
    bool hasValidPortReference(QSharedPointer<PortSlice> portSlice) const;

    /*!
     *  Validates the left range in the given port slice.
     *
     *      @param [in] portSlice           The port slice to validate.
     *
     *      @return True, if the portSlice is has a valid left range, otherwise false.
     */
    bool hasValidLeftRange(QSharedPointer<PortSlice> portSlice) const;

    /*!
     *  Validates the right range in the given port slice.
     *
     *      @param [in] portSlice           The port slice to validate.
     *
     *      @return True, if the portSlice is has a valid right range, otherwise false.
     */
    bool hasValidRightRange(QSharedPointer<PortSlice> portSlice) const;

    /*!
     *  Check if the portSlice name is valid.
     *
     *      @param [in] name    The name to be evaluated.
     *
     *      @return True, if the name is valid, otherwise false.
     */
    bool hasValidName(QString const& name) const;

    /*!
     *  Finds possible errors in a PortSlice and creates a list of them.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] PortSlice        The PortSlice whose errors to find.
     *      @param [in] context     Context to help locate the errors.
     */
    void findErrorsIn(QVector<QString>& errors, QSharedPointer<PortSlice> portSlice, QString const& context) const;

    /*!
     *  Change the active component.
     *
     *      @param [in] newComponent    List of found errors.
     */
    void componentChange(QSharedPointer<Component> newComponent);

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

    //! The component containing the PortSlices.
    QSharedPointer<Component> component_ = nullptr;

    //! The expression parser to use.
    QSharedPointer<ExpressionParser> expressionParser_;
};

#endif // PORTSLICEVALIDATOR_H
