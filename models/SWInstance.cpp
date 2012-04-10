//-----------------------------------------------------------------------------
// File: SWInstance.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 10.4.2012
//
// Description:
// SW instance class for extending IP-XACT designs.
//-----------------------------------------------------------------------------

#include "SWInstance.h"

#include "ComInterface.h"

//-----------------------------------------------------------------------------
// Function: SWInstance::SWInstance()
//-----------------------------------------------------------------------------
SWInstance::SWInstance() : instanceName_(), displayName_(), desc_(),
                           componentRef_(), fileSetRef_(), hwRef_(), pos_(),
                           comInterfaces_(), propertyValues_()
{
}

//-----------------------------------------------------------------------------
// Function: SWInstance::SWInstance()
//-----------------------------------------------------------------------------
SWInstance::SWInstance(SWInstance const& rhs)
    : instanceName_(rhs.instanceName_),
      displayName_(rhs.displayName_),
      desc_(rhs.desc_),
      componentRef_(rhs.componentRef_),
      fileSetRef_(rhs.fileSetRef_),
      hwRef_(rhs.hwRef_),
      pos_(rhs.pos_),
      comInterfaces_(),
      propertyValues_(rhs.propertyValues_)
{
    foreach (QSharedPointer<ComInterface> comIf, rhs.comInterfaces_)
    {
        comInterfaces_.append(QSharedPointer<ComInterface>(new ComInterface(*comIf.data())));
    }
}

//-----------------------------------------------------------------------------
// Function: SWInstance::SWInstance()
//-----------------------------------------------------------------------------
SWInstance::SWInstance(QDomNode& node) : instanceName_(), displayName_(), desc_(),
                                         componentRef_(), fileSetRef_(), hwRef_(), pos_(),
                                         comInterfaces_(), propertyValues_()
{
    for (int i = 0; i < node.childNodes().count(); ++i)
    {
        QDomNode childNode = node.childNodes().at(i);

        if (childNode.isComment())
        {
            continue;
        }

        if (childNode.nodeName() == "spirit:instanceName")
        {
            instanceName_ = childNode.nodeValue();
        }
        else if (childNode.nodeName() == "spirit:displayName")
        {
            displayName_ = childNode.nodeValue();
        }
        else if (childNode.nodeName() == "spirit:description")
        {
            desc_ = childNode.nodeValue();
        }
        else if (childNode.nodeName() == "kactus2:componentRef")
        {
            componentRef_ = General::createVLNV(childNode, VLNV::COMPONENT);
        }
        else if (childNode.nodeName() == "kactus2:fileSetRef")
        {
            fileSetRef_ = childNode.nodeValue();
        }
        else if (childNode.nodeName() == "kactus2:mapping")
        {
            hwRef_ = childNode.attributes().namedItem("kactus2:hwRef").nodeValue();
        }
        else if (childNode.nodeName() == "kactus2:position")
        {
            pos_.setX(childNode.attributes().namedItem("x").nodeValue().toInt());
            pos_.setY(childNode.attributes().namedItem("y").nodeValue().toInt());
        }
        else if (childNode.nodeName() == "kactus2:comInterfaces")
        {
            parseComInterfaces(childNode);
        }
        else if (childNode.nodeName() == "kactus2:propertyValues")
        {
            parsePropertyValues(childNode);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: SWInstance::~SWInstance()
//-----------------------------------------------------------------------------
SWInstance::~SWInstance()
{
}

//-----------------------------------------------------------------------------
// Function: SWInstance::setInstanceName()
//-----------------------------------------------------------------------------
void SWInstance::setInstanceName(QString const& name)
{
    instanceName_ = name;
}

//-----------------------------------------------------------------------------
// Function: SWInstance::setDisplayName()
//-----------------------------------------------------------------------------
void SWInstance::setDisplayName(QString const& displayName)
{
    displayName_ = displayName;
}

//-----------------------------------------------------------------------------
// Function: SWInstance::setDescription()
//-----------------------------------------------------------------------------
void SWInstance::setDescription(QString const& description)
{
    desc_ = description;
}

//-----------------------------------------------------------------------------
// Function: SWInstance::setComponentRef()
//-----------------------------------------------------------------------------
void SWInstance::setComponentRef(VLNV const& vlnv)
{
    componentRef_ = vlnv;
}

//-----------------------------------------------------------------------------
// Function: SWInstance::setFileSetRef()
//-----------------------------------------------------------------------------
void SWInstance::setFileSetRef(QString const& fileSetName)
{
    fileSetRef_ = fileSetName;
}


//-----------------------------------------------------------------------------
// Function: SWInstance::setMapping()
//-----------------------------------------------------------------------------
void SWInstance::setMapping(QString const& hwRef)
{
    hwRef_ = hwRef;
}

//-----------------------------------------------------------------------------
// Function: SWInstance::setPosition()
//-----------------------------------------------------------------------------
void SWInstance::setPosition(QPointF const& pos)
{
    pos_ = pos;
}

//-----------------------------------------------------------------------------
// Function: SWInstance::getInstanceName()
//-----------------------------------------------------------------------------
QString const& SWInstance::getInstanceName() const
{
    return instanceName_;
}

//-----------------------------------------------------------------------------
// Function: SWInstance::getDisplayName()
//-----------------------------------------------------------------------------
QString const& SWInstance::getDisplayName() const
{
    return displayName_;
}

//-----------------------------------------------------------------------------
// Function: SWInstance::getDescription()
//-----------------------------------------------------------------------------
QString const& SWInstance::getDescription() const
{
    return desc_;
}

//-----------------------------------------------------------------------------
// Function: SWInstance::getComponentRef()
//-----------------------------------------------------------------------------
VLNV const& SWInstance::getComponentRef() const
{
    return componentRef_;
}

//-----------------------------------------------------------------------------
// Function: SWInstance::getFileSetRef()
//-----------------------------------------------------------------------------
QString const& SWInstance::getFileSetRef() const
{
    return fileSetRef_;
}

//-----------------------------------------------------------------------------
// Function: SWInstance::getMapping()
//-----------------------------------------------------------------------------
QString const& SWInstance::getMapping() const
{
    return hwRef_;
}

//-----------------------------------------------------------------------------
// Function: SWInstance::getPosition()
//-----------------------------------------------------------------------------
QPointF const& SWInstance::getPosition() const
{
    return pos_;
}

//-----------------------------------------------------------------------------
// Function: SWInstance::operator=()
//-----------------------------------------------------------------------------
SWInstance& SWInstance::operator=(SWInstance const& rhs)
{
    if (&rhs != this)
    {
        instanceName_ = rhs.instanceName_;
        displayName_ = rhs.displayName_;
        desc_ = rhs.desc_;
        componentRef_ = rhs.componentRef_;
        fileSetRef_ = rhs.fileSetRef_;
        hwRef_ = rhs.hwRef_;
        pos_ = rhs.pos_;

        comInterfaces_.clear();

        foreach (QSharedPointer<ComInterface> comIf, rhs.comInterfaces_)
        {
            comInterfaces_.append(QSharedPointer<ComInterface>(new ComInterface(*comIf.data())));
        }

        propertyValues_ = rhs.propertyValues_;
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: SWInstance::parseComInterfaces()
//-----------------------------------------------------------------------------
void SWInstance::parseComInterfaces(QDomNode& node)
{
    for (int i = 0; i < node.childNodes().count(); ++i)
    {
        QDomNode comIfNode = node.childNodes().at(i);

        if (comIfNode.nodeName() == "kactus2:comInterface")
        {
            QSharedPointer<ComInterface> comIf(new ComInterface(comIfNode));
            comInterfaces_.append(comIf);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: SWInstance::parsePropertyValues()
//-----------------------------------------------------------------------------
void SWInstance::parsePropertyValues(QDomNode& node)
{
    for (int i = 0; i < node.childNodes().count(); ++i)
    {
        QDomNode propNode = node.childNodes().at(i);

        if (propNode.nodeName() == "kactus2:propertyValue")
        {
            QString name = propNode.attributes().namedItem("kactus2:name").nodeValue();
            QString value = propNode.attributes().namedItem("kactus2:value").nodeValue();

            propertyValues_.insert(name, value);
        }
    }
}
