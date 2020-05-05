//-----------------------------------------------------------------------------
// File: PythonAPI.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 12.02.2020
//
// Description:
// Interface for accessing Kactus2 data through python.
//-----------------------------------------------------------------------------

#include "PythonAPI.h"

#include <library/LibraryHandler.h>

#include <editors/ComponentEditor/ports/PortsInterface.h>
#include <editors/ComponentEditor/parameters/ParametersInterface.h>
#include <editors/ComponentEditor/memoryMaps/interfaces/RegisterInterface.h>
#include <editors/ComponentEditor/memoryMaps/interfaces/FieldInterface.h>
#include <editors/ComponentEditor/memoryMaps/interfaces/ResetInterface.h>

#include <editors/ComponentEditor/common/ComponentAndInstantiationsParameterFinder.h>
#include <editors/ComponentEditor/common/IPXactSystemVerilogParser.h>
#include <editors/ComponentEditor/common/ExpressionFormatter.h>

#include <PythonAPI/messageMediator/PythonMessageMediator.h>

#include <VersionHelper.h>

#include <IPXACTmodels/common/validators/ParameterValidator.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Port.h>
#include <IPXACTmodels/Component/View.h>
#include <IPXACTmodels/Component/MemoryMap.h>
#include <IPXACTmodels/Component/AddressBlock.h>
#include <IPXACTmodels/Component/MemoryBlockBase.h>
#include <IPXACTmodels/Component/RegisterBase.h>
#include <IPXACTmodels/Component/Register.h>
#include <IPXACTmodels/Component/Field.h>
#include <IPXACTmodels/Component/validators/PortValidator.h>
#include <IPXACTmodels/Component/validators/FieldValidator.h>
#include <IPXACTmodels/Component/validators/EnumeratedValueValidator.h>
#include <IPXACTmodels/Component/validators/RegisterValidator.h>

#include <QCoreApplication>

