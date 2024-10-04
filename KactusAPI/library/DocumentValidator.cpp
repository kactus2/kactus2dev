//-----------------------------------------------------------------------------
// File: DocumentValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 09.02.2018
//
// Description:
// <Short description of the class/file contents>
//-----------------------------------------------------------------------------

#include "DocumentValidator.h"

#include <KactusAPI/include/ComponentParameterFinder.h>
#include <KactusAPI/include/IPXactSystemVerilogParser.h>
#include <KactusAPI/include/ListParameterFinder.h>
#include <KactusAPI/include/ParameterCache.h>
#include <KactusAPI/include/SystemVerilogExpressionParser.h>
#include <KactusAPI/include/ModeConditionParserInterface.h>

#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>
#include <IPXACTmodels/BusDefinition/BusDefinition.h>
#include <IPXACTmodels/Catalog/Catalog.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Design/Design.h>
#include <IPXACTmodels/DesignConfiguration/DesignConfiguration.h>

//-----------------------------------------------------------------------------
// Function: DocumentValidator::DocumentValidator()
//-----------------------------------------------------------------------------
DocumentValidator::DocumentValidator(LibraryInterface* library) :
    library_(library),
    abstractionValidator_(library_, QSharedPointer<ExpressionParser>(new IPXactSystemVerilogParser(absDefParameterFinder_))),
    busValidator_(library_, QSharedPointer<ExpressionParser>(new SystemVerilogExpressionParser())),
    designValidator_(QSharedPointer<ExpressionParser>(new IPXactSystemVerilogParser(designValidatorFinder_)), library_),
    designConfigurationValidator_(QSharedPointer<ExpressionParser>(new SystemVerilogExpressionParser()), library_),
    systemDesignConfigurationValidator_(QSharedPointer<ExpressionParser>(new SystemVerilogExpressionParser()), library_)
{

}

//-----------------------------------------------------------------------------
// Function: DocumentValidator::validate()
//-----------------------------------------------------------------------------
bool DocumentValidator::validate(QSharedPointer<Document> document)
{
    VLNV::IPXactType documentType = document->getVlnv().getType();
    if (documentType == VLNV::ABSTRACTIONDEFINITION)
    {
        absDefParameterFinder_->setParameterList(document->getParameters());
        return abstractionValidator_.validate(document.dynamicCast<AbstractionDefinition>());
    }
    else if (documentType == VLNV::BUSDEFINITION)
    {
        return busValidator_.validate(document.dynamicCast<BusDefinition>());
    } 
    else if (documentType == VLNV::CATALOG)
    {
        return catalogValidator_.validate(document.dynamicCast<Catalog>());
    }
    else if (documentType == VLNV::COMPONENT)
    {
        QSharedPointer<Component> currentComponent = document.dynamicCast<Component>();
        changeComponentValidatorParameterFinder(currentComponent);

        QSharedPointer<ModeConditionParserInterface> modeConditionParserInterface(new ModeConditionParserInterface(componentValidatorFinder_));

        ComponentValidator componentValidator(QSharedPointer<ExpressionParser>(
            new IPXactSystemVerilogParser(componentValidatorFinder_)), modeConditionParserInterface, library_, currentComponent->getRevision());

        return componentValidator.validate(currentComponent);
    }
    else if (documentType == VLNV::DESIGN)
    {
        QSharedPointer<Design> currentDesign = document.dynamicCast<Design>();
        designValidatorFinder_->setParameterList(currentDesign->getParameters());

        return designValidator_.validate(currentDesign);
    }
    else if (documentType == VLNV::DESIGNCONFIGURATION)
    {
        QSharedPointer<DesignConfiguration> configuration = document.dynamicCast<DesignConfiguration>();

        if (configuration->getImplementation() == KactusAttribute::SYSTEM)
        {
            return systemDesignConfigurationValidator_.validate(configuration);
        }
        else
        {
            return designConfigurationValidator_.validate(configuration);
        }
    }
    else if (documentType == VLNV::APIDEFINITION || documentType == VLNV::COMDEFINITION)
    {
        return true;
    }
    
    return false;
}

