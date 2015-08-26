//-----------------------------------------------------------------------------
// File: ComponentInstance.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 17.08.2015
//
// Description:
// Describes the ipxact:componentInstance element.
//-----------------------------------------------------------------------------

#include "ComponentInstance.h"

#include <IPXACTmodels/XmlUtils.h>

#include <IPXACTmodels/kactusExtensions/Kactus2Placeholder.h>
#include <IPXACTmodels/validators/namevalidator.h>

#include <QRegExpValidator>
#include <QUuid>

//-----------------------------------------------------------------------------
// Function: ComponentInstance::ComponentInstance()
//-----------------------------------------------------------------------------
ComponentInstance::ComponentInstance(QString instanceName, QString displayName,
                                     QString description, VLNV const& componentRef,
                                     QPointF const& position,
									 const QString& uuid) :
Extendable(),
instanceName_(instanceName),
displayName_(displayName),
desc_(description),
isPresent_(),
componentRef_(new ConfigurableVLNVReference(componentRef)),
pos_(position),
imported_(false),
importRef_(),
portPositions_(),
adHocPortPositions_(),
apiInterfacePositions_(),
comInterfacePositions_(),
portAdHocVisibilities_(),
swPropertyValues_(),
uuid_(uuid)
{
	if (uuid_.isEmpty())
    {
		uuid_ = QUuid::createUuid().toString();
	}
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::ComponentInstance()
//-----------------------------------------------------------------------------
ComponentInstance::ComponentInstance(ComponentInstance const& other) :
Extendable(other),
instanceName_(other.instanceName_),
displayName_(other.displayName_),
desc_(other.desc_),
isPresent_(other.isPresent_),
componentRef_(other.componentRef_),
pos_(other.pos_),
imported_(other.imported_),
importRef_(other.importRef_),
portPositions_(other.portPositions_),
adHocPortPositions_(other.adHocPortPositions_),
apiInterfacePositions_(other.apiInterfacePositions_),
comInterfacePositions_(other.comInterfacePositions_),
portAdHocVisibilities_(other.portAdHocVisibilities_),
swPropertyValues_(other.swPropertyValues_),
uuid_(other.uuid_)
{
	// make sure instances always have uuid
	if (uuid_.isEmpty())
    {
		uuid_ = QUuid::createUuid().toString();
	}

    copyVendorExtensions(other);    
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::ComponentInstance()
//-----------------------------------------------------------------------------
ComponentInstance::ComponentInstance() :
Extendable(),
instanceName_(),
displayName_(),
desc_(),
isPresent_(),
componentRef_(new ConfigurableVLNVReference()),
pos_(),
imported_(false),
importRef_(),
portPositions_(),
adHocPortPositions_(),
apiInterfacePositions_(),
comInterfacePositions_(),
portAdHocVisibilities_(),
swPropertyValues_(),
uuid_()
{

}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::~ComponentInstance()
//-----------------------------------------------------------------------------
ComponentInstance::~ComponentInstance()
{
    componentRef_.clear();
}
/*
//-----------------------------------------------------------------------------
// Function: ComponentInstance::isValid()
//-----------------------------------------------------------------------------
bool ComponentInstance::isValid(QStringList& errorList, QString const& parentIdentifier) const
{
    bool valid = true;
    const QString thisIdentifier(QObject::tr("component instance %1").arg(instanceName_));

    if (instanceName_.isEmpty())
    {
        errorList.append(QObject::tr("No name specified for component instance "
                                     "within %1").arg(parentIdentifier));
        valid = false;
    }

    // if the instance name contains characters that are not allowed in vhdl
    //QRegExpValidator nameValidator(QRegExp("^[a-zA-Z]+[a-zA-Z0-9_]*$"));
    NameValidator nameValidator(0);
    int pos = 0;
    QString instName(instanceName_);

    if (nameValidator.validate(instName, pos) != QValidator::Acceptable)
    {
        errorList.append(QObject::tr("The instance name %1 contains illegal characters.").arg(instName));
        valid = false;
    }
    /*
    if (!isDraft() && !componentRef_.isValid(errorList, thisIdentifier))
    {
        valid = false;
    }

    for (QMap<QString, QString>::const_iterator i = configurableElementValues_.begin();
        i != configurableElementValues_.end(); ++i)
    {
            if (i.key().isEmpty())
            {
                errorList.append(QObject::tr("No reference id set for configurable"
                                             " element value in %1 within %2").arg(thisIdentifier).arg(parentIdentifier));
                valid = false;
            }

            if (i.value().isEmpty())
            {
                errorList.append(QObject::tr("No configurable element value set"
                                             " for %1 within %2").arg(thisIdentifier).arg(parentIdentifier));
                valid = false;
            }
    }*/
/*
    return valid;
}*/
/*
//-----------------------------------------------------------------------------
// Function: ComponentInstance::isValid()
//-----------------------------------------------------------------------------
bool ComponentInstance::isValid() const
{
    if (instanceName_.isEmpty()) {
        return false;
    }

    // if the instance name contains characters that are not allowed in vhdl
    //QRegExpValidator nameValidator;
    NameValidator nameValidator(0);
    int pos = 0;
    QString instName(instanceName_);

    if (nameValidator.validate(instName, pos) != QValidator::Acceptable)
    {
        return false;
    }
    /*
    if (!isDraft() && !componentRef_.isValid())
    {
        return false;
    }

    for (QMap<QString, QString>::const_iterator i = configurableElementValues_.begin();
        i != configurableElementValues_.end(); ++i)
    {
            if (i.key().isEmpty())
            {
                return false;
            }

            if (i.value().isEmpty())
            {
                return false;
            }
    }*/
/*
    return true;
}*/

//-----------------------------------------------------------------------------
// Function: ComponentInstance::setInstanceName()
//-----------------------------------------------------------------------------
void ComponentInstance::setInstanceName(QString const& name)
{
    instanceName_ = name;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::setDisplayName()
//-----------------------------------------------------------------------------
void ComponentInstance::setDisplayName(QString const& displayName)
{
    displayName_ = displayName;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::setDescription()
//-----------------------------------------------------------------------------
void ComponentInstance::setDescription(QString const& description)
{
    desc_ = description;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::setComponentRef()
//-----------------------------------------------------------------------------
void ComponentInstance::setComponentRef(QSharedPointer<ConfigurableVLNVReference> newComponentRef)
{
    componentRef_ = newComponentRef;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::setConfigurableElementValues()
//-----------------------------------------------------------------------------
void ComponentInstance::setConfigurableElementValues(
    QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > newValues)
{
    componentRef_->setConfigurableElementValues(newValues);
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::setPosition()
//-----------------------------------------------------------------------------
void ComponentInstance::setPosition(QPointF const& pos)
{
    pos_ = pos;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::setImported()
//-----------------------------------------------------------------------------
void ComponentInstance::setImported(bool imported)
{
    imported_ = imported;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::setImportRef()
//-----------------------------------------------------------------------------
void ComponentInstance::setImportRef(QString const& nameRef)
{
    importRef_ = nameRef;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::setPropertyValues()
//-----------------------------------------------------------------------------
void ComponentInstance::setPropertyValues(QMap<QString, QString> const& values)
{
    swPropertyValues_ = values;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::updateBusInterfacePosition()
//-----------------------------------------------------------------------------
void ComponentInstance::updateBusInterfacePosition(QString const& name, QPointF const& pos)
{
    portPositions_[name] = pos;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::updateAdHocPortPosition()
//-----------------------------------------------------------------------------
void ComponentInstance::updateAdHocPortPosition(QString const& name, QPointF const& pos)
{
    adHocPortPositions_[name] = pos;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::updateApiInterfacePosition()
//-----------------------------------------------------------------------------
void ComponentInstance::updateApiInterfacePosition(QString const& name, QPointF const& pos)
{
    apiInterfacePositions_[name] = pos;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::updateComInterfacePosition()
//-----------------------------------------------------------------------------
void ComponentInstance::updateComInterfacePosition(QString const& name, QPointF const& pos)
{
    comInterfacePositions_[name] = pos;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::setPortAdHocVisibilities()
//-----------------------------------------------------------------------------
void ComponentInstance::setPortAdHocVisibilities(QMap<QString, bool> const& visibilities)
{
    portAdHocVisibilities_ = visibilities;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::getInstanceName()
//-----------------------------------------------------------------------------
QString const& ComponentInstance::getInstanceName() const
{
    return instanceName_;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::getDisplayName()
//-----------------------------------------------------------------------------
QString const& ComponentInstance::getDisplayName() const
{
    return displayName_;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::getDescription()
//-----------------------------------------------------------------------------
QString const& ComponentInstance::getDescription() const
{
    return desc_;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::getComponentRef()
//-----------------------------------------------------------------------------
QSharedPointer<ConfigurableVLNVReference> ComponentInstance::getComponentRef() const
{
    return componentRef_;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::getConfigurableElementValues()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > ComponentInstance::getConfigurableElementValues()
    const
{
    return componentRef_->getConfigurableElementValues();
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::getPosition()
//-----------------------------------------------------------------------------
QPointF const& ComponentInstance::getPosition() const
{
    return pos_;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::isImported()
//-----------------------------------------------------------------------------
bool ComponentInstance::isImported() const
{
    return imported_;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::getImportRef()
//-----------------------------------------------------------------------------
QString const& ComponentInstance::getImportRef() const
{
    return importRef_;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::getPropertyValues()
//-----------------------------------------------------------------------------
QMap<QString, QString> const& ComponentInstance::getPropertyValues() const
{
    return swPropertyValues_;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::getBusInterfacePositions()
//-----------------------------------------------------------------------------
QMap<QString, QPointF> const& ComponentInstance::getBusInterfacePositions() const
{
    return portPositions_;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::getAdHocPortPositions()
//-----------------------------------------------------------------------------
QMap<QString, QPointF> const& ComponentInstance::getAdHocPortPositions() const
{
    return adHocPortPositions_;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::getApiInterfacePositions()
//-----------------------------------------------------------------------------
QMap<QString, QPointF> const& ComponentInstance::getApiInterfacePositions() const
{
    return apiInterfacePositions_;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::getComInterfacePositions()
//-----------------------------------------------------------------------------
QMap<QString, QPointF> const& ComponentInstance::getComInterfacePositions() const
{
    return comInterfacePositions_;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::getPortAdHocVisibilities()
//-----------------------------------------------------------------------------
QMap<QString, bool> const& ComponentInstance::getPortAdHocVisibilities() const
{
    return portAdHocVisibilities_;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::operator=()
//-----------------------------------------------------------------------------
ComponentInstance& ComponentInstance::operator=(ComponentInstance const& other)
{
    if (this != &other)
    {
        instanceName_ = other.instanceName_;
        displayName_ = other.displayName_;
        desc_ = other.desc_;
        isPresent_ = other.isPresent_;
        componentRef_ = other.componentRef_;
        pos_ = other.pos_;
        imported_ = other.imported_;
        importRef_ = other.importRef_;
        portPositions_ = other.portPositions_;
        adHocPortPositions_ = other.adHocPortPositions_;
        apiInterfacePositions_ = other.apiInterfacePositions_;
        comInterfacePositions_ = other.comInterfacePositions_;
        portAdHocVisibilities_ = other.portAdHocVisibilities_;
        swPropertyValues_ = other.swPropertyValues_;
		uuid_ = other.uuid_;

        copyVendorExtensions(other);
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::parsePropertyValues()
//-----------------------------------------------------------------------------
void ComponentInstance::parsePropertyValues(QDomNode& node)
{
    for (int i = 0; i < node.childNodes().count(); ++i)
    {
        QDomNode propNode = node.childNodes().at(i);

        if (propNode.nodeName() == "kactus2:propertyValue")
        {
            QString name = propNode.attributes().namedItem("kactus2:name").nodeValue();
            QString value = propNode.attributes().namedItem("kactus2:value").nodeValue();

            swPropertyValues_.insert(name, value);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::getUuid()
//-----------------------------------------------------------------------------
QString ComponentInstance::getUuid() const
{
	return uuid_;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::isDraft()
//-----------------------------------------------------------------------------
bool ComponentInstance::isDraft() const
{
    foreach(QSharedPointer<VendorExtension> extension, *getVendorExtensions())
    {
        if (extension->type() == "kactus2:draft")
        {
            return true;
        }
    }
    return false;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::getIsPresent()
//-----------------------------------------------------------------------------
QString ComponentInstance::getIsPresent() const
{
    return isPresent_;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstance::setIsPresent()
//-----------------------------------------------------------------------------
void ComponentInstance::setIsPresent(QString const& newIsPresent)
{
    isPresent_ = newIsPresent;
}