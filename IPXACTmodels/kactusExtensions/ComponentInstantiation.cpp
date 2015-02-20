//-----------------------------------------------------------------------------
// File: ComponentInstantiation.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 18.02.2015
//
// Description:
// Component instance element in IP-XACT 2014.
//-----------------------------------------------------------------------------

#include "ComponentInstantiation.h"
#include "ModuleParameter.h"

#include <IPXACTmodels/modelparameter.h>

//-----------------------------------------------------------------------------
// Function: ComponentInstantiation::ComponentInstantiation()
//-----------------------------------------------------------------------------
ComponentInstantiation::ComponentInstantiation(): nameGroup_(), 
    moduleParameters_(new QList<QSharedPointer<ModelParameter> >())
{

}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiation::ComponentInstantiation()
//-----------------------------------------------------------------------------
ComponentInstantiation::ComponentInstantiation(QDomNode& node) :
nameGroup_(node), 
    moduleParameters_(new QList<QSharedPointer<ModelParameter> >())
{
    int childCount = node.childNodes().count();
    for (int i = 0; i < childCount; i++)
    {
        QDomNode childNode = node.childNodes().at(i);
        if (childNode.nodeName() == "kactus2:moduleParameters")
        {
            int moduleParameterNodes = childNode.childNodes().count();
            for (int j = 0; j < moduleParameterNodes; j++)
            {
                QDomNode moduleParameterNode = childNode.childNodes().at(j);
                addModuleParameter(QSharedPointer<ModuleParameter>(new ModuleParameter(moduleParameterNode)));
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiation::~ComponentInstantiation()
//-----------------------------------------------------------------------------
ComponentInstantiation::~ComponentInstantiation()
{

}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiation::clone()
//-----------------------------------------------------------------------------
VendorExtension* ComponentInstantiation::clone() const
{
    ComponentInstantiation* cloned = new ComponentInstantiation();
    cloned->nameGroup_ = nameGroup_;
    foreach(QSharedPointer<ModelParameter> moduleParameter, *moduleParameters_)
    {
        QSharedPointer<ModuleParameter> copy(moduleParameter.dynamicCast<ModuleParameter>()->clone());
        cloned->addModuleParameter(copy);
    }
    return cloned;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiation::type()
//-----------------------------------------------------------------------------
QString ComponentInstantiation::type() const
{
    return "kactus2:componentInstantiation";
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiation::write()
//-----------------------------------------------------------------------------
void ComponentInstantiation::write(QXmlStreamWriter& writer) const
{
    writer.writeStartElement(type());
    nameGroup_.write(writer);

    if (!moduleParameters_->isEmpty())
    {
        writer.writeStartElement("kactus2:moduleParameters");
        foreach(QSharedPointer<Parameter> moduleParameter, *moduleParameters_)
        {
            moduleParameter->write(writer);
        }
        writer.writeEndElement();
    }

    writer.writeEndElement();
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiation::setName()
//-----------------------------------------------------------------------------
void ComponentInstantiation::setName(QString const& name)
{
    nameGroup_.setName(name);
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiation::getName()
//-----------------------------------------------------------------------------
QString ComponentInstantiation::getName() const
{
    return nameGroup_.name();
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiation::addModuleParameter()
//-----------------------------------------------------------------------------
void ComponentInstantiation::addModuleParameter(QSharedPointer<ModuleParameter> moduleParameter)
{
    if (!moduleParameters_->contains(moduleParameter))
    {
        moduleParameters_->append(moduleParameter);
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiation::getModuleParameters()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<ModelParameter> > > ComponentInstantiation::getModuleParameters() const
{
    return moduleParameters_;
}
