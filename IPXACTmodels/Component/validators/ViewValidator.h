//-----------------------------------------------------------------------------
// View: ViewValidator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 24.11.2015
//
// Description:
// Validator for ipxact:View.
//-----------------------------------------------------------------------------

#ifndef ViewVALIDATOR_H
#define ViewVALIDATOR_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/Component/DesignConfigurationInstantiation.h>
#include <IPXACTmodels/Component/DesignInstantiation.h>
#include <IPXACTmodels/Component/ComponentInstantiation.h>
#include <IPXACTmodels/Component/View.h>
#include <editors/ComponentEditor/common/ExpressionParser.h>
#include <IPXACTmodels/Component/Model.h>

#include <QString>
#include <QVector>

//-----------------------------------------------------------------------------
//! Validator for ipxact:View.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT ViewValidator
{
public:

	/*!
	 *  The constructor.
	 *
	 */
    ViewValidator();

	//! The destructor.
	~ViewValidator();

    /*!
     *  Validates the given View.
     *
     *      @param [in] View           The View to validate.
     *
     *      @return True, if the View is valid IP-XACT, otherwise false.
     */
    virtual bool validate(QSharedPointer<View> view, QSharedPointer<Model> model) const;

    /*!
     *  Finds possible errors in a View and creates a list of them.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] View   The View whose errors to find.
     *      @param [in] context     Context to help locate the errors.
     */
    virtual void findErrorsIn(QVector<QString>& errors,
		QSharedPointer<View> view, QString const& contex, QSharedPointer<Model> model) const;

private:

    /*!
     *  Check if the name is valid.
     *
     *      @param [in] name    The name to be evaluated.
     *
     *      @return True, if the name is valid, otherwise false.
     */
	bool hasValidName(QString const& name) const;
	
    /*!
     *  Check if an env identifier is valid.
     *
     *      @param [in] name    The env identifier to be evaluated.
     *
     *      @return True, if the env identifier is valid, otherwise false.
     */
	bool isValidEnvId( QString const& envId ) const;

	// Disable copying.
	ViewValidator(ViewValidator const& rhs);
	ViewValidator& operator=(ViewValidator const& rhs);
};

#endif // SYSTEMVERILOGVALIDATOR_H
