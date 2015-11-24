//-----------------------------------------------------------------------------
// View: ViewValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 24.11.2015
//
// Description:
// Validator for ipxact:View.
//-----------------------------------------------------------------------------

#include "ViewValidator.h"

#include <IPXACTmodels/Component/choice.h>
#include <IPXACTmodels/common/Enumeration.h>

#include <editors/ComponentEditor/common/SystemVerilogExpressionParser.h>

#include <QRegularExpression>
#include <QStringList>

//-----------------------------------------------------------------------------
// Function: ViewValidator::ViewValidator()
//-----------------------------------------------------------------------------
ViewValidator::ViewValidator()
{

}

//-----------------------------------------------------------------------------
// Function: ViewValidator::~ViewValidator()
//-----------------------------------------------------------------------------
ViewValidator::~ViewValidator()
{

}

//-----------------------------------------------------------------------------
// Function: ViewValidator::validateInstantiation()
//-----------------------------------------------------------------------------
bool ViewValidator::validate(QSharedPointer<View> view, QSharedPointer<Model> model) const
{
	QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());

	if ( !hasValidName( view->name() ) )
	{
		return false;
	}

	if ( !view->getIsPresent().isEmpty() && !parser->isValidExpression( view->getIsPresent() ) )
	{
		return false;
	}

	foreach( QString envId, view->getEnvIdentifiers() )
	{
		if ( !isValidEnvId( envId ) )
		{
			return false;
		}
	}

	if ( !view->getComponentInstantiationRef().isEmpty() &&
		!model->findComponentInstantiation( view->getComponentInstantiationRef() ) )
	{
		return false;
	}

	if ( !view->getDesignInstantiationRef().isEmpty() &&
		!model->findDesignInstantiation( view->getDesignInstantiationRef() ) )
	{
		return false;
	}

	if ( !view->getDesignConfigurationInstantiationRef().isEmpty() &&
		!model->findDesignConfigurationInstantiation( view->getDesignConfigurationInstantiationRef() ) )
	{
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// Function: ViewValidator::findErrorsIn()
//-----------------------------------------------------------------------------
void ViewValidator::findErrorsIn(QVector<QString>& errors,
	QSharedPointer<View> view, QString const& contex, QSharedPointer<Model> model) const
{
	QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());

	if ( !hasValidName( view->name() ) )
	{
		errors.append(QObject::tr("The name is invalid or in-existing: %1").arg(view->name()));
	}

	if ( !view->getIsPresent().isEmpty() && !parser->isValidExpression( view->getIsPresent() ) )
	{
		errors.append(QObject::tr("The presence is invalid: %1").arg(view->getIsPresent()));
	}

	foreach( QString envId, view->getEnvIdentifiers() )
	{
		if ( !isValidEnvId( envId ) )
		{
			errors.append(QObject::tr("The env identifier is invalid: %1").arg(envId));
		}
	}

	if ( !view->getComponentInstantiationRef().isEmpty() &&
		!model->findComponentInstantiation( view->getComponentInstantiationRef() ) )
	{
		errors.append(QObject::tr("Referring inexisting component instantiation: %1")
			.arg(view->getComponentInstantiationRef()));
	}

	if ( !view->getDesignInstantiationRef().isEmpty() &&
		!model->findDesignInstantiation( view->getDesignInstantiationRef() ) )
	{
		errors.append(QObject::tr("Referring inexisting design instantiation: %1")
			.arg(view->getDesignInstantiationRef()));
	}

	if ( !view->getDesignConfigurationInstantiationRef().isEmpty() &&
		!model->findDesignConfigurationInstantiation( view->getDesignConfigurationInstantiationRef() ) )
	{
		errors.append(QObject::tr("Referring inexisting design configuration instantiation: %1")
			.arg(view->getDesignConfigurationInstantiationRef()));
	}
}

//-----------------------------------------------------------------------------
// Function: ViewValidator::hasValidName()
//-----------------------------------------------------------------------------
bool ViewValidator::hasValidName(QString const& name) const
{
	QRegularExpression whiteSpaceExpression;
	whiteSpaceExpression.setPattern("^\\s*$");
	QRegularExpressionMatch whiteSpaceMatch = whiteSpaceExpression.match(name);

	if (name.isEmpty() || whiteSpaceMatch.hasMatch())
	{
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// Function: ViewValidator::isValidEnvId()
//-----------------------------------------------------------------------------
bool ViewValidator::isValidEnvId(QString const& envId) const
{
	QRegularExpression envIdExpression;
	envIdExpression.setPattern("[A-Za-z0-9_+\\*\\.]*:[A-Za-z0-9_+\\*\\.]*:[A-Za-z0-9_+\\*\\.]*");
	QRegularExpressionMatch envIdMatch = envIdExpression.match(envId);

	if ( !envId.isEmpty() && !envIdMatch.hasMatch() )
	{
		return false;
	}

	return true;
}
