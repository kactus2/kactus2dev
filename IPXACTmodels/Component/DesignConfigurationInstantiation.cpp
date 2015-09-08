//-----------------------------------------------------------------------------
// File: DesignConfigurationInstantiation.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 07.09.2015
//
// Description:
// Describes the ipxact:designConfigurationInstantiation element.
//-----------------------------------------------------------------------------

#include "DesignConfigurationInstantiation.h"

//-----------------------------------------------------------------------------
// Function: DesignConfigurationInstantiation::DesignConfigurationInstantiation()
//-----------------------------------------------------------------------------
DesignConfigurationInstantiation::DesignConfigurationInstantiation(QString const& name /* = QString() */,
    QString const& displayName /* = QString() */, QString const& description /* = QString() */) :
NameGroup(name, displayName, description),
Extendable(),
language_(),
languageStrictness_(),
designConfigurationRef_(new ConfigurableVLNVReference()),
parameters_(new QList<QSharedPointer<Parameter> > ())
{

}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationInstantiation::DesignConfigurationInstantiation()
//-----------------------------------------------------------------------------
DesignConfigurationInstantiation::DesignConfigurationInstantiation(const DesignConfigurationInstantiation& other) :
NameGroup(other),
Extendable(other),
language_(other.language_),
languageStrictness_(other.languageStrictness_),
designConfigurationRef_(new ConfigurableVLNVReference(*other.designConfigurationRef_.data())),
parameters_(new QList<QSharedPointer<Parameter> > ())
{
    copyParameters(other);
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationInstantiation::operator=()
//-----------------------------------------------------------------------------
DesignConfigurationInstantiation& DesignConfigurationInstantiation::operator=(
    const DesignConfigurationInstantiation& other)
{
    if (this != &other)
    {
        NameGroup::operator=(other);
        Extendable::operator=(other);

        language_ = other.language_;
        languageStrictness_ = other.languageStrictness_;

        designConfigurationRef_.clear();
        designConfigurationRef_ = QSharedPointer<ConfigurableVLNVReference> 
            (new ConfigurableVLNVReference(*other.designConfigurationRef_.data()));

        parameters_->clear();
        copyParameters(other);
    }
    return *this;
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationInstantiation::~DesignConfigurationInstantiation()
//-----------------------------------------------------------------------------
DesignConfigurationInstantiation::~DesignConfigurationInstantiation()
{
    designConfigurationRef_.clear();
    parameters_.clear();
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationInstantiation::getLanguage()
//-----------------------------------------------------------------------------
QString DesignConfigurationInstantiation::getLanguage() const
{
    return language_;
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationInstantiation::setLanguage()
//-----------------------------------------------------------------------------
void DesignConfigurationInstantiation::setLanguage(QString const& newLanguage)
{
    language_ = newLanguage;
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationInstantiation::isLangugageStrict()
//-----------------------------------------------------------------------------
bool DesignConfigurationInstantiation::isLangugageStrict() const
{
    return languageStrictness_;
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationInstantiation::setLanguageStrict()
//-----------------------------------------------------------------------------
void DesignConfigurationInstantiation::setLanguageStrict(bool strictness)
{
    languageStrictness_ = strictness;
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationInstantiation::getDesignConfigurationReference()
//-----------------------------------------------------------------------------
QSharedPointer<ConfigurableVLNVReference> DesignConfigurationInstantiation::getDesignConfigurationReference() const
{
    return designConfigurationRef_;
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationInstantiation::setDesignConfigurationReference()
//-----------------------------------------------------------------------------
void DesignConfigurationInstantiation::setDesignConfigurationReference(
    QSharedPointer<ConfigurableVLNVReference> newDesignConfigurationReference)
{
    designConfigurationRef_ = newDesignConfigurationReference;
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationInstantiation::getParameters()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<Parameter> > > DesignConfigurationInstantiation::getParameters() const
{
    return parameters_;
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationInstantiation::setParameters()
//-----------------------------------------------------------------------------
void DesignConfigurationInstantiation::setParameters(
    QSharedPointer<QList<QSharedPointer<Parameter> > > newParameters)
{
    parameters_ = newParameters;
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationInstantiation::copyParameters()
//-----------------------------------------------------------------------------
void DesignConfigurationInstantiation::copyParameters(DesignConfigurationInstantiation const& other) const
{
    foreach (QSharedPointer<Parameter> parameter, *other.parameters_)
    {
        if (parameter)
        {
            QSharedPointer<Parameter> copy = QSharedPointer<Parameter>(new Parameter(*parameter.data()));
            parameters_->append(copy);
        }
    }
}