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
    for (QSharedPointer<ConfigurableElementValue> configurable : *other.configurableElementValues_)
    {
            auto copy = QSharedPointer<ConfigurableElementValue>(
                new ConfigurableElementValue(configurable->getConfigurableValue(), configurable->getReferenceId()));
            configurableElementValues_->append(copy);
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
// Function: ConfigurableVLNVReference::operator=()
//-----------------------------------------------------------------------------
ConfigurableVLNVReference& ConfigurableVLNVReference::operator=(const ConfigurableVLNVReference& other)
{
    if (this != &other)
    {
        VLNV::operator=(other);

        configurableElementValues_->clear();
        for (QSharedPointer<ConfigurableElementValue> configurable : *other.configurableElementValues_)
        {
            auto copy = QSharedPointer<ConfigurableElementValue>(
                new ConfigurableElementValue(configurable->getConfigurableValue(), configurable->getReferenceId()));
            configurableElementValues_->append(copy);
        }
    }
    return *this;
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
    return std::any_of(configurableElementValues_->cbegin(), configurableElementValues_->cend(),
        [referenceId](auto const& singleElement) { return referenceId.compare(singleElement->getReferenceId()) == 0; });
}

//-----------------------------------------------------------------------------
// Function: ConfigurableVLNVReference::getSingleConfigurableElementValue()
//-----------------------------------------------------------------------------
QString ConfigurableVLNVReference::getSingleConfigurableElementValue(QString const& referenceId) const
{
    auto element = std::find_if(configurableElementValues_->cbegin(), configurableElementValues_->cend(),
        [referenceId](auto const& singleElement) {return referenceId.compare(singleElement->getReferenceId()) == 0; });

    if (element != configurableElementValues_->cend())
    {
        return (*element)->getConfigurableValue();
    }

    return QString();
}
