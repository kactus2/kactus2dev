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

#include <IPXACTmodels/Component/Model.h>

#include <KactusAPI/include/ExpressionParser.h>

#include <QRegularExpression>
#include <QStringList>

//-----------------------------------------------------------------------------
// Function: ViewValidator::ViewValidator()
//-----------------------------------------------------------------------------
ViewValidator::ViewValidator(QSharedPointer<ExpressionParser> expressionParser, QSharedPointer<Model> model):
expressionParser_(expressionParser),
model_(model)
{

}

//-----------------------------------------------------------------------------
// Function: ViewValidator::componentChange()
//-----------------------------------------------------------------------------
void ViewValidator::componentChange(QSharedPointer<Model> newModel)
{
    model_ = newModel;
}

//-----------------------------------------------------------------------------
// Function: ViewValidator::validateInstantiation()
//-----------------------------------------------------------------------------
bool ViewValidator::validate(QSharedPointer<View> view) const
{
    return hasValidName(view->name()) && hasValidIsPresent(view) && hasValidEnvironmentIdentifiers(view) &&
        hasValidComponentInstantiationReference(view) && hasValidDesignInstantiationReference(view) &&
        hasValidDesignConfigurationInstantiationReference(view) && validComparedToSiblings(view);
}

//-----------------------------------------------------------------------------
// Function: ViewValidator::hasValidName()
//-----------------------------------------------------------------------------
bool ViewValidator::hasValidName(QString const& name) const
{
    QRegularExpression whiteSpaceExpression;
    whiteSpaceExpression.setPattern(QStringLiteral("^\\s*$"));
    QRegularExpressionMatch whiteSpaceMatch = whiteSpaceExpression.match(name);

    if (name.isEmpty() || whiteSpaceMatch.hasMatch())
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: ViewValidator::hasValidIsPresent()
//-----------------------------------------------------------------------------
bool ViewValidator::hasValidIsPresent(QSharedPointer<View> view) const
{
    if (!view->getIsPresent().isEmpty())
    {
        bool isPresentOk = false;
        QString solvedValue = expressionParser_->parseExpression(view->getIsPresent(), &isPresentOk);

        int intValue = solvedValue.toInt();

        return isPresentOk && (intValue == 0 || intValue == 1);
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: ViewValidator::hasValidEnvironmentIdentifiers()
//-----------------------------------------------------------------------------
bool ViewValidator::hasValidEnvironmentIdentifiers(QSharedPointer<View> view) const
{
    for ( QSharedPointer<View::EnvironmentIdentifier> const& envId : *view->getEnvIdentifiers() )
    {
        if ( !isValidEnvId( envId ) )
        {
            return false;
        }
    }
    return true;
}

//-----------------------------------------------------------------------------
// Function: ViewValidator::isValidEnvId()
//-----------------------------------------------------------------------------
bool ViewValidator::isValidEnvId(QSharedPointer<View::EnvironmentIdentifier> const& envId) const
{
    QRegularExpression envIdExpression;
	envIdExpression.setPattern(QStringLiteral("[A-Za-z0-9_+\\*\\.]*"));

	QRegularExpressionMatch envIdMatchLanguage = envIdExpression.match(envId->language);
    if (!envId->language.isEmpty() && !envIdMatchLanguage.hasMatch())
    {
        return false;
    }
    
    QRegularExpressionMatch envIdMatchTool = envIdExpression.match(envId->tool);
    if (!envId->tool.isEmpty() && !envIdMatchTool.hasMatch())
    {
        return false;
    }

    QRegularExpressionMatch envIdMatchVendor = envIdExpression.match(envId->vendorSpecific);
	if ( !envId->vendorSpecific.isEmpty() && !envIdMatchVendor.hasMatch() )
	{
		return false;
	}

    return true;
}

//-----------------------------------------------------------------------------
// Function: ViewValidator::hasValidComponentInstantiationReference()
//-----------------------------------------------------------------------------
bool ViewValidator::hasValidComponentInstantiationReference(QSharedPointer<View> view) const
{
    if ( !view->getComponentInstantiationRef().isEmpty() &&
        !model_->findComponentInstantiation( view->getComponentInstantiationRef() ) )
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: ViewValidator::hasValidDesignInstantiationReference()
//-----------------------------------------------------------------------------
bool ViewValidator::hasValidDesignInstantiationReference(QSharedPointer<View> view) const
{
    if ( !view->getDesignInstantiationRef().isEmpty() &&
        !model_->findDesignInstantiation( view->getDesignInstantiationRef() ) )
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: ViewValidator::hasValidDesignConfigurationInstantiationReference()
//-----------------------------------------------------------------------------
bool ViewValidator::hasValidDesignConfigurationInstantiationReference(QSharedPointer<View> view) const
{
    if ( !view->getDesignConfigurationInstantiationRef().isEmpty() &&
        !model_->findDesignConfigurationInstantiation( view->getDesignConfigurationInstantiationRef() ) )
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: ViewValidator::findErrorsIn()
//-----------------------------------------------------------------------------
void ViewValidator::findErrorsIn(QVector<QString>& errors, QSharedPointer<View> view, QString const& contex) const
{
	if ( !hasValidName( view->name() ) )
	{
		errors.append(QObject::tr("Invalid name specified for view %1 within %2").arg(view->name()).arg(contex));
	}

    if (!hasValidIsPresent(view))
	{
		errors.append(QObject::tr("The presence %1 is invalid in view %2")
            .arg(view->getIsPresent()).arg(view->name()));
	}

	for ( QSharedPointer<View::EnvironmentIdentifier> const& envId : *view->getEnvIdentifiers() )
	{
		if ( !isValidEnvId( envId ) )
		{
			errors.append(QObject::tr("The env identifier %1 is invalid in view %2").arg(envId->toString()).arg(view->name()));
		}
	}

    if (!hasValidComponentInstantiationReference(view))
	{
		errors.append(QObject::tr("Referring non-existing component instantiation %1 in view %2")
			.arg(view->getComponentInstantiationRef()).arg(view->name()));
	}

    if (!hasValidDesignInstantiationReference(view))
	{
		errors.append(QObject::tr("Referring non-existing design instantiation %1 in view %2")
			.arg(view->getDesignInstantiationRef()).arg(view->name()));
	}

    if (!hasValidDesignConfigurationInstantiationReference(view))
	{
		errors.append(QObject::tr("Referring non-existing design configuration instantiation %1 in view %2")
			.arg(view->getDesignConfigurationInstantiationRef()).arg(view->name()));
	}
}
