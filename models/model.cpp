/* 
 *
 *  Created on: 2.8.2010
 *      Author: Antti Kamppi
 */

#include "model.h"
#include "port.h"
#include "view.h"
#include "modelparameter.h"
#include <LibraryManager/vlnv.h>

#include <QDomNode>
#include <QString>
#include <QList>
#include <QXmlStreamWriter>

#include <QDebug>

Model::Model(QDomNode &modelNode): views_(), ports_(), modelParameters_() {
	for (int i = 0; i < modelNode.childNodes().count(); ++i) {
		QDomNode tempNode = modelNode.childNodes().at(i);

		if (tempNode.nodeName() == QString("spirit:views")) {

			// go through all the views
			for (int j = 0; j < tempNode.childNodes().count(); ++j) {
				QDomNode viewNode = tempNode.childNodes().at(j);
				if (!viewNode.isComment()) {
					views_.append(QSharedPointer<View>(
							new View(viewNode)));
				}
			}

		}
		else if (tempNode.nodeName() == QString("spirit:ports")) {

			// go through all ports
			for (int j = 0; j < tempNode.childNodes().count(); ++j) {
				QDomNode portNode = tempNode.childNodes().at(j);
				if (!portNode.isComment()) {
					Port* port = new Port(portNode);
					ports_.insert(port->getName(), QSharedPointer<Port>(port));
				}
			}
		}

		else if (tempNode.nodeName() == QString("spirit:modelParameters")) {

			// go through all model parameters
			for (int j = 0; j < tempNode.childNodes().count(); ++j) {
				QDomNode modelParameterNode = tempNode.childNodes().at(j);
				if (!modelParameterNode.isComment()) {

					modelParameters_.append(QSharedPointer<ModelParameter>(new ModelParameter(modelParameterNode)));
				}
			}
		}
	}
	return;
}

Model::Model(): views_(), ports_(), modelParameters_() {
}

Model::Model( const Model &other ):
views_(),
ports_(),
modelParameters_() {

	foreach (QSharedPointer<View> view, other.views_) {
		if (view) {
			QSharedPointer<View> copy = QSharedPointer<View>(
				new View(*view.data()));
			views_.append(copy);
		}
	}

	for (QMap<QString, QSharedPointer<Port> >::const_iterator i = other.ports_.begin();
		i != other.ports_.end(); ++i) {
			if (i.value()) {
				QSharedPointer<Port> copy = QSharedPointer<Port>(
					new Port(*i.value().data()));
				ports_.insert(copy->getName(), copy);
			}
	}

	foreach (QSharedPointer<ModelParameter> modelParam, other.modelParameters_) {
		if (modelParam) {
			QSharedPointer<ModelParameter> copy = QSharedPointer<ModelParameter>(
				new ModelParameter(*modelParam.data()));
			modelParameters_.append(copy);
		}
	}
}

Model& Model::operator=( const Model &other ) {
	if (this != &other) {
		
		views_.clear();
		foreach (QSharedPointer<View> view, other.views_) {
			if (view) {
				QSharedPointer<View> copy = QSharedPointer<View>(
					new View(*view.data()));
				views_.append(copy);
			}
		}

		ports_.clear();
		for (QMap<QString, QSharedPointer<Port> >::const_iterator i = 
                 other.ports_.begin();
			i != other.ports_.end(); ++i) {
				if (i.value()) {
					QSharedPointer<Port> copy = QSharedPointer<Port>(
						new Port(*i.value().data()));
					ports_.insert(copy->getName(), copy);
				}
		}

		modelParameters_.clear();
		foreach (QSharedPointer<ModelParameter> modelParam, other.modelParameters_) {
			if (modelParam) {
				QSharedPointer<ModelParameter> copy = QSharedPointer<ModelParameter>(
					new ModelParameter(*modelParam.data()));
				modelParameters_.append(copy);
			}
		}
	}
	return *this;
}

Model::~Model() {
	views_.clear();
	ports_.clear();
	modelParameters_.clear();
	return;
}

void Model::write(QXmlStreamWriter& writer) {
	writer.writeStartElement("spirit:model");

	// write the views of the model
	if (views_.size() != 0) {
		writer.writeStartElement("spirit:views");

		// go through each view
		for (int i = 0; i < views_.size(); ++i) {
			views_.at(i)->write(writer);
		}

		writer.writeEndElement(); // spirit:views
	}

	// write the ports of the model
	if (ports_.size() != 0) {
		writer.writeStartElement("spirit:ports");

		const QStringList viewNames = getViewNames();

		// go through each port
		for (QMap<QString, QSharedPointer<Port> >::iterator i = ports_.begin();
				i != ports_.end(); ++i) {

			i.value()->write(writer, viewNames);
		}

		writer.writeEndElement(); // spirit:ports
	}

	// write the modelParameters of the model
	if (modelParameters_.size() != 0) {
		writer.writeStartElement("spirit:modelParameters");

		// go through each modelParameter
		foreach (QSharedPointer<ModelParameter> modelparam, modelParameters_) {
			modelparam->write(writer);
		}

		writer.writeEndElement(); // spirit:modelParameters
	}

	writer.writeEndElement(); // spirit:model
}

