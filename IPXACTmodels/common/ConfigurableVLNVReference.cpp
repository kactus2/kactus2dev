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
// Function: ConfigurableVLNVReference::ConfigurableVLNVReference()
//-----------------------------------------------------------------------------
ConfigurableVLNVReference::ConfigurableVLNVReference(const IPXactType& type, const QString& vendor,
    const QString& library, const QString& name, const QString& version):
VLNV(type, vendor, library, name, version),
configurableElementValues_(new QList<QSharedPointer<ConfigurableElementValue> >)
{

}

//-----------------------------------------------------------------------------
// Function: ConfigurableVLNVReference::ConfigurableVLNVReference()
//-----------------------------------------------------------------------------
ConfigurableVLNVReference::ConfigurableVLNVReference(const VLNV& configurableVLNV):
VLNV(configurableVLNV),
configurableElementValues_(new QList<QSharedPointer<ConfigurableElementValue> > )
{

}

//-----------------------------------------------------------------------------
// Function: ConfigurableVLNVReference::~ConfigurableVLNVReference()
//-----------------------------------------------------------------------------
ConfigurableVLNVReference::~ConfigurableVLNVReference()
{
    configurableElementValues_->clear();
}

//-----------------------------------------------------------------------------
// Function: ConfigurableVLNVReference::setVLNV()
//-----------------------------------------------------------------------------
void ConfigurableVLNVReference::setVLNV(VLNV const& vlnv)
{
    VLNV::operator=(vlnv);
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

//-----------------------------------------------------------------------------
// Function: ConfigurableVLNVReference::hasConfigurableElementValue()
//-----------------------------------------------------------------------------
bool ConfigurableVLNVReference::hasConfigurableElementValue(QString const& referenceId) const
{
    foreach (QSharedPointer<ConfigurableElementValue> singleElement, *configurableElementValues_)
    {
        if (0 == referenceId.compare(singleElement->getReferenceId(), Qt::CaseInsensitive))
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ConfigurableVLNVReference::getSingleConfigurableElementValue()
//-----------------------------------------------------------------------------
QString ConfigurableVLNVReference::getSingleConfigurableElementValue(QString const& referenceId) const
{
    foreach (QSharedPointer<ConfigurableElementValue> singleElement, *configurableElementValues_)
    {
        if (0 == referenceId.compare(singleElement->getReferenceId(), Qt::CaseInsensitive))
        {
            return singleElement->getConfigurableValue();
        }
    }

    return QString();
}
