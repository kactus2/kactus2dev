//-----------------------------------------------------------------------------
// File: ComponentWizardImportPage.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 06.06.2013
//
// Description:
// Import page for the component wizard.
//-----------------------------------------------------------------------------

#include "ComponentWizardImportPage.h"
#include "ComponentWizardPages.h"

#include <wizards/ComponentWizard/ImportEditor/ImportEditor.h>

#include <IPXACTmodels/Component/Component.h>

#include <IPXACTmodels/kactusExtensions/Kactus2Group.h>
#include <IPXACTmodels/kactusExtensions/Kactus2Value.h>

#include <QVBoxLayout>
#include <QLineEdit>

//-----------------------------------------------------------------------------
// Function: ComponentWizardVhdlImportPage::ComponentWizardVhdlImportPage()
//-----------------------------------------------------------------------------
ComponentWizardImportPage::ComponentWizardImportPage(QSharedPointer<Component> component, 
	LibraryInterface* handler, 
    QSharedPointer <ComponentParameterFinder> parameterFinder,
    QSharedPointer<ExpressionFormatter> expressionFormatter, QWidget* parent):
QWizardPage(parent),
editor_(new ImportEditor(component, handler, parameterFinder, expressionFormatter, this))
{
    setTitle(tr("Import source file"));
    setSubTitle(tr("Choose the top-level file to import into component.\n"
        "Any model parameter not found in the input file will be removed. Any port not found "
        "in the input file will be set as phantom."));
    setFinalPage(true);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(editor_);

    connect(editor_, SIGNAL(contentChanged()), this, SIGNAL(completeChanged()), Qt::UniqueConnection);

    connect(editor_, SIGNAL(componentChanged(QSharedPointer<Component>)),
        this, SLOT(onComponentChange(QSharedPointer<Component>)), Qt::UniqueConnection);

    connect(editor_, SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(editor_, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);

    QLineEdit *dummy = new QLineEdit(this);
    dummy->setVisible(false);
    registerField(InstanceData::VERILOGINSTANCES, dummy);
}

//-----------------------------------------------------------------------------
// Function: ComponentWizardVhdlImportPage::~ComponentWizardVhdlImportPage()
//-----------------------------------------------------------------------------
ComponentWizardImportPage::~ComponentWizardImportPage()
{
}

//-----------------------------------------------------------------------------
// Function: ComponentWizardVhdlImportPage::nextId()
//-----------------------------------------------------------------------------
int ComponentWizardImportPage::nextId() const
{
    return ComponentWizardPages::VIEWS;
}

//-----------------------------------------------------------------------------
// Function: ComponentWizardVhdlImportPage::initializePage()
//-----------------------------------------------------------------------------
void ComponentWizardImportPage::initializePage()
{	
    editor_->initializeFileSelection();
}

//-----------------------------------------------------------------------------
// Function: ComponentWizardVhdlImportPage::isComplete()
//-----------------------------------------------------------------------------
bool ComponentWizardImportPage::isComplete() const
{
	return true;
}

//-----------------------------------------------------------------------------
// Function: ComponentWizardImportPage::onComponentChange()
//-----------------------------------------------------------------------------
void ComponentWizardImportPage::onComponentChange(QSharedPointer<Component> newComponent)
{
    QVector<InstanceData::instanceData> verilogInstances = getVerilogInstancesFromComponent(newComponent);
    setField(InstanceData::VERILOGINSTANCES, QVariant::fromValue(verilogInstances));

    emit componentChanged(newComponent);
}

//-----------------------------------------------------------------------------
// Function: ComponentWizardImportPage::getVerilogInstances()
//-----------------------------------------------------------------------------
QVector<InstanceData::instanceData> ComponentWizardImportPage::getVerilogInstancesFromComponent(
    QSharedPointer<Component> newComponent) const
{
    for (auto componentExtension : *newComponent->getVendorExtensions())
    {
        if (componentExtension->type() == InstanceData::VERILOGINSTANCES)
        {
            QSharedPointer<Kactus2Group> verilogInstanceGroup = componentExtension.dynamicCast<Kactus2Group>();
            if (verilogInstanceGroup)
            {
                newComponent->getVendorExtensions()->removeAll(componentExtension);
                return getVerilogInstancesFromExtension(verilogInstanceGroup);
            }
        }
    }

    return QVector<InstanceData::instanceData>();
}

//-----------------------------------------------------------------------------
// Function: ComponentWizardImportPage::getVerilogInstancesFromExtension()
//-----------------------------------------------------------------------------
QVector<InstanceData::instanceData> ComponentWizardImportPage::getVerilogInstancesFromExtension(
    QSharedPointer<Kactus2Group> instancesGroup) const
{
    QVector<InstanceData::instanceData> instances;

    for (auto extension : instancesGroup->getByType(InstanceData::SINGLEINSTANCE))
    {
        QSharedPointer<Kactus2Group> extensionGroup = extension.dynamicCast<Kactus2Group>();
        if (extensionGroup)
        {
            InstanceData::instanceData newInstance;
            newInstance.instanceName_ = getInstanceString(InstanceData::INSTANCENAME, extensionGroup);
            newInstance.moduleName_ = getInstanceString(InstanceData::MODULENAME, extensionGroup);
            newInstance.parameters_ =
                getInstanceStringPairs(InstanceData::PARAMETERS, InstanceData::SINGLEPARAMETER, extensionGroup);
            newInstance.portConnections_ =
                getInstanceStringPairs(InstanceData::CONNECTIONS, InstanceData::SINGLECONNECTION, extensionGroup);

            instances.append(newInstance);
        }
    }

    return instances;
}

//-----------------------------------------------------------------------------
// Function: ComponentWizardImportPage::getInstanceString()
//-----------------------------------------------------------------------------
QString ComponentWizardImportPage::getInstanceString(QString const& stringID,
    QSharedPointer<Kactus2Group> extensionGroup) const
{
    QList<QSharedPointer<VendorExtension> > stringExtensionList = extensionGroup->getByType(stringID);
    if (!stringExtensionList.isEmpty())
    {
        QSharedPointer<Kactus2Value> stringValue = stringExtensionList.first().dynamicCast<Kactus2Value>();
        if (stringValue)
        {
            return stringValue->value();
        }
    }

    return QString();
}

//-----------------------------------------------------------------------------
// Function: ComponentWizardImportPage::getInstanceStringPairs()
//-----------------------------------------------------------------------------
QVector<QPair<QString, QString> > ComponentWizardImportPage::getInstanceStringPairs(QString const& itemGroupID,
    QString const& itemID, QSharedPointer<Kactus2Group> extensionGroup) const
{
    QVector<QPair<QString, QString> > instanceItems;

    QList<QSharedPointer<VendorExtension> > groupExtensions = extensionGroup->getByType(itemGroupID);
    if (groupExtensions.size() == 1)
    {
        QSharedPointer<Kactus2Group> itemGroup = groupExtensions.first().dynamicCast<Kactus2Group>();
        if (itemGroup)
        {
            QList<QSharedPointer<VendorExtension> > nameValueExtensions = itemGroup->getByType(itemID);
            for (auto singleNameValueExtension : nameValueExtensions)
            {
                QSharedPointer<Kactus2Group> nameValueGroup = singleNameValueExtension.dynamicCast<Kactus2Group>();
                if (nameValueGroup)
                {
                    QPair<QString, QString> newNameValue;
                    newNameValue.first = getSingleValueFromGroup(InstanceData::SUBITEMNAME, nameValueGroup);
                    newNameValue.second = getSingleValueFromGroup(InstanceData::SUBITEMVALUE, nameValueGroup);

                    instanceItems.append(newNameValue);
                }
            }
        }
    }

    return instanceItems;
}

//-----------------------------------------------------------------------------
// Function: ComponentWizardImportPage::getSingleValueFromGroup()
//-----------------------------------------------------------------------------
QString ComponentWizardImportPage::getSingleValueFromGroup(QString const& valueID,
    QSharedPointer<Kactus2Group> extensionGroup) const
{
    QList<QSharedPointer<VendorExtension> > valueExtensions = extensionGroup->getByType(valueID);
    if (valueExtensions.size() == 1)
    {
        QSharedPointer<Kactus2Value> value = valueExtensions.first().dynamicCast<Kactus2Value>();
        if (value)
        {
            return value->value();
        }
    }

    return QString();
}