bool Model::isValid( const QStringList& fileSetNames, 
					QStringList& errorList, 
					const QString& parentIdentifier ) const {
	bool valid = true;
	bool hasViews = !views_.isEmpty();

	QStringList portNames;
	foreach (QSharedPointer<Port> port, ports_) {

		if (portNames.contains(port->getName())) {
			errorList.append(QObject::tr("%1 contains several ports with name %2").arg(
				parentIdentifier).arg(port->getName()));
			valid = false;
		}
		else {
			portNames.append(port->getName());
		}

		if (!port->isValid(hasViews, errorList, parentIdentifier)) {
			valid = false;
		}
	}

	QStringList viewNames;
	foreach (QSharedPointer<View> view, views_) {

		if (viewNames.contains(view->getName())) {
			errorList.append(QObject::tr("%1 contains several views with name %2").arg(
				parentIdentifier).arg(view->getName()));
			valid = false;
		}
		else {
			viewNames.append(view->getName());
		}

		if (!view->isValid(fileSetNames, errorList, parentIdentifier)) {
			valid = false;
		}
	}

	QStringList modelParamNames;
	foreach (QSharedPointer<ModelParameter> modelParam, modelParameters_) {

		if (modelParamNames.contains(modelParam->getName())) {
			errorList.append(QObject::tr("%1 contains several model parameters"
				" with name %2").arg(parentIdentifier).arg(modelParam->getName()));
			valid = false;
		}
		else {
			modelParamNames.append(modelParam->getName());
		}

		if (!modelParam->isValid(errorList, parentIdentifier)) {
			valid = false;
		}
	}

	return valid;
}

bool Model::isValid(const QStringList& fileSetNames) const {
	bool hasViews = !views_.isEmpty();

	QStringList portNames;
	foreach (QSharedPointer<Port> port, ports_) {

		if (portNames.contains(port->getName())) {
			return false;
		}
		else {
			portNames.append(port->getName());
		}

		if (!port->isValid(hasViews)) {
			return false;
		}
	}

	QStringList viewNames;
	foreach (QSharedPointer<View> view, views_) {

		if (viewNames.contains(view->getName())) {
			return false;
		}
		else {
			viewNames.append(view->getName());
		}

		if (!view->isValid(fileSetNames)) {
			return false;
		}
	}

	QStringList modelParamNames;
	foreach (QSharedPointer<ModelParameter> modelParam, modelParameters_) {

		if (modelParamNames.contains(modelParam->getName())) {
			return false;
		}
		else {
			modelParamNames.append(modelParam->getName());
		}

		if (!modelParam->isValid()) {
			return false;
		}
	}

	return true;
}

void Model::setViews(const QList<QSharedPointer<View> > &views) {
	// delete the old views
	views_.clear();

	// save the new views
	views_ = views;
}

const QMap<QString, QSharedPointer<Port> >& Model::getPorts() const {
	return ports_;
}

QMap<QString, QSharedPointer<Port> >& Model::getPorts() {
	return ports_;
}

void Model::setPorts(const QMap<QString, QSharedPointer<Port> > &ports) {
	// delete the old ports
	ports_.clear();

	// save the new ports
	ports_ = ports;
}

const QList<QSharedPointer<View> >& Model::getViews() const {
	return views_;
}

QList<QSharedPointer<View> >& Model::getViews() {
	return views_;
}

bool Model::hasHierView() const {

	// search all views
	for (int i = 0; i < views_.size(); ++i) {

		// if a hierarchy ref is found
		if (views_.at(i)->getHierarchyRef().isValid()) {
			return true;
		}
	}

	// if none of the views contained hierarchy refs then return false
	return false;
}

VLNV Model::getHierRef(const QString viewName) const {

	// search all views
	for (int i = 0; i < views_.size(); ++i) {

		// if the view has the given name 
		// or no name is given AND the view is hierarchical
		if (views_.at(i)->getName() == viewName ||
				(viewName.isEmpty() && views_.at(i)->getHierarchyRef().isValid())) {
			return views_.at(i)->getHierarchyRef();
		}
	}

	// if no hierarchical view was found or none matched the given name
	return VLNV();
}

