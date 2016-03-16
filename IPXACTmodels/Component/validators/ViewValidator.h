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

#include <QString>
#include <QVector>
#include <QSharedPointer>

class ExpressionParser;
class View;
class Model;
class ComponentInstantiation;
class DesignInstantiation;
class DesignConfigurationInstantiation;

//-----------------------------------------------------------------------------
//! Validator for ipxact:View.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT ViewValidator
{
public:

	/*!
	 *  The constructor.
     *
     *      @param [in] expressionParser    The used expression parser.
	 *      @param [in] model               The containing component model.
	 */
    ViewValidator(QSharedPointer<ExpressionParser> expressionParser, QSharedPointer<Model> model);

	//! The destructor.
	virtual ~ViewValidator();

    /*!
     *  Change the available model.
     *
     *      @param [in] newModel    The new model.
     */
    void componentChange(QSharedPointer<Model> newModel);

    /*!
     *  Validates the given View.
     *
     *      @param [in] View    The View to validate.
     *
     *      @return True, if the View is valid IP-XACT, otherwise false.
     */
    virtual bool validate(QSharedPointer<View> view) const;

    /*!
     *  Check if the name is valid.
     *
     *      @param [in] name    The name to be evaluated.
     *
     *      @return True, if the name is valid, otherwise false.
     */
	bool hasValidName(QString const& name) const;
	
    /*!
     *  Check if the is present value is valid.
     *
     *      @param [in] view    The selected view.
     *
     *      @return True, if the is present value is valid, otherwise false.
     */
    bool hasValidIsPresent(QSharedPointer<View> view) const;

    /*!
     *  Check if the environment identifiers are valid.
     *
     *      @param [in] view    The selected view.
     *
     *      @return True, if the environment identifiers are valid, otherwise false.
     */
    bool hasValidEnvironmentIdentifiers(QSharedPointer<View> view) const;

    /*!
     *  Check if the component instantiation reference is valid.
     *
     *      @param [in] view    The selected view.
     *
     *      @return True, if the component instantiation reference is valid, otherwise false.
     */
    bool hasValidComponentInstantiationReference(QSharedPointer<View> view) const;

    /*!
     *  Check if the is design instantiation reference is valid.
     *
     *      @param [in] view    The selected view.
     *
     *      @return True, if the design instantiation reference is valid, otherwise false.
     */
    bool hasValidDesignInstantiationReference(QSharedPointer<View> view) const;

    /*!
     *  Check if the design configuration reference is valid.
     *
     *      @param [in] view    The selected view.
     *
     *      @return True, if the is design configuration reference is valid, otherwise false.
     */
    bool hasValidDesignConfigurationInstantiationReference(QSharedPointer<View> view) const;

    /*!
     *  Check if an environment identifier is valid.
     *
     *      @param [in] name    The environment identifier to be evaluated.
     *
     *      @return True, if the environment identifier is valid, otherwise false.
     */
	bool isValidEnvId( QString const& envId ) const;

    /*!
     *  Finds possible errors in a View and creates a list of them.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] View        The View whose errors to find.
     *      @param [in] context     Context to help locate the errors.
     */
    virtual void findErrorsIn(QVector<QString>& errors, QSharedPointer<View> view, QString const& contex) const;

private:

	// Disable copying.
	ViewValidator(ViewValidator const& rhs);
	ViewValidator& operator=(ViewValidator const& rhs);

    //! The used expression parser.
    QSharedPointer<ExpressionParser> expressionParser_;

    //! The containing component model.
    QSharedPointer<Model> model_;
};

#endif // SYSTEMVERILOGVALIDATOR_H
