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

					ModelParameter* parameter =
							new ModelParameter(modelParameterNode);
					modelParameters_.insert(parameter->getName(),
							QSharedPointer<ModelParameter>(parameter));
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

	for (QMap<QString, QSharedPointer<ModelParameter> >::const_iterator i = other.modelParameters_.begin();
		i != other.modelParameters_.end(); ++i) {
			if (i.value()) {
				QSharedPointer<ModelParameter> copy = QSharedPointer<ModelParameter>(
					new ModelParameter(*i.value().data()));
				modelParameters_.insert(copy->getName(), copy);
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
		for (QMap<QString, QSharedPointer<ModelParameter> >::const_iterator i =
                 other.modelParameters_.begin();
			i != other.modelParameters_.end(); ++i) {
				if (i.value()) {
					QSharedPointer<ModelParameter> copy = QSharedPointer<ModelParameter>(
						new ModelParameter(*i.value().data()));
					modelParameters_.insert(copy->getName(), copy);
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

		// go through each port
		for (QMap<QString, QSharedPointer<Port> >::iterator i = ports_.begin();
				i != ports_.end(); ++i) {

			i.value()->write(writer);
		}

		writer.writeEndElement(); // spirit:ports
	}

	// write the modelParameters of the model
	if (modelParameters_.size() != 0) {
		writer.writeStartElement("spirit:modelParameters");

		// go through each modelParameter
		for (QMap<QString, QSharedPointer<ModelParameter> >::iterator i =
				modelParameters_.begin(); i != modelParameters_.end(); ++i) {
			i.value()->write(writer);
		}

		writer.writeEndElement(); // spirit:modelParameters
	}

	writer.writeEndElement(); // spirit:model
}

const QMap<QString, QSharedPointer<ModelParameter> >& Model::getModelParameters() {
	return modelParameters_;
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

void Model::setModelParameters(const
		QMap<QString, QSharedPointer<ModelParameter> > &modelParameters) {
	// delete the old model parameters
	modelParameters_.clear();

	// save the new model parameters
	modelParameters_ = modelParameters;
}

void Model::setPorts(const QMap<QString, QSharedPointer<Port> > &ports) {
	// delete the old ports
	ports_.clear();

	// save the new ports
	ports_ = ports;
}

const QList<QSharedPointer<View> >& Model::getViews() {
	return views_;
}

bool Model::hasHierView() const {

	// search all views
	for (int i = 0; i < views_.size(); ++i) {

		// if a hierarchyref is found
		if (views_.at(i)->getHierarchyRef().isValid()) {
			return true;
		}
	}

	// if none of the views contained hierarchyrefs then return false
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

ModelParameter* Model::getParameter(const QString& name) const {

	// if model parameter doesn't exist
	if (!modelParameters_.contains(name)) {
		return 0;
	}
	else {
		return modelParameters_.value(name).data();
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

QMap<QString, QSharedPointer<ModelParameter> >* Model::getModelParametersPointer() {
	return &modelParameters_;
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

bool Model::hasModelParameters() const {
	return !modelParameters_.isEmpty();
}

bool Model::hasPorts() const {
	return !ports_.isEmpty();
}

bool Model::hasViews() const {
	return !views_.isEmpty();
}

//-----------------------------------------------------------------------------
// Function: addModelParameter()
//-----------------------------------------------------------------------------
bool Model::addModelParameter(QSharedPointer<ModelParameter> param)
{
    // Duplicates are not allowed.
    if (modelParameters_.contains(param->getName()))
        return false;

    modelParameters_.insert(param->getName(), param);
    return true;
}

//-----------------------------------------------------------------------------
// Function: removeModelParameter()
//-----------------------------------------------------------------------------
void Model::removeModelParameter(const QString& paramName)
{
    modelParameters_.remove(paramName);
}