void Model::setHierRef(const VLNV& vlnv, const QString& viewName /*= QString()*/ ) {

	// search all views
	foreach (QSharedPointer<View> view, views_) {
		
		// if the view has given name or no name is given AND view is hierarchical
		if (view->getName() == viewName ||
			(viewName.isEmpty() && view->getHierarchyRef().isValid())) {
			view->setHierarchyRef(vlnv);
		}
	}
}

Port* Model::getPort(const QString& name) const {

	// if port doesn't exist
	if (!ports_.contains(name)) {
		return 0;
	}
	else {
		return ports_.value(name).data();
	}
}

int Model::getPortWidth( const QString& port ) const {
	if (!ports_.contains(port)) 
		return -1;
	else {
		return ports_.value(port)->getPortSize();
	}
}

const QList<VLNV> Model::getHierarchyRefs() const {

	// list to store the pointers to VLNVs
	QList<VLNV> refs;

	// search all views
	for (int i = 0; i < views_.size(); ++i) {

		// if view contains hierarchy reference
		if (views_.at(i)->getHierarchyRef().isValid()) {
			refs.append(views_.at(i)->getHierarchyRef());
		}
	}
	return refs;
}

View* Model::findView(const QString name) const {
	// search all views
	for (int i = 0; i < views_.size(); ++i) {

		// if the view has the specified name
		if (views_.at(i)->getName() == name) {
			return views_.at(i).data();
		}
	}

	// view was not found
	return 0;
}

void Model::addView(View* newView) {

	// remove previous views with the same name.
	removeView(newView->getName());

	// add the new view
	views_.append(QSharedPointer<View>(newView));
}

void Model::removeView(const QString& name) {

	// search all views
	for (int i = 0; i < views_.size(); ++i) {

		// if the view has the specified name
		if (views_.at(i)->getName() == name) {

			// remove the view
			views_.removeAt(i);
		}
	}
}

bool Model::addPort( QSharedPointer<Port> port ) {
	
	// if port with same name already exists
	if (ports_.contains(port->getName()))
		return false;

	ports_.insert(port->getName(), port);
	return true;
}

void Model::removePort( const QString& portName ) {
	ports_.remove(portName);
}

bool Model::renamePort( const QString& oldName, const QString& newName ) {
	
	// if port with old name does not exist
	if (!ports_.contains(oldName))
		return false;

	QSharedPointer<Port> portP(ports_.value(oldName));
	ports_.remove(oldName);
	ports_.insert(newName, portP);
	return true;
}

QMap<QString, QSharedPointer<Port> >* Model::getPortsPointer() {
	return &ports_;
}

QStringList Model::getPortNames() const {
	QStringList portNames = ports_.keys();
	return portNames;
}

View* Model::createView() {

	QSharedPointer<View> viewP = QSharedPointer<View>(new View());
	views_.append(viewP);
	return viewP.data();
}

QStringList Model::getViewNames() const {

	QStringList list;
	foreach (QSharedPointer<View> view, views_) {
		list.append(view->getName());
	}
	return list;
}

int Model::viewCount() const {
	return views_.count();
}

General::Direction Model::getPortDirection( const QString& portName ) const {

	// if port is not found
	if (!ports_.contains(portName))
		return General::DIRECTION_INVALID;

	return ports_.value(portName)->getDirection();
}

bool Model::isPhysicalPort( const QString& portName ) const {
	// if port is not found
	if (!ports_.contains(portName)) {
		return false;
	}
	// if port direction is unspecified or phantom
	else if (ports_.value(portName)->getDirection() == General::DIRECTION_INVALID ||
		ports_.value(portName)->getDirection() == General::DIRECTION_PHANTOM) {
			return false;
	}
	// if port direction is valid
	else {
		return true;
	}
}

QStringList Model::getHierViews() const {

	QStringList viewList;
	foreach (QSharedPointer<View> view, views_) {
		if (view->isHierarchical())
			viewList.append(view->getName());
	}
	return viewList;
}

QStringList Model::getFileSetRefs( const QString& viewName ) const {

	QStringList fileSets;
	foreach (QSharedPointer<View> view, views_) {
		if (view->getName() == viewName) {
			
			// if view is hierarchical and contains reference to another 
			// rtl-level implementation
			if (view->isHierarchical() && !view->getTopLevelView().isEmpty()) {
				fileSets += getFileSetRefs(view->getTopLevelView());
			}
			// if view is not hierarchical
			else if (!view->isHierarchical()) {
				fileSets += view->getFileSetRefs();
			}
			return fileSets;
		}
	}
	return fileSets;
}

bool Model::hasPorts() const {
	return !ports_.isEmpty();
}

bool Model::hasViews() const {
	return !views_.isEmpty();
}

