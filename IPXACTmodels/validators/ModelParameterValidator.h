//-----------------------------------------------------------------------------
// File: ModelParameterValidator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 20.11.2014
//
// Description:
// Validator for the IP-XACT Model Parameter element.
//-----------------------------------------------------------------------------

#ifndef MODELPARAMETERVALIDATOR_H
#define MODELPARAMETERVALIDATOR_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/validators/ParameterValidator.h>

#include <QString>

class ModelParameter;

//-----------------------------------------------------------------------------
//! Validator for the IP-XACT Model Parameter element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT ModelParameterValidator : public ParameterValidator
{
public:

	//! The constructor.
	ModelParameterValidator();

	//! The destructor.
	virtual ~ModelParameterValidator();

    /*!
     *  Validates the given model parameter.
     *
     *      @param [in] modelParameter   The model parameter to validate.
     *
     *      @return True, if the model parameter is valid IP-XACT, otherwise false.
     */
    virtual bool validate(ModelParameter const* modelParameter,
        QSharedPointer<QList<QSharedPointer<Choice> > > availableChoices) const;
    
    /*!
     *  Validates the usage type of the given model parameter.
     *
     *      @param [in] parameter   The model parameter whose usage type to validate.
     *
     *      @return True, if the model parameter usaget type is valid, otherwise false.
     */
    bool hasValidUsageType(ModelParameter const* modelParameter) const;

    /*!
     *  Finds possible errors in a model parameter and creates a list of the.
     *
     *      @param [in] modelParameter  The model parameter whose errors to find.
     *      @param [in] context         Context to help locate the errors.
     *
     *      @return List of the found errors in the model parameter.
     */
    virtual QStringList findErrorsIn(ModelParameter const* modelParameter, QString const& context, 
        QSharedPointer<QList<QSharedPointer<Choice> > > availableChoices) const;

private:

	// Disable copying.
	ModelParameterValidator(ModelParameterValidator const& rhs);
	ModelParameterValidator& operator=(ModelParameterValidator const& rhs);
    
    /*!
     *  Finds possible errors in a model parameter usage type.
     *
     *      @param [in] modelParameter  The model parameter whose errors to find.
     *      @param [in] context         Context to help locate the errors.
     *
     *      @return List of the found errors in the model parameter usage type.
     */
    QStringList findErrorsInUsageType(ModelParameter const* modelParameter, QString const& context) const;

};

#endif // MODELPARAMETERVALIDATOR_H
