/* 
 *
 *  Created on: 9.8.2010
 *      Author: Antti Kamppi
 */

#include "componentgenerator.h"

#include <IPXACTmodels/common/Parameter.h>

#include <QList>
#include <QString>

// the constructor
ComponentGenerator::ComponentGenerator():
scope_(ComponentGenerator::INSTANCE),
groups_(), NameGroup(), hidden_(false),
phase_(-1), parameters_(), apiType_(ComponentGenerator::TGI_2014_BASE),
generatorExe_(), vendorExtensions_()
{
}

ComponentGenerator::ComponentGenerator( const ComponentGenerator &other ):
scope_(other.scope_),
	groups_(other.groups_),
	NameGroup(other),
	hidden_(other.hidden_),
	phase_(other.phase_),
	parameters_(),
	apiType_(other.apiType_),
	generatorExe_(other.generatorExe_),
	vendorExtensions_(other.vendorExtensions_)
{
		foreach (QSharedPointer<Parameter> param, other.parameters_)
		{
			if (param) {
				QSharedPointer<Parameter> copy = QSharedPointer<Parameter>(
					new Parameter(*param.data()));
				parameters_.append(copy);
			}
		}
}

ComponentGenerator& ComponentGenerator::operator=( const ComponentGenerator &other ) {
	if (this != &other) {
		scope_ = other.scope_;
		groups_ = other.groups_;
		NameGroup::operator=(other);
		hidden_ = other.hidden_;
		phase_ = other.phase_;
		apiType_ = other.apiType_;
		generatorExe_ = other.generatorExe_;
		vendorExtensions_ = other.vendorExtensions_;

		parameters_.clear();
		foreach (QSharedPointer<Parameter> param, other.parameters_)
		{
			if (param) {
				QSharedPointer<Parameter> copy = QSharedPointer<Parameter>(
					new Parameter(*param.data()));
				parameters_.append(copy);
			}
		}
	}
	return *this;
}

// the destructor
ComponentGenerator::~ComponentGenerator() {
}

bool ComponentGenerator::isValid(QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices,
    QStringList& errorList, const QString& parentIdentifier ) const {
	bool valid = true;

	if (name().isEmpty()) {
		errorList.append(QObject::tr("No name specified for component generator"
			" within %1").arg(parentIdentifier));
		valid = false;
	}

//     ParameterValidator validator;
//     foreach (QSharedPointer<Parameter> param, parameters_)
//     {
//         errorList.append(validator.findErrorsIn(param.data(), QObject::tr("component generator %1").arg(name()),
//             componentChoices));
//         if (!validator.validate(param.data(), componentChoices)) 
//         {
//             valid = false;
//         }
//     }

	if (generatorExe_.isEmpty())
    {
		errorList.append(QObject::tr("No path to the generator executable specified"
			" for component generator %1 within %2").arg(name()).arg(parentIdentifier));
		valid = false;
	}

	return valid;
}

bool ComponentGenerator::isValid(QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices) const
{
	if (name().isEmpty())
	{
		return false;
	}

//     ParameterValidator validator;
//     foreach (QSharedPointer<Parameter> param, parameters_)
//     {
//         if (!validator.validate(param.data(), componentChoices)) 
//         {
//             return false;
//         }
//     }

	if (generatorExe_.isEmpty()) {
		return false;
	}
	return true;
}

ComponentGenerator::Instance ComponentGenerator::getScope() const {
	return scope_;
}

const QList<QString>& ComponentGenerator::getGroups() {
	return groups_;
}

void ComponentGenerator::setGroups(QList<QString> &groups) {
	// delete old groups
	groups_.clear();

	// save the new groups
	groups_ = groups;
}

void ComponentGenerator::setScope(Instance scope) {
	scope_ = scope;
}

double ComponentGenerator::getPhase() const {
	return phase_;
}

void ComponentGenerator::setHidden(bool hidden) {
	hidden_ = hidden;
}

bool ComponentGenerator::getHidden() const {
	return hidden_;
}

void ComponentGenerator::setPhase(double phase) {
	phase_ = phase;
}

void ComponentGenerator::setGeneratorExe(const QString &generatorExe) {
	generatorExe_ = generatorExe;
}

void ComponentGenerator::setParameters(const QList<QSharedPointer<Parameter> > &parameters) {
	// delete old parameters
	parameters_.clear();

	// save the new parameters
	parameters_ = parameters;
}

void ComponentGenerator::setApiType(ApiType apiType) {
	apiType_ = apiType;
}

const QList<QSharedPointer<Parameter> >& ComponentGenerator::getParameters() {
	return parameters_;
}

QString ComponentGenerator::getGeneratorExe() const {
	return generatorExe_;
}

ComponentGenerator::ApiType ComponentGenerator::getApiType() const {
	return apiType_;
}

void ComponentGenerator::setTransportMethods(QStringList transportMethods)
{
	transportMethods_ = transportMethods;
}

const QStringList& ComponentGenerator::getTransportMethods() const
{
	return transportMethods_;
}