QMap<QString, QString> Model::getPortDefaultValues() const {

	QMap<QString, QString> defaultValues;

	foreach (QSharedPointer<Port> port, ports_) {
		
		// only the in and inout ports can have default values
		if (port->getDirection() == General::IN ||
			port->getDirection() == General::INOUT) {

				// if port has a default value specified
				if (!port->getDefaultValue().isEmpty()) {
					defaultValues.insert(port->getName(), port->getDefaultValue());
				}
		}
	}

	return defaultValues;
}

QStringList Model::getPortTypeDefinitions() const {

	QStringList typeDefs;
	foreach (QSharedPointer<Port> port, ports_) {

		// get all the type defs that the port uses
		QStringList portTypeDefs = port->getTypeDefinitions();
		foreach (QString portType, portTypeDefs) {

			// if the type def is not yet in the list and it is not empty
			if (!typeDefs.contains(portType) && !portType.isEmpty()) {
				typeDefs.append(portType);
			}
		}
	}
	return typeDefs;
}

bool Model::hasPortTypes() const {

	// check each port
	foreach (QSharedPointer<Port> port, ports_) {

		// if port has a type defined
		if (port->hasTypeDefinitions()) {
			return true;
		}
	}
	// none of the port had a type defined
	return false;
}

QString Model::getEntityName( const QString& viewName ) const {
	// search all views
	foreach (QSharedPointer<View> view, views_) {
		// if the specified view was found
		if (view->getName() == viewName) {

			// if view is not hierarchical
			if (!view->isHierarchical()) {
				QString modelName = view->getModelName();
				int index = modelName.indexOf(QString("("), 0);
				// if '(' was found
				if (index > 0) {
					modelName.resize(index);
				}
				return modelName;
			}
			// if view is hierarchical
			else {
				// if view references another implementation view
				if (!view->getTopLevelView().isEmpty()) {
					return getEntityName(view->getTopLevelView());
				}
			}
		}
	}

	return QString();
}

QString Model::getArchitectureName( const QString& viewName ) const {
	// search all views
	foreach (QSharedPointer<View> view, views_) {
		// if the specified view was found
		if (view->getName() == viewName) {

			// if view is not hierarchical
			if (!view->isHierarchical()) {
				QString architectureName;

				QString modelName = view->getModelName();
				int index = modelName.indexOf(QString("("), 0);
				// if '(' was found
				if (index > 0) {
					// remove the '*(' so only the stuff inside brackets is used
					architectureName = modelName.remove(0, index + 1);
					// remove the last ')' from the end of the string
					architectureName.chop(1);
				}
				return architectureName;
			}
			// if view is hierarchical
			else {
				// if view references another implementation view
				if (!view->getTopLevelView().isEmpty()) {
					return getArchitectureName(view->getTopLevelView());
				}
			}
		}
	}
	return QString();
}

QList<General::PortBounds> Model::getPortBounds() const {
	QList<General::PortBounds> portBounds;
	foreach (QSharedPointer<Port> port, ports_) {
		General::PortBounds tempBound(port->getName(), port->getLeftBound(), 
			port->getRightBound());
		portBounds.append(tempBound);
	}
	return portBounds;
}

const QList<QSharedPointer<ModelParameter> >& Model::getModelParameters() const {
	return modelParameters_;
}

QList<QSharedPointer<ModelParameter> >& Model::getModelParameters() {
	return modelParameters_;
}

QSharedPointer<ModelParameter> Model::getModelParameter( const QString& name ) const {

	foreach (QSharedPointer<ModelParameter> modelParam, modelParameters_) {
		if (modelParam->getName() == name) {
			return modelParam;
		}
	}
	return QSharedPointer<ModelParameter>();
}

void Model::setModelParameters(const QList<QSharedPointer<ModelParameter> >& modelParameters) {
	// delete the old model parameters
	modelParameters_.clear();

	// save the new model parameters
	modelParameters_ = modelParameters;
}

//-----------------------------------------------------------------------------
// Function: addModelParameter()
//-----------------------------------------------------------------------------
bool Model::addModelParameter(QSharedPointer<ModelParameter> param) {

	// no duplicates
	foreach (QSharedPointer<ModelParameter> modelParam, modelParameters_) {
		if (modelParam == param) {
			return false;
		}
	}

	modelParameters_.append(param);
	return true;
}

//-----------------------------------------------------------------------------
// Function: removeModelParameter()
//-----------------------------------------------------------------------------
void Model::removeModelParameter(const QString& paramName) {

	for (int i = 0; i < modelParameters_.size(); ++i) {
		if (modelParameters_.at(i)->getName() == paramName) {
			modelParameters_.removeAt(i);
			--i;
		}
	}
}

QStringList Model::getModelParameterNames() const {
	QStringList list;
	foreach (QSharedPointer<ModelParameter> modelParam, modelParameters_) {
		list.append(modelParam->getName());
	}
	return list;
}

bool Model::hasModelParameters() const {
	return !modelParameters_.isEmpty();
}
