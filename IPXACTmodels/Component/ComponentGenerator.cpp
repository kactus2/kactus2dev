//-----------------------------------------------------------------------------
// File: ComponentGenerator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 05.10.2015
//
// Description:
// Implementation for ipxact:componentGenerator element.
//-----------------------------------------------------------------------------

#include "ComponentGenerator.h"

#include <IPXACTmodels/common/Parameter.h>

#include <QList>
#include <QSharedPointer>
#include <QString>

//-----------------------------------------------------------------------------
// Function: ComponentGenerator::ComponentGenerator()
//-----------------------------------------------------------------------------
ComponentGenerator::ComponentGenerator():
NameGroup(),
Extendable(),
hidden_(),
scope_(ComponentGenerator::NO_SCOPE),
phase_(), 
parameters_(new QList<QSharedPointer<Parameter> >()),
apiType_(ComponentGenerator::EMPTY_API_TYPE),
transportMethods_(),
generatorExe_(),
groups_()
{
}

//-----------------------------------------------------------------------------
// Function: ComponentGenerator::ComponentGenerator()
//-----------------------------------------------------------------------------
ComponentGenerator::ComponentGenerator(ComponentGenerator const& other):
NameGroup(other),
Extendable(other),
hidden_(other.hidden_),
scope_(other.scope_),
phase_(other.phase_),
parameters_(new QList<QSharedPointer<Parameter> >()),
apiType_(other.apiType_),
transportMethods_(other.transportMethods_),
generatorExe_(other.generatorExe_),
groups_(other.groups_)
{
    foreach (QSharedPointer<Parameter> parameter, *other.parameters_)
    {
        QSharedPointer<Parameter> copy = QSharedPointer<Parameter>(new Parameter(*parameter.data()));
        parameters_->append(copy);
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentGenerator::operator=()
//-----------------------------------------------------------------------------    
ComponentGenerator& ComponentGenerator::operator=(ComponentGenerator const& other)
{
	if (this != &other)
    {
		NameGroup::operator=(other);
        Extendable::operator=(other);
        hidden_ = other.hidden_;
        scope_ = other.scope_;
		phase_ = other.phase_;
		apiType_ = other.apiType_;
        generatorExe_ = other.generatorExe_;
        groups_ = other.groups_;

        parameters_->clear();
        foreach (QSharedPointer<Parameter> parameter, *other.parameters_)
        {
            QSharedPointer<Parameter> copy = QSharedPointer<Parameter>(new Parameter(*parameter.data()));
            parameters_->append(copy);
        }
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: ComponentGenerator::~ComponentGenerator()
//-----------------------------------------------------------------------------
ComponentGenerator::~ComponentGenerator()
{
}

//-----------------------------------------------------------------------------
// Function: ComponentGenerator::setHidden()
//-----------------------------------------------------------------------------
void ComponentGenerator::setHidden(bool hidden)
{
    hidden_.setValue(hidden);
}

//-----------------------------------------------------------------------------
// Function: ComponentGenerator::getHidden()
//-----------------------------------------------------------------------------
BooleanValue ComponentGenerator::getHidden() const
{
    return hidden_;
}

//-----------------------------------------------------------------------------
// Function: ComponentGenerator::setScope()
//-----------------------------------------------------------------------------
void ComponentGenerator::setScope(Scope scope)
{
    scope_ = scope;
}

//-----------------------------------------------------------------------------
// Function: ComponentGenerator::getScope()
//-----------------------------------------------------------------------------
ComponentGenerator::Scope ComponentGenerator::getScope() const
{
	return scope_;
}

//-----------------------------------------------------------------------------
// Function: ComponentGenerator::setPhase()
//-----------------------------------------------------------------------------
void ComponentGenerator::setPhase(QString const& phaseExpression)
{
    phase_ = phaseExpression;
}

//-----------------------------------------------------------------------------
// Function: ComponentGenerator::getPhase()
//-----------------------------------------------------------------------------
QString ComponentGenerator::getPhase() const
{
    return phase_;
}

//-----------------------------------------------------------------------------
// Function: ComponentGenerator::getParameters()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<Parameter> > > ComponentGenerator::getParameters()
{
    return parameters_;
}

//-----------------------------------------------------------------------------
// Function: ComponentGenerator::setApiType()
//-----------------------------------------------------------------------------
void ComponentGenerator::setApiType(ApiType apiType)
{
    apiType_ = apiType;
}
//-----------------------------------------------------------------------------
// Function: ComponentGenerator::getApiType()
//-----------------------------------------------------------------------------
ComponentGenerator::ApiType ComponentGenerator::getApiType() const
{
    return apiType_;
}

//-----------------------------------------------------------------------------
// Function: ComponentGenerator::setTransportMethods()
//-----------------------------------------------------------------------------
void ComponentGenerator::setTransportMethods(QStringList const& transportMethods)
{
    transportMethods_ = transportMethods;
}

//-----------------------------------------------------------------------------
// Function: ComponentGenerator::getTransportMethods()
//-----------------------------------------------------------------------------
QStringList ComponentGenerator::getTransportMethods() const
{
    return transportMethods_;
}

//-----------------------------------------------------------------------------
// Function: ComponentGenerator::setGeneratorExe()
//-----------------------------------------------------------------------------
void ComponentGenerator::setGeneratorExe(QString const& generatorExe)
{
    generatorExe_ = generatorExe;
}

//-----------------------------------------------------------------------------
// Function: ComponentGenerator::getGeneratorExe()
//-----------------------------------------------------------------------------
QString ComponentGenerator::getGeneratorExe() const
{
    return generatorExe_;
}

//-----------------------------------------------------------------------------
// Function: ComponentGenerator::setGroups()
//-----------------------------------------------------------------------------
void ComponentGenerator::setGroups(QStringList const& groups)
{
    groups_ = groups;
}

//-----------------------------------------------------------------------------
// Function: ComponentGenerator::getGroups()
//-----------------------------------------------------------------------------
QStringList ComponentGenerator::getGroups() const
{
    return groups_;
}
