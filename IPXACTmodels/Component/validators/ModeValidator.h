//-----------------------------------------------------------------------------
// File: ModeValidator.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 14.08.2023
//
// Description:
// Validator for ipxact:mode.
//-----------------------------------------------------------------------------

#ifndef MODEVALIDATOR_H
#define MODEVALIDATOR_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/Component/Mode.h>
#include <IPXACTmodels/Component/Component.h>

#include <IPXACTmodels/Component/validators/PortSliceValidator.h>
#include <IPXACTmodels/Component/validators/FieldSliceValidator.h>

#include <QSharedPointer>
#include <QString>
#include <QVector>

class ExpressionParser;
class BuildCommand;

//-----------------------------------------------------------------------------
//! Validator for ipxact:File.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT ModeValidator
{
public:

	/*!
     *  The constructor.
     * 
     *      @param [in] sliceValidator The validator for port slices.
     *      @param [in] component      The component containing the modes.
     *      @param [in] mode           The expression parser for solving expressions in modes.
	 *
	 */
    ModeValidator(
        QSharedPointer<Component> component,
        QSharedPointer<ExpressionParser> expressionParser);

	//! The destructor.
	virtual ~ModeValidator();
    
	// Disable copying.
	ModeValidator(ModeValidator const& rhs) = delete;
	ModeValidator& operator=(ModeValidator const& rhs) = delete;

    /*!
     *  Validates the given mode.
     *
     *      @param [in] mode           The mode to validate.
     *
     *      @return True, if the mode is valid IP-XACT, otherwise false.
     */
    bool validate(QSharedPointer<Mode> mode) const;

    /*!
     *  Check if the mode name is valid.
     *
     *      @param [in] name    The name to be evaluated.
     *
     *      @return True, if the name is valid, otherwise false.
     */
    bool hasValidName(QString const& name) const;

    bool hasValidCondition(QSharedPointer<Mode> mode) const;

    /*!
     *  Check if the port slices are valid.
     *
     *      @param [in] mode    The mode whose port slices to check.
     *
     *      @return True, if the slices are valid, otherwise false.
     */
    bool hasValidPortSlices(QSharedPointer<Mode> mode) const;

    bool hasValidFieldSlices(QSharedPointer<Mode> mode) const;

    /*!
     *  Finds possible errors in a mode and creates a list of them.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] mode        The mode whose errors to find.
     *      @param [in] context     Context to help locate the errors.
     */
    void findErrorsIn(QVector<QString>& errors, QSharedPointer<Mode> mode,
		QString const& context) const;

    // Change the active component.
    void componentChange(QSharedPointer<Component> newComponent);

    //! Get the port slice validator.
    QSharedPointer<PortSliceValidator> getPortSliceValidator() const;

    //! Get the field slice validator.
    QSharedPointer<FieldSliceValidator> getFieldSliceValidator() const;

private:
    
    //! The port slice validator to use.
    QSharedPointer<PortSliceValidator> portValidator_;

    QSharedPointer<FieldSliceValidator> fieldValidator_;

    //! The component containing the modes.
    QSharedPointer<Component> component_ = nullptr;

    //! The expression parser to use.
    QSharedPointer<ExpressionParser> expressionParser_;
};

#endif // MODEVALIDATOR_H