//-----------------------------------------------------------------------------
// Function: PythonAPI::PythonAPI()
//-----------------------------------------------------------------------------
PythonAPI::PythonAPI():
library_(),
messager_(new PythonMessageMediator()),
activeComponent_(),
portsInterface_(new PortsInterface()),
componentParameterInterface_(new ParametersInterface()),
registerInterfaces_(),
parameterFinder_(new ComponentAndInstantiationsParameterFinder(QSharedPointer<Component>())),
expressionParser_(new IPXactSystemVerilogParser(parameterFinder_)),
expressionFormatter_(new ExpressionFormatter(parameterFinder_)),
portValidator_(new PortValidator(expressionParser_, QSharedPointer<QList<QSharedPointer<View> > >())),
parameterValidator_(new ParameterValidator(expressionParser_, QSharedPointer<QList<QSharedPointer<Choice> > >())),
registerValidator_()
{
    portsInterface_->setExpressionParser(expressionParser_);
    portsInterface_->setExpressionFormatter(expressionFormatter_);
    portsInterface_->setValidator(portValidator_);

    componentParameterInterface_->setExpressionParser(expressionParser_);
    componentParameterInterface_->setExpressionFormatter(expressionFormatter_);
    componentParameterInterface_->setValidator(parameterValidator_);

    QSharedPointer<EnumeratedValueValidator> enumValidator(new EnumeratedValueValidator(expressionParser_));
    QSharedPointer<FieldValidator>fieldValidator_(
        new FieldValidator(expressionParser_, enumValidator, parameterValidator_));

    registerValidator_ = QSharedPointer<RegisterValidator>(
        new RegisterValidator(expressionParser_, fieldValidator_, parameterValidator_));
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::setupLibrary()
//-----------------------------------------------------------------------------
void PythonAPI::setupLibrary(QString const& settingsFile)
{
    QCoreApplication::setOrganizationDomain(QStringLiteral("tut.fi"));
    QCoreApplication::setOrganizationName(QStringLiteral("TUT"));
    QCoreApplication::setApplicationName(QStringLiteral("Kactus2"));
    QCoreApplication::setApplicationVersion(VersionHelper::createVersionString());

    messager_->showMessage(settingsFile);

    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings settings(settingsFile, QSettings::IniFormat);

    QStringList pluginsPath = settings.value(QStringLiteral("Platform/PluginsPath"),
        QStringList(QStringLiteral("Plugins"))).toStringList();
    for (auto singlePath : pluginsPath)
    {
        messager_->showMessage(singlePath);
    }

    library_ = new LibraryHandler(0, messager_, 0);
    library_->searchForIPXactFiles();
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::getFileCount()
//-----------------------------------------------------------------------------
int PythonAPI::getFileCount() const
{
    return library_->getAllVLNVs().count();
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::listVLNVs()
//-----------------------------------------------------------------------------
void PythonAPI::listVLNVs(QString const& vendor /* = QString("") */) const
{
    int vlnvCount = 0;

    for (auto itemVLNV : library_->getAllVLNVs())
    {
        messager_->showMessage(itemVLNV.toString());
        vlnvCount++;
    }

    messager_->showMessage(QString("VLNVs found: %1").arg(QString::number(vlnvCount)));
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::listComponentVLNVs()
//-----------------------------------------------------------------------------
void PythonAPI::listComponentVLNVs() const
{
    int vlnvCount = 0;

    for (auto itemVLNV : library_->getAllVLNVs())
    {
        if (itemVLNV.getType() == VLNV::COMPONENT)
        {
            messager_->showMessage(itemVLNV.toString());
            vlnvCount++;
        }
    }

    messager_->showMessage(QString("Components found: %1").arg(QString::number(vlnvCount)));
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::openComponent()
//-----------------------------------------------------------------------------
bool PythonAPI::openComponent(QString const& componentVLNV)
{
    QStringList vlnvArray = componentVLNV.split(':');
    if (vlnvArray.size() != 4)
    {
        messager_->showError(QString("The given VLNV %1 is not valid").arg(componentVLNV));
        return false;
    }

    VLNV targetVLNV(VLNV::COMPONENT, vlnvArray.at(0), vlnvArray.at(1), vlnvArray.at(2), vlnvArray.at(3));
    QSharedPointer<Document> componentDocument = library_->getModel(targetVLNV);
    if (componentDocument)
    {
        QSharedPointer<Component> component = componentDocument.dynamicCast<Component>();
        if (component)
        {
            parameterFinder_->setComponent(component);
            portValidator_->componentChange(component->getViews());
            portsInterface_->setPorts(component);

            parameterValidator_->componentChange(component->getChoices());
            componentParameterInterface_->setParameters(component->getParameters());
            componentParameterInterface_->setChoices(component->getChoices());

            registerValidator_->componentChange(component->getResetTypes());

            setupRegisterInterfaces(component);
            
            messager_->showMessage(QString("Registers found: %1").arg(QString::number(registerInterfaces_.at(0)->getItemNames().size())));

            activeComponent_ = component;
            messager_->showMessage(QString("Component %1 is open").arg(componentVLNV));
            return true;
        }
        else
        {
            messager_->showError(QString("The given VLNV %1 is not a component").arg(componentVLNV));
            return false;
        }
    }
    else
    {
        messager_->showError(QString("Could not open document with VLNV %1").arg(componentVLNV));
        return false;
    }
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::setupRegisterInterfaces()
//-----------------------------------------------------------------------------
void PythonAPI::setupRegisterInterfaces(QSharedPointer<Component> component)
{
    registerInterfaces_.clear();

    for (auto map : *component->getMemoryMaps())
    {
        for (auto blockBase : *map->getMemoryBlocks())
        {
            QSharedPointer<AddressBlock> block = blockBase.dynamicCast<AddressBlock>();
            if (block)
            {
                RegisterInterface* registerInterface(new RegisterInterface());

                registerInterface->setRegisters(block->getRegisterData());
                registerInterface->setValidator(registerValidator_);
                registerInterface->setExpressionParser(expressionParser_);
                registerInterface->setExpressionFormatter(expressionFormatter_);

                setupFieldInterfaces(registerInterface, block->getRegisterData(),
                    registerValidator_->getFieldValidator());

                registerInterfaces_.push_back(registerInterface);
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::setupFieldInterfaces()
//-----------------------------------------------------------------------------
void PythonAPI::setupFieldInterfaces(RegisterInterface* containingRegisterInterface,
    QSharedPointer<QList<QSharedPointer<RegisterBase>>> registers, QSharedPointer<FieldValidator> validator) const
{
    for (auto registerBase : *registers)
    {
        QSharedPointer<Register> blockRegister = registerBase.dynamicCast<Register>();
        if (blockRegister)
        {
            FieldInterface* fieldInterface(new FieldInterface());

            fieldInterface->setFields(blockRegister->getFields());
            fieldInterface->setValidator(validator);
            fieldInterface->setExpressionParser(expressionParser_);
            fieldInterface->setExpressionFormatter(expressionFormatter_);
            
            setupResetInterfaces(fieldInterface, blockRegister->getFields(), validator);

            containingRegisterInterface->addSubInterface(blockRegister->name().toStdString(), fieldInterface);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::setupResetInterfaces()
//-----------------------------------------------------------------------------
void PythonAPI::setupResetInterfaces(FieldInterface* containingFieldInterface,
    QSharedPointer<QList<QSharedPointer<Field>>> registerFields, QSharedPointer<FieldValidator> validator) const
{
    for (auto field : *registerFields)
    {
        ResetInterface* newResetInterface(new ResetInterface());
        newResetInterface->setResets(field);
        newResetInterface->setValidator(validator);
        newResetInterface->setExpressionParser(expressionParser_);
        newResetInterface->setExpressionFormatter(expressionFormatter_);

        containingFieldInterface->addResetInterface(field->name().toStdString(), newResetInterface);
    }
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::closeOpenComponent()
//-----------------------------------------------------------------------------
void PythonAPI::closeOpenComponent()
{
    activeComponent_ = QSharedPointer<Component>();
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::getComponentName()
//-----------------------------------------------------------------------------
QString PythonAPI::getComponentName()
{
    return activeComponent_->getVlnv().getName();
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::getComponentDescription()
//-----------------------------------------------------------------------------
QString PythonAPI::getComponentDescription()
{
    return activeComponent_->getDescription();
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::saveComponent()
//-----------------------------------------------------------------------------
void PythonAPI::saveComponent()
{
    messager_->showMessage(QString("Saving component %1 ...").arg(activeComponent_->getVlnv().toString()));

    if (library_->writeModelToFile(activeComponent_))
    {
        messager_->showMessage(QString("Save complete"));
    }
    else
    {
        messager_->showError(QString("Could not save component %1").arg(activeComponent_->getVlnv().toString()));
    }
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::getPortsInterface()
//-----------------------------------------------------------------------------
PortsInterface* PythonAPI::getPortsInterface() const
{
    return portsInterface_;
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::getComponentParameterInterface()
//-----------------------------------------------------------------------------
ParametersInterface* PythonAPI::getComponentParameterInterface() const
{
    return componentParameterInterface_;
}

//-----------------------------------------------------------------------------
// Function: PythonAPI::getRegisterInterfaces()
//-----------------------------------------------------------------------------
std::vector<RegisterInterface*> PythonAPI::getRegisterInterfaces() const
{
    return registerInterfaces_;
}