//-----------------------------------------------------------------------------
// Function: DocumentValidator::findErrorsInDocument()
//-----------------------------------------------------------------------------
void DocumentValidator::findErrorsIn(QSharedPointer<Document> document, QVector<QString>& errorList)
{
    // Check if the document XML is valid and if not then print errors of the document..
    VLNV::IPXactType documentType = document->getVlnv().getType();
    if (documentType == VLNV::BUSDEFINITION)
    {
        findErrorsInBusDefinition(document.dynamicCast<BusDefinition>(), errorList);
    }
    else if (documentType == VLNV::ABSTRACTIONDEFINITION)
    {
        findErrorsInAbstractionDefinition(document.dynamicCast<AbstractionDefinition>(), errorList);
    }
    else if (documentType == VLNV::COMPONENT)
    {
        findErrorsInComponent(document.dynamicCast<Component>(), errorList);
    }
    else if (documentType == VLNV::DESIGN)
    {
        findErrorsInDesign(document.dynamicCast<Design>(), errorList);
    }
    else if (documentType == VLNV::DESIGNCONFIGURATION)
    {
        findErrorsInDesignConfiguration(document.dynamicCast<DesignConfiguration>(), errorList);
    }
}

//-----------------------------------------------------------------------------
// Function: DocumentValidator::changeComponentValidatorParameterFinder()
//-----------------------------------------------------------------------------
void DocumentValidator::changeComponentValidatorParameterFinder(QSharedPointer<Component> targetComponent)
{
    componentValidatorFinder_->setComponent(targetComponent);
}

//-----------------------------------------------------------------------------
// Function: DocumentValidator::findErrorsInBusDefinition()
//-----------------------------------------------------------------------------
void DocumentValidator::findErrorsInBusDefinition(QSharedPointer<BusDefinition> busDefinition,
    QVector<QString>& errorList)
{
    BusDefinitionValidator validator(
        library_, QSharedPointer<ExpressionParser>(new SystemVerilogExpressionParser()));
    validator.findErrorsIn(errorList, busDefinition);
}

//-----------------------------------------------------------------------------
// Function: DocumentValidator::findErrorsInAbstractionDefinition()
//-----------------------------------------------------------------------------
void DocumentValidator::findErrorsInAbstractionDefinition(QSharedPointer<AbstractionDefinition> abstraction,
    QVector<QString>& errorList)
{
    AbstractionDefinitionValidator validator(library_, 
        QSharedPointer<ExpressionParser>(new SystemVerilogExpressionParser()));
    validator.findErrorsIn(errorList, abstraction);
}

//-----------------------------------------------------------------------------
// Function: DocumentValidator::findErrorsInComponent()
//-----------------------------------------------------------------------------
void DocumentValidator::findErrorsInComponent(QSharedPointer<Component> component, QVector<QString>& errorList)
{
    changeComponentValidatorParameterFinder(component);
    QSharedPointer<ModeConditionParserInterface> parserFactory(new ModeConditionParserInterface(componentValidatorFinder_));
    ComponentValidator componentValidator(QSharedPointer<ExpressionParser>(
        new IPXactSystemVerilogParser(componentValidatorFinder_)), parserFactory, library_, component->getRevision());
    componentValidator.findErrorsIn(errorList, component);
}

//-----------------------------------------------------------------------------
// Function: DocumentValidator::findErrorsInDesign()
//-----------------------------------------------------------------------------
void DocumentValidator::findErrorsInDesign(QSharedPointer<Design> design, QVector<QString>& errorList)
{
    designValidatorFinder_->setParameterList(design->getParameters());
    designValidator_.findErrorsIn(errorList, design);
}

//-----------------------------------------------------------------------------
// Function: DocumentValidator::findErrorsInDesignConfiguration()
//-----------------------------------------------------------------------------
void DocumentValidator::findErrorsInDesignConfiguration(QSharedPointer<DesignConfiguration> configuration,
    QVector<QString>& errorList)
{
    if (configuration->getImplementation() == KactusAttribute::SYSTEM)
    {
        systemDesignConfigurationValidator_.findErrorsIn(errorList, configuration);
    }
    else
    {
        designConfigurationValidator_.findErrorsIn(errorList, configuration);
    }
}
