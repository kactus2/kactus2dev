//-----------------------------------------------------------------------------
// File: SystemVerilogValidator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 10.12.2014
//
// Description:
// <Short description of the class/file contents>
//-----------------------------------------------------------------------------

#ifndef SYSTEMVERILOGVALIDATOR_H
#define SYSTEMVERILOGVALIDATOR_H

#include <IPXACTmodels/ipxactmodels_global.h>
#include <IPXACTmodels/validators/ParameterValidator.h>

#include <QString>

class IPXACTMODELS_EXPORT ParameterValidator2014 : public ParameterValidator 
{
public:

	//! The constructor.
	ParameterValidator2014(QSharedPointer<ExpressionParser> expressionParser);

	//! The destructor.
	~ParameterValidator2014();
    
    /*!
     *   Checks if the given value is valid for the given type.
     *
     *      @param [in] value   The value to validate.
     *      @param [in] type    The type to which resolve the value.
     *
     *      @return True, if the value is valid for the given type, otherwise false.
     */
    virtual bool hasValidValueForType(QString const& value, QString const& type) const;

    virtual bool hasValidValueForType(Parameter const* parameter) const;

    
    virtual bool hasValidValue(Parameter const* parameter, QSharedPointer<QList<QSharedPointer<Choice> > > availableChoices) const;

    virtual bool hasValidMinimumValue(Parameter const* parameter) const;
    virtual bool hasValidMaximumValue(Parameter const* parameter) const;
    virtual bool valueIsLessThanMinimum(Parameter const* parameter) const;
    virtual bool valueIsGreaterThanMaximum(Parameter const* parameter) const;

protected:

    virtual bool shouldCompareValueAndBoundary(QString const& boundaryValue, QString const& type) const;
         
    virtual qreal valueOf(QString const& value, QString const& type) const;

private:

	// Disable copying.
	ParameterValidator2014(ParameterValidator2014 const& rhs);
	ParameterValidator2014& operator=(ParameterValidator2014 const& rhs);


    QSharedPointer<ExpressionParser> expressionParser_;
};

#endif // SYSTEMVERILOGVALIDATOR_H
