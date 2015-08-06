//-----------------------------------------------------------------------------
// File: ConfigurableVLNVReference.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 04.08.2015
//
// Description:
// Configurable VLNV reference matching ipxact type configurableLibraryReference.
//-----------------------------------------------------------------------------

#include "ConfigurableVLNVReference.h"

//-----------------------------------------------------------------------------
// Function: ConfigurableVLNVReference::ConfigurableVLNVReference()
//-----------------------------------------------------------------------------
ConfigurableVLNVReference::ConfigurableVLNVReference():
VLNV(),
configurableElementValues_(new QList<QSharedPointer<ConfigurableElementValue> >)
{

}

//-----------------------------------------------------------------------------
// Function: ConfigurableVLNVReference::ConfigurableVLNVReferenceCopy()
//-----------------------------------------------------------------------------
ConfigurableVLNVReference::ConfigurableVLNVReference(const ConfigurableVLNVReference& other):
VLNV(other),
configurableElementValues_(new QList<QSharedPointer<ConfigurableElementValue> >)
{
    foreach (QSharedPointer<ConfigurableElementValue> configurable, *other.configurableElementValues_)
    {
        if (configurable)
        {
            QSharedPointer<ConfigurableElementValue> copy = QSharedPointer<ConfigurableElementValue>(
                new ConfigurableElementValue(configurable->getConfigurableValue(), configurable->getReferenceId()));
            configurableElementValues_->append(copy);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ConfigurableVLNVReference::~ConfigurableVLNVReference()
//-----------------------------------------------------------------------------
ConfigurableVLNVReference::~ConfigurableVLNVReference()
{
    configurableElementValues_->clear();
}

//-----------------------------------------------------------------------------
// Function: ConfigurableVLNVReference::ConfigurableVLNVReference()
//-----------------------------------------------------------------------------
ConfigurableVLNVReference::ConfigurableVLNVReference(const QDomNode& vlnvReferenceNode, IPXactType type):
VLNV(),
configurableElementValues_(new QList<QSharedPointer<ConfigurableElementValue> >)
{
    createVLNV(vlnvReferenceNode, type);

    for (int i = 0; i < vlnvReferenceNode.childNodes().count(); ++i) 
    {
        QDomNode tempNode = vlnvReferenceNode.childNodes().at(i);

        if (tempNode.nodeName() == "ipxact:configurableElementValues") 
        {
            for (int j = 0; j < tempNode.childNodes().count(); ++j)
            {
                QDomNode confElementNode = tempNode.childNodes().at(j);

                QString value = confElementNode.childNodes().at(0).nodeValue();

                QDomNamedNodeMap attributeMap = confElementNode.attributes();
                QString referenceID = attributeMap.namedItem(QString("referenceId")).nodeValue();

                configurableElementValues_->append(QSharedPointer<ConfigurableElementValue>(
                    new ConfigurableElementValue(value, referenceID)));
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ConfigurableVLNVReference::ConfigurableVLNVReference()
//-----------------------------------------------------------------------------
ConfigurableVLNVReference::ConfigurableVLNVReference(const IPXactType& type, const QString& vendor,
    const QString& library, const QString& name, const QString& version):
VLNV(type, vendor, library, name, version),
configurableElementValues_(new QList<QSharedPointer<ConfigurableElementValue> >)
{

}

//-----------------------------------------------------------------------------
// Function: ConfigurableVLNVReference::getConfigurableElementValues()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > ConfigurableVLNVReference::
    getConfigurableElementValues()
{
    return configurableElementValues_;
}

//-----------------------------------------------------------------------------
// Function: ConfigurableVLNVReference::setConfigurableElementValues()
//-----------------------------------------------------------------------------
void ConfigurableVLNVReference::setConfigurableElementValues(
    QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > newConfigurableElementValues)
{
    configurableElementValues_.clear();
    configurableElementValues_ = newConfigurableElementValues;
}