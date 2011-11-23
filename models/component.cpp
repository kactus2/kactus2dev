/* 
 *
 *  Created on: 16.7.2010
 *      Author: Antti Kamppi
 */

#include "component.h"
#include "librarycomponent.h"
#include "businterface.h"
#include "parameter.h"
#include "addressspace.h"
#include "memorymap.h"
#include "remapstate.h"
#include "model.h"
#include "otherclockdriver.h"
#include "cpu.h"
#include "componentgenerator.h"
#include "fileset.h"
#include "channel.h"
#include "choice.h"
#include "file.h"

#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <QString>
#include <QDomNamedNodeMap>
#include <QList>
#include <QStringList>
#include <QXmlStreamWriter>
#include <QFile>
#include <QMap>
#include <QDir>
#include <QFileInfo>

#include <QDebug>

// the constructor
Component::Component(QDomDocument &doc): 
LibraryComponent(doc),
busInterfaces_(), 
channels_(), 
remapStates_(), 
addressSpaces_(),
memoryMaps_(),
model_(), 
compGenerators_(), 
choices_(), 
fileSets_(),
cpus_(),
otherClockDrivers_(),
parameters_(), 
attributes_() {

	LibraryComponent::vlnv_->setType(VLNV::COMPONENT);

	// find the IP-Xact root element (spirit:component or spirit:design ...)
	QDomNodeList nodeList = doc.childNodes();
	int i = 0;
	// search for the first element with children (=document type)
	while (!nodeList.at(i).hasChildNodes()) {
		++i;
	}

	QDomNodeList children = doc.childNodes().item(i).childNodes();

	// parse the attributes
	General::parseAttributes(doc.childNodes().item(i), attributes_);

	for (int i = 0; i < children.size(); ++i) {

		// dont parse comments
		if (children.at(i).isComment()) {
			continue;
		}

		// get busInterfaces
		if (children.at(i).nodeName() == QString("spirit:busInterfaces")) {
			// call constructors for all bus interfaces
			for (int j = 0; j < children.at(i).childNodes().count(); ++j) {

				// call constructor and give the child node representing the
				// single choice as parameter
				QDomNode tempNode = children.at(i).childNodes().at(j);

				// dont parse comments
				if (!tempNode.isComment()) {

					// create the bus interface from the node.
					BusInterface* interface =
							new BusInterface(tempNode);

					// add the pointer to the map
					busInterfaces_.insert(interface->getName(),
							QSharedPointer<BusInterface>(interface));
				}
			}
		}

		// get channels
		else if (children.at(i).nodeName() == QString("spirit:channels")) {

			// go through all the channel elements
			for (int j = 0; j < children.at(i).childNodes().count(); ++j) {

				QDomNode channelNode = children.at(i).childNodes().at(j);

				// dont parse comments
				if (!channelNode.isComment()) {
					channels_.append(QSharedPointer<Channel>(
							new Channel(channelNode)));
				}
			}
		}

		// get remapStates
		else if (children.at(i).nodeName() == QString("spirit:remapStates")) {

			// go through all remap states
			for (int j = 0; j < children.at(i).childNodes().count(); ++j) {

				QDomNode remapStateNode = children.at(i).childNodes().at(j);

				// dont parse comments
				if (!remapStateNode.isComment()) {
					remapStates_.append(QSharedPointer<RemapState>(
							new RemapState(remapStateNode)));
				}
			}
		}

		// get addressSpaces
		else if (children.at(i).nodeName() == QString("spirit:addressSpaces")) {

			// go through all the addressSpace elements
			for (int j = 0; j < children.at(i).childNodes().count(); ++j) {
				QDomNode addrSpaceNode = children.at(i).childNodes().at(j);

				// don't parse comments
				if (!addrSpaceNode.isComment()) {
					addressSpaces_.append(QSharedPointer<AddressSpace>(
							new AddressSpace(addrSpaceNode)));
				}
			}
		}

		// get memoryMaps
		else if (children.at(i).nodeName() == QString("spirit:memoryMaps")) {

			// go through all the memory maps
			for (int j = 0; j < children.at(i).childNodes().count(); ++j) {
				QDomNode memoryMapNode = children.at(i).childNodes().at(j);

				// don't parse comments
				if (!memoryMapNode.isComment()) {
					memoryMaps_.append(QSharedPointer<MemoryMap>(
							new MemoryMap(memoryMapNode)));
				}
			}
		}

		// get model
		else if (children.at(i).nodeName() == QString("spirit:model")) {
			QDomNode modelNode = children.at(i);
			model_ = QSharedPointer<Model>(new Model(modelNode));
		}

		// get componentGenerators
		else if (children.at(i).nodeName() ==
				QString("spirit:componentGenerators")) {

			// call constructor for all component generators
			for (int j = 0; j < children.at(i).childNodes().count(); ++j) {

				// call constructor and give the child node representing the
				// single componentGenerator as parameter
				QDomNode generatorNode = children.at(i).childNodes().at(j);

				// don't parse comments
				if (!generatorNode.isComment()) {
					compGenerators_.append(QSharedPointer<ComponentGenerator>(
							new ComponentGenerator(generatorNode)));
				}
			}
		}

		// get choices
		else if (children.at(i).nodeName() == QString("spirit:choices")) {

			// call constructors for all choices
			for (int j = 0; j < children.at(i).childNodes().count(); ++j) {

				// call constructor and give the child node representing the
				// single choice as parameter
				QDomNode tempNode = children.at(i).childNodes().at(j);

				// don't parse comments
				if (!tempNode.isComment()) {
					choices_.append(QSharedPointer<Choice>(
							new Choice(tempNode)));
				}
			}
		}

		else if (children.at(i).nodeName() == QString("spirit:fileSets")) {

			// call constructors for all file sets
			for (int j = 0; j < children.at(i).childNodes().count(); ++j) {

				// call constructor and give the child node representing the
				// single file set as parameter
				QDomNode tempNode = children.at(i).childNodes().at(j);

				// don't parse comments
				if (!tempNode.isComment()) {
					fileSets_.append(QSharedPointer<FileSet>(
							new FileSet(tempNode)));
				}
			}
		}

		// get cpus
		else if (children.at(i).nodeName() == QString("spirit:cpus")) {

			// go through all cpus
			for (int j = 0; j < children.at(i).childNodes().count(); ++j) {
				QDomNode cpuNode = children.at(i).childNodes().at(j);

				// don't parse comments
				if (!cpuNode.isComment()) {
					cpus_.append(QSharedPointer<Cpu>(
							new Cpu(cpuNode)));
				}
			}
		}

		// get otherClockDrivers
		else if (children.at(i).nodeName() ==
				QString("spirit:otherClockDrivers")) {

			// go through all otherClockDrivers
			for (int j = 0; j < children.at(i).childNodes().count(); ++j) {
				QDomNode clockNode = children.at(i).childNodes().at(j);

				// don't parse comments
				if (!clockNode.isComment()) {
					otherClockDrivers_.append(QSharedPointer<OtherClockDriver>(
							new OtherClockDriver(clockNode)));
				}
			}
		}

		// get parameters
		else if (children.at(i).nodeName() == QString("spirit:parameters")) {
			// go through all parameters
			for (int j = 0; j < children.at(i).childNodes().count(); ++j) {

				QDomNode parameterNode = children.at(i).childNodes().at(j);

				// don't parse comments
				if (!parameterNode.isComment()) {
					parameters_.append(QSharedPointer<Parameter>(
							new Parameter(parameterNode)));
				}
			}
		}

		// get vendor extensions
		else if (children.at(i).nodeName() == QString("spirit:vendorExtensions")) {
			for (int j = 0; j < children.at(i).childNodes().count(); ++j) {
				
				QDomNode extensionNode = children.at(i).childNodes().at(j);

				// if node is for kactus2 extensions
				if (extensionNode.nodeName() == QString("kactus2:extensions")) {

					for (int k = 0; k < extensionNode.childNodes().count(); ++k) {
						
						QDomNode kactusExtension = extensionNode.childNodes().at(k);

						// if node is for kactus2 attributes
						if (kactusExtension.nodeName() == QString("kactus2:kts_attributes")) {
							parseKactus2Attributes(kactusExtension);
						}
					}
				}
			}
		}
	}
	return;
}

Component::Component(const VLNV &vlnv):
LibraryComponent(vlnv),
busInterfaces_(),
channels_(),
remapStates_(),
addressSpaces_(),
memoryMaps_(),
model_(),
compGenerators_(),
choices_(),
fileSets_(),
cpus_(),
otherClockDrivers_(),
parameters_(), 
attributes_() {

	LibraryComponent::vlnv_->setType(VLNV::COMPONENT);
}

Component::Component():
LibraryComponent(),
busInterfaces_(), 
channels_(),
remapStates_(),
addressSpaces_(),
memoryMaps_(),
model_(),
compGenerators_(), 
choices_(),
fileSets_(), 
cpus_(), 
otherClockDrivers_(), 
parameters_(),
attributes_() {

}

Component::Component( const Component &other ):
LibraryComponent(other),
busInterfaces_(),
channels_(),
remapStates_(),
addressSpaces_(),
memoryMaps_(),
model_(),
compGenerators_(),
choices_(),
fileSets_(),
cpus_(),
otherClockDrivers_(),
parameters_(),
attributes_(other.attributes_) {

	for (QMap<QString, QSharedPointer<BusInterface> >::const_iterator i = other.busInterfaces_.begin();
		i != other.busInterfaces_.end(); ++i) {
			if (i.value()) {
				QSharedPointer<BusInterface> copy = QSharedPointer<BusInterface>(
					new BusInterface(*i.value().data()));
				busInterfaces_.insert(copy->getName(), copy);
			}
	}

	foreach (QSharedPointer<Channel> channel, other.channels_) {
		if (channel) {
			QSharedPointer<Channel> copy = QSharedPointer<Channel>(
				new Channel(*channel.data()));
			channels_.append(copy);
		}
	}

	foreach (QSharedPointer<RemapState> remState, other.remapStates_) {
		if (remState) {
			QSharedPointer<RemapState> copy = QSharedPointer<RemapState>(
				new RemapState(*remState.data()));
			remapStates_.append(copy);
		}
	}

	foreach (QSharedPointer<AddressSpace> addrSpace, other.addressSpaces_) {
		if (addrSpace) {
			QSharedPointer<AddressSpace> copy = QSharedPointer<AddressSpace>(
				new AddressSpace(*addrSpace.data()));
			addressSpaces_.append(copy);
		}
	}

	foreach (QSharedPointer<MemoryMap> memMap, other.memoryMaps_) {
		if (memMap) {
			QSharedPointer<MemoryMap> copy = QSharedPointer<MemoryMap>(
				new MemoryMap(*memMap.data()));
			memoryMaps_.append(copy);
		}
	}

	if (other.model_) {
		model_ = QSharedPointer<Model>(new Model(*other.model_.data()));
	}

	foreach (QSharedPointer<ComponentGenerator> compGen, other.compGenerators_) {
		if (compGen) {
			QSharedPointer<ComponentGenerator> copy = QSharedPointer<ComponentGenerator>(
				new ComponentGenerator(*compGen.data()));
			compGenerators_.append(copy);
		}
	}

	foreach (QSharedPointer<Choice> choice, other.choices_) {
		if (choice) {
			QSharedPointer<Choice> copy = QSharedPointer<Choice>(
				new Choice(*choice.data()));
			choices_.append(copy);
		}
	}

	foreach (QSharedPointer<FileSet> fileSet, other.fileSets_) {
		if (fileSet) {
			QSharedPointer<FileSet> copy = QSharedPointer<FileSet>(
				new FileSet(*fileSet.data()));
			fileSets_.append(copy);
		}
	}

	foreach (QSharedPointer<Cpu> cpu, other.cpus_) {
		if (cpu) {
			QSharedPointer<Cpu> copy = QSharedPointer<Cpu>(
				new Cpu(*cpu.data()));
			cpus_.append(copy);
		}
	}

	foreach (QSharedPointer<OtherClockDriver> othClock, other.otherClockDrivers_) {
		if (othClock) {
			QSharedPointer<OtherClockDriver> copy = QSharedPointer<OtherClockDriver>(
				new OtherClockDriver(*othClock.data()));
			otherClockDrivers_.append(copy);
		}
	}

	foreach (QSharedPointer<Parameter> param, other.parameters_) {
		if (param) {
			QSharedPointer<Parameter> copy = QSharedPointer<Parameter>(
				new Parameter(*param.data()));
			parameters_.append(copy);
		}
	}
}

Component & Component::operator=( const Component &other ) {
	if (this != &other) {
		LibraryComponent::operator=(other);
        attributes_ = other.attributes_;

		busInterfaces_.clear();
		for (QMap<QString, QSharedPointer<BusInterface> >::const_iterator i = other.busInterfaces_.begin();
			i != other.busInterfaces_.end(); ++i) {
				if (i.value()) {
					QSharedPointer<BusInterface> copy = QSharedPointer<BusInterface>(
						new BusInterface(*i.value().data()));
					busInterfaces_.insert(copy->getName(), copy);
				}
		}

		channels_.clear();
		foreach (QSharedPointer<Channel> channel, other.channels_) {
			if (channel) {
				QSharedPointer<Channel> copy = QSharedPointer<Channel>(
					new Channel(*channel.data()));
				channels_.append(copy);
			}
		}

		remapStates_.clear();
		foreach (QSharedPointer<RemapState> remState, other.remapStates_) {
			if (remState) {
				QSharedPointer<RemapState> copy = QSharedPointer<RemapState>(
					new RemapState(*remState.data()));
				remapStates_.append(copy);
			}
		}

		addressSpaces_.clear();
		foreach (QSharedPointer<AddressSpace> addrSpace, other.addressSpaces_) {
			if (addrSpace) {
				QSharedPointer<AddressSpace> copy = QSharedPointer<AddressSpace>(
					new AddressSpace(*addrSpace.data()));
				addressSpaces_.append(copy);
			}
		}

		memoryMaps_.clear();
		foreach (QSharedPointer<MemoryMap> memMap, other.memoryMaps_) {
			if (memMap) {
				QSharedPointer<MemoryMap> copy = QSharedPointer<MemoryMap>(
					new MemoryMap(*memMap.data()));
				memoryMaps_.append(copy);
			}
		}

		if (other.model_) {
			model_ = QSharedPointer<Model>(new Model(*other.model_.data()));
		}
		else
			model_ = QSharedPointer<Model>();

		compGenerators_.clear();
		foreach (QSharedPointer<ComponentGenerator> compGen, other.compGenerators_) {
			if (compGen) {
				QSharedPointer<ComponentGenerator> copy = QSharedPointer<ComponentGenerator>(
					new ComponentGenerator(*compGen.data()));
				compGenerators_.append(copy);
			}
		}

		choices_.clear();
		foreach (QSharedPointer<Choice> choice, other.choices_) {
			if (choice) {
				QSharedPointer<Choice> copy = QSharedPointer<Choice>(
					new Choice(*choice.data()));
				choices_.append(copy);
			}
		}

		fileSets_.clear();
		foreach (QSharedPointer<FileSet> fileSet, other.fileSets_) {
			if (fileSet) {
				QSharedPointer<FileSet> copy = QSharedPointer<FileSet>(
					new FileSet(*fileSet.data()));
				fileSets_.append(copy);
			}
		}

		cpus_.clear();
		foreach (QSharedPointer<Cpu> cpu, other.cpus_) {
			if (cpu) {
				QSharedPointer<Cpu> copy = QSharedPointer<Cpu>(
					new Cpu(*cpu.data()));
				cpus_.append(copy);
			}
		}

		otherClockDrivers_.clear();
		foreach (QSharedPointer<OtherClockDriver> othClock, other.otherClockDrivers_) {
			if (othClock) {
				QSharedPointer<OtherClockDriver> copy = QSharedPointer<OtherClockDriver>(
					new OtherClockDriver(*othClock.data()));
				otherClockDrivers_.append(copy);
			}
		}

		parameters_.clear();
		foreach (QSharedPointer<Parameter> param, other.parameters_) {
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
Component::~Component() {
	busInterfaces_.clear();
	channels_.clear();
	remapStates_.clear();
	addressSpaces_.clear();
	memoryMaps_.clear();
	compGenerators_.clear();
	choices_.clear();
	fileSets_.clear();
	cpus_.clear();
	otherClockDrivers_.clear();
	parameters_.clear();
	return;
}

QSharedPointer<LibraryComponent> Component::clone() {
	return QSharedPointer<LibraryComponent>(new Component(*this));
}

void Component::write(QFile& file) {
	// create a writer instance and set it to operate on the given file
	QXmlStreamWriter writer(&file);

	// writer automatically adds whitespaces to make document more readable
	writer.setAutoFormatting(true);
	// writer uses one tab as indentation
	writer.setAutoFormattingIndent(-1);

	// call the base class implementation to write the top comment and
	// vlvn info. It also starts the type element (i.e <spirit:busDefinition>)
	LibraryComponent::write(writer);

	// write the attributes for the spirit:component element
	General::writeAttributes(writer, attributes_);

	// call base class to write the VLNV info
	LibraryComponent::writeVLNV(writer);

	if (busInterfaces_.size() != 0) {
		writer.writeStartElement("spirit:busInterfaces");

		// write each busInterface
		for (QMap<QString, QSharedPointer<BusInterface> >::iterator i =
				busInterfaces_.begin(); i != busInterfaces_.end(); ++i) {

			i.value()->write(writer);
		}

		writer.writeEndElement(); // spirit:busInterfaces
	}

	if (channels_.size() != 0) {
		writer.writeStartElement("spirit:channels");

		// write each channel
		for (int i = 0; i < channels_.size(); ++i) {
			channels_.at(i)->write(writer);
		}

		writer.writeEndElement(); // spirit:channels
	}

	if (remapStates_.size() != 0) {
		writer.writeStartElement("spirit:remapStates");

		// write each remapState
		for (int i = 0; i < remapStates_.size(); ++i) {
			remapStates_.at(i)->write(writer);
		}

		writer.writeEndElement(); // spirit:remapStates
	}

	if (addressSpaces_.size() != 0) {
		writer.writeStartElement("spirit:addressSpaces");

		// write each addressSpace
		for (int i = 0; i < addressSpaces_.size(); ++i) {
			addressSpaces_.at(i)->write(writer);
		}

		writer.writeEndElement(); // spirit:addressSpaces
	}

	if (memoryMaps_.size() != 0) {
		writer.writeStartElement("spirit:memoryMaps");

		// write each memoryMap
		for (int i = 0; i < memoryMaps_.size(); ++i) {
			// each memorymap root element must be written outside of
			// memoryMap::write() function
			writer.writeStartElement("spirit:memoryMap");
			memoryMaps_.at(i)->write(writer);
			writer.writeEndElement(); // spirit:memoryMap
		}

		writer.writeEndElement(); // spirit:memoryMaps
	}

	if (model_) {
		model_->write(writer);
	}

	if (compGenerators_.size() != 0) {
		writer.writeStartElement("spirit:componentGenerators");

		// write each component generator
		for (int i = 0; i < compGenerators_.size(); ++i) {
			compGenerators_.at(i)->write(writer);
		}

		writer.writeEndElement(); // spirit:componeneGenerators
	}

	if (choices_.size() != 0) {
		writer.writeStartElement("spirit:choices");

		// write each choice
		for (int i = 0; i < choices_.size(); ++i) {
			choices_.at(i)->write(writer);
		}

		writer.writeEndElement(); // spirit:choices
	}

	if (fileSets_.size() != 0) {
		writer.writeStartElement("spirit:fileSets");

		// write each file set
		for (int i = 0; i < fileSets_.size(); ++i) {
			fileSets_.at(i)->write(writer);
		}

		writer.writeEndElement(); // spirit:fileSets
	}

	if (cpus_.size() != 0) {
		writer.writeStartElement("spirit:cpus");

		// write each cpus
		for (int i = 0; i < cpus_.size(); ++i) {
			cpus_.at(i)->write(writer);
		}

		writer.writeEndElement(); // spirit:cpus
	}

	if (otherClockDrivers_.size() != 0) {
		writer.writeStartElement("spirit:otherClockDrivers");

		// write each otherClockDriver
		for (int i = 0; i < otherClockDrivers_.size(); ++i) {
			otherClockDrivers_.at(i)->write(writer);
		}

		writer.writeEndElement(); // spirit:otherClockDrivers
	}

	// if description is specified in the base class
	if (!LibraryComponent::description_.isEmpty()) {
		writer.writeTextElement("spirit:description", description_);
	}

	if (parameters_.size() != 0) {
		writer.writeStartElement("spirit:parameters");

		// write each parameter
		for (int i = 0; i < parameters_.size(); ++i) {
			parameters_.at(i)->write(writer);
		}

		writer.writeEndElement(); // spirit:parameters
	}

	// if contains kactus2 attributes
	if (!kactus2Attributes_.isEmpty()) {
		writer.writeStartElement("spirit:vendorExtensions");
		writer.writeStartElement("kactus2:extensions");
		
		writeKactus2Attributes(writer);

		writer.writeEndElement(); // kactus2:extensions
		writer.writeEndElement(); // spirit:vendorExtensions
	}

	writer.writeEndElement(); // spirit:component
	writer.writeEndDocument();
	return;

}

// get the attributes
const QMap<QString, QString>& Component::getAttributes() const {
	return attributes_;
}

// set the attributes
void Component::setAttributes(const QMap<QString, QString>& attributes) {
	attributes_ = attributes;
}

Model *Component::getModel() {
	
	// if model is defined for this component
	if (model_)
		return model_.data();
	
	// if model is not defined then create an empty model and return pointer to
	// it
	else {
		model_ = QSharedPointer<Model>(new Model());
		return model_.data();
	}
}

const QList<QSharedPointer<ComponentGenerator> >&
Component::getCompGenerators() const {
	return compGenerators_;
}

QList<QSharedPointer<Parameter> >& Component::getParameters() {
	return parameters_;
}

const QList<QSharedPointer<MemoryMap> >& Component::getMemoryMaps() const {
	return memoryMaps_;
}

const QList<QSharedPointer<Cpu> >& Component::getCpus() const {
	return cpus_;
}

void Component::setOtherClockDrivers(const
QList<QSharedPointer<OtherClockDriver> > &otherClockDrivers) {
	// first remove old clock drivers and free the memory
	otherClockDrivers_.clear();

	// save new otherclockdrivers
	otherClockDrivers_ = otherClockDrivers;
}

void Component::setModel(Model *model) {
	// if an old model exists, delete it
	if (!model_) {
		model_.clear();
	}
	model_ = QSharedPointer<Model>(model);
}

const QList<QSharedPointer<FileSet> >& Component::getFileSets() const {
	return fileSets_;
}

void Component::setParameters(const QList<QSharedPointer<Parameter> >
&parameters) {
	// remove old parameters first
	parameters_.clear();
	parameters_ = parameters;
}

void Component::setCpus(const QList<QSharedPointer<Cpu> > &cpus) {
	// delete old cpus
	cpus_.clear();

	// save new cpus
	cpus_ = cpus;
}

void Component::setMemoryMaps(const QList<QSharedPointer<MemoryMap> >
&memoryMaps) {
	// remove old memory maps first
	memoryMaps_.clear();

	// save new memory maps
	memoryMaps_ = memoryMaps;
}

void Component::setChannels(const QList<QSharedPointer<Channel> > &channels) {
	// delete old channels
	channels_.clear();

	// save new channels
	channels_ = channels;
}

void Component::setChoices(const QList<QSharedPointer<Choice> > &choices) {
	// delete old choices
	choices_.clear();

	// save new choices
	choices_ = choices;
}

void Component::setBusInterfaces(const
QMap<QString, QSharedPointer<BusInterface> > &busInterfaces) {
	// delete old bus interfaces
	busInterfaces_.clear();

	// save new bus interfaces
	busInterfaces_ = busInterfaces;
}

void Component::setFileSets(const QList<QSharedPointer<FileSet> > &fileSets) {
	// delete old file sets
	fileSets_.clear();

	// save new file sets
	fileSets_ = fileSets;
}

const QList<QSharedPointer<Channel> >& Component::getChannels() const {
	return channels_;
}

void Component::setCompGenerators(const
QList<QSharedPointer<ComponentGenerator> > &compGenerators) {
	// delete old component generators
	compGenerators_.clear();

	// save new component generators
	compGenerators_ = compGenerators;
}

void Component::setAddressSpaces(const
QList<QSharedPointer<AddressSpace> > &addressSpaces) {
	// delete old address spaces
	addressSpaces_.clear();

	// save new address spaces
	addressSpaces_ = addressSpaces;
}

void Component::setRemapStates(const
QList<QSharedPointer<RemapState> > &remapStates) {
	// delete old remapstates
	remapStates_.clear();

	// save new remap states
	remapStates_ = remapStates;
}

const QMap<QString, QSharedPointer<BusInterface> >&
Component::getBusInterfaces() const {
	return busInterfaces_;
}

const QList<QSharedPointer<AddressSpace> >& Component::getAddressSpaces() const {
	return addressSpaces_;
}

const QList<QSharedPointer<RemapState> >& Component::getRemapStates() const {
	return remapStates_;
}

const QList<QSharedPointer<Choice> >& Component::getChoices() const {
	return choices_;
}

QList<QSharedPointer<OtherClockDriver> >&
Component::getOtherClockDrivers() {
	return otherClockDrivers_;
}

const QList<VLNV> Component::getDependentVLNVs() const {
	QList<VLNV> vlnvList;

	// search all bus interfaces
	for (QMap<QString, QSharedPointer<BusInterface> >::const_iterator i =
			busInterfaces_.begin(); i != busInterfaces_.end(); ++i) {

		// if abstraction type is defined
		if (i.value()->getAbstractionType().isValid()) {
			vlnvList.append(i.value()->getAbstractionType());
		}
		// if bus type is defined
		if (i.value()->getBusType().isValid()) {
			vlnvList.append(i.value()->getBusType());
		}
	}

	// ask model for all hierarchyrefs
	if (model_)
		vlnvList += model_->getHierarchyRefs();

	return vlnvList;
}

const QStringList Component::getDependentFiles() const {
	QStringList files;

	// find the file sets
	for (int i = 0; i < fileSets_.size(); ++i) {
		files += fileSets_.at(i)->getFilePaths();
	}

	// find the component generators
	for (int i = 0; i < compGenerators_.size(); ++i) {
		files += compGenerators_.at(i)->getGeneratorExe();
	}
	return files;
}

BusInterface* Component::getBusInterface(const QString& name) {

	// if bus interface is found
	if (busInterfaces_.contains(name)) {
		return busInterfaces_.value(name).data();
	}

	// if named bus interface couldn't be found
	return 0;
}

QList<General::LibraryFilePair> Component::getVhdlLibraries() const {
	QList<General::LibraryFilePair> libraries;

	// take libraries from all file sets
	for (int i = 0; i < fileSets_.size(); ++i) {
		libraries += fileSets_.at(i)->getVhdlLibraries();
	}

	return libraries;
}

QStringList Component::getVhdlLibraries( const QString& viewName ) const {

	QStringList fileSetNames;

	// if view was specified
	if (!viewName.isEmpty()) {

		// if theres no views
		if (!model_) {
			return QStringList();
		}

		// get the names of the file sets that are used.
		fileSetNames = model_->getFileSetRefs(viewName);
	}
	// view was not specified so take all file sets
	else {
		foreach (QSharedPointer<FileSet> fileSet, fileSets_) {
			fileSetNames.append(fileSet->getName());
		}
	}

	QStringList libraries;
	// each file set referenced by the view
	foreach (QString fileSetName, fileSetNames) {

		foreach (QSharedPointer<FileSet> fileSet, fileSets_) {
			
			// if file set is the searched one
			if (fileSet->getName() == fileSetName) {
				libraries += fileSet->getVhdlLibraryNames();
			}
		}
	}
	libraries.removeDuplicates();
	return libraries;
}

QList<General::LibraryFilePair> Component::getLibraries( const QString& viewName ) const {

	QList<General::LibraryFilePair> libraries;
	// if no model then there are no views.
	if (!model_)
		return libraries;

	// ask the model for references to file sets.
	QStringList fileSets = model_->getFileSetRefs(viewName);
	foreach (QString fileSetName, fileSets) {

		// find the matching file set
		foreach (QSharedPointer<FileSet> fileSet, fileSets_) {
			if (fileSet->getName() == fileSetName) {
				libraries += fileSet->getVerilogLibraries();
				libraries += fileSet->getVhdlLibraries();
			}
		}
	}
	return libraries;
}

QList<General::LibraryFilePair> Component::getVerilogLibraries() const {
	QList<General::LibraryFilePair> libraries;

	// take libraries from all file sets
	for (int i = 0; i < fileSets_.size(); ++i) {
		libraries += fileSets_.at(i)->getVerilogLibraries();
	}

	return libraries;
}

bool Component::isCpu() const {
	if (cpus_.size() != 0) {
		return true;
	}
	return false;
}


bool Component::isBus() const {

	// if there is channels this is bus component
	if (channels_.size() != 0 ) {
		return true;
	}

	// if bridge is found in one of the bus interfaces this is bus component
	for (QMap<QString, QSharedPointer<BusInterface> >::const_iterator i =
			busInterfaces_.begin(); i != busInterfaces_.end(); ++i) {

		// if one of the bus interfaces has a bridge, this is bus component
		if (i.value()->hasBridge()) {
			return true;
		}
	}

	// if no channel or bridge was found
	return false;
}

bool Component::isChannel() const {
	return !channels_.isEmpty();
}

bool Component::isBridge() const {
	
	// search all bus interfaces.
	foreach (QSharedPointer<BusInterface> busif, busInterfaces_) {

		// if one bridge is found
		if (busif->hasBridge())
			return true;
	}
	// not a single bridge was found
	return false;
}


bool Component::isHierarchical() const {

	// if no model element is defined then there is no hierarchical view
	if (!model_)
		return false;

	// if the model-element has atleast one hierachical view defined
	return model_->hasHierView();
}

Port* Component::getPort(const QString& name) const {
	if (model_)
		// ask model for the port
		return model_->getPort(name);

	return 0;
}

int Component::getPortWidth( const QString& port ) const {
	if (model_)
		return model_->getPortWidth(port);

	return -1;
}

bool Component::hasPort( const QString& name ) const {
	return model_->getPort(name);
}

FileSet* Component::getFileSet(const QString& name) const {
	// search all file sets
	for (int i = 0; i < fileSets_.size(); ++i) {

		// if the file set was found
		if (fileSets_.at(i)->getName() == name) {
			return fileSets_.at(i).data();
		}
	}

	// file set was not found
	return 0;
}

void Component::addFileSet(FileSet* fileSet) {
	fileSets_.append(QSharedPointer<FileSet>(fileSet));
}

QStringList Component::getFiles(const QString fileSetName) {

	QStringList files;

	// search all file sets
	for (int i = 0; i < fileSets_.size(); ++i) {

		// if the file set has the specified name
		if (fileSets_.at(i)->getName() == fileSetName) {
			files += fileSets_.at(i)->getFileNames();
		}
	}
	return files;
}

QStringList Component::getFiles() const {

	QStringList files;

	foreach (QSharedPointer<FileSet> fileSet, fileSets_) {
		files += fileSet->getFileNames();
	}
	return files;
}

void Component::removeFileSet( const QString& fileSetName ) {
	for (int i = 0; i <fileSets_.size(); ++i) {
		
		// if the file set is the specified one
		if (fileSets_.at(i)->getName() == fileSetName) {
			fileSets_.value(i).clear();
			fileSets_.removeAt(i);
			return;
		}
	}
}

FileSet* Component::createFileSet() {
	// create an empty file set
	fileSets_.append(QSharedPointer<FileSet>(new FileSet()));
	// and return pointer to it
	return fileSets_.last().data();
}

bool Component::addPort( QSharedPointer<Port> port ) {
	
	// if no model has been specified for the component
	if (!model_)
		model_ = QSharedPointer<Model>(new Model());

	return model_->addPort(port);
}

void Component::removePort( const QString& portName ) {
	
	// if no model has been specified for the component
	if (!model_)
		model_ = QSharedPointer<Model>(new Model());

	model_->removePort(portName);
}

bool Component::renamePort( const QString& oldName, const QString& newName ) {

	// if no model has been specified for the component
	if (!model_)
		model_ = QSharedPointer<Model>(new Model());

	return model_->renamePort(oldName, newName);
}

bool Component::addBusInterface(QSharedPointer<BusInterface> busInterface ) {

	if (busInterfaces_.contains(busInterface->getName()))
		return false;

	busInterfaces_.insert(busInterface->getName(), busInterface);
	return true;
}

void Component::removeBusInterface( const QString& busifName ) {

	busInterfaces_.remove(busifName);
}

void Component::removeBusInterface( const BusInterface* busInterface ) {
	// get all bus interface pointers
	QList<QSharedPointer<BusInterface> > list = busInterfaces_.values();
	// and check if any matches with given bus interface
	foreach (QSharedPointer<BusInterface> busif, list) {

		// if they match
		if (busif.data() == busInterface) {

			// remove the old item in the map
			busInterfaces_.remove(busif->getName());
			continue;
		}
	}
}

Channel* Component::createChannel() {

	QSharedPointer<Channel> chan = QSharedPointer<Channel>(new Channel());
	channels_.append(chan);
	return chan.data();
}

QStringList Component::getBusInterfaceNames() const {

	QStringList list;
	foreach (QSharedPointer<BusInterface> busif, busInterfaces_) {
		list.append(busif->getName());
	}
	return list;
}

void Component::updateBusInterface(BusInterface* busInterface ) {

	// get all bus interface pointers
	QList<QSharedPointer<BusInterface> > list = busInterfaces_.values();
	// and check if any matches with given bus interface
	foreach (QSharedPointer<BusInterface> busif, list) {

		// if they match
		if (busif.data() == busInterface) {

			// remove the old item in the map
			busInterfaces_.take(busInterfaces_.key(busif)/*busif->getName()*/);

			// update the name
			busif->setName(busInterface->getName());

			// add new item to the map 
			busInterfaces_.insert(busif->getName(), busif);

			return;
		}
	}

	// if no match was found, add the bus interface to the map
	busInterfaces_.insert(busInterface->getName(), 
		QSharedPointer<BusInterface>(busInterface));
}

BusInterface* Component::createBusInterface() {

	BusInterface* busif = new BusInterface();

	// there might another bus interface with no name so must use insertMulti
	busInterfaces_.insertMulti(busif->getName(), QSharedPointer<BusInterface>(busif));
	return busif;
}

const QStringList Component::getAddressSpaceNames() const {

	// the list to store the names
	QStringList list;

	// get the names of all address spaces.
	foreach (QSharedPointer<AddressSpace> addrSpace, addressSpaces_) {
		list.append(addrSpace->getName());
	}
	return list;
}

QStringList Component::getPortNames() const {

	// if model exists then ask for it's ports
	if (model_)
		return model_->getPortNames();

	// if there is no model there is no ports either.
	else
		return QStringList();
}

QStringList Component::getFileSetNames() const {

	QStringList list;

	foreach (QSharedPointer<FileSet> fileSet, fileSets_) {
		list.append(fileSet->getName());
	}
	return list;
}

bool Component::hasFileSet( const QString& fileSetName ) const {

	foreach (QSharedPointer<FileSet> fileSet, fileSets_) {
		
		// if the file set is found.
		if (fileSet->getName() == fileSetName)
			return true;
	}

	// all file sets were searched but none found
	return false;
}

QList<QSharedPointer<File> > Component::getRTLFiles() const {

	QList<QSharedPointer<File> > list;
	foreach (QSharedPointer<FileSet> fileSet, fileSets_) {
		list += fileSet->getRTLFiles();
	}
	return list;
}

const QMap<QString, QSharedPointer<ModelParameter> > Component::getModelParameters() const {
	if (model_) {
		return model_->getModelParameters();
	}
	// if theres no model theres no model parameters
    // Pekka: cannot return a reference to a temporary object!
	return QMap<QString, QSharedPointer<ModelParameter> >();
}

General::Direction Component::getPortDirection( const QString& portName ) const {

	if (!model_)
		return General::DIRECTION_INVALID;

	return model_->getPortDirection(portName);
}

void Component::removeChannel( const QString& channelName ) {

	for (int i = 0; i < channels_.size(); ++i) {
		if (channels_.at(i)->getName() == channelName) {
			channels_.removeAt(i);
		}
	}
}

void Component::updateFiles( const Component& other,
							const QString& sourcePath, 
							const QString& targetPath ) {
				
	Q_ASSERT_X(!sourcePath.isEmpty(), "Component::updateFiles",
		"Empty source path given as parameter");
	Q_ASSERT_X(!targetPath.isEmpty(), "Component::updateFiles",
		"Empty targetPath given as parameter");

	// can't operate on the same component
	if (&other == this)
		return;

	// get the files from the other component
	QStringList files = other.getFiles();
	
	// take each file
	foreach (QString file, files) {
	
		// get the absolute path to the file
		QDir source(sourcePath);
		QString absoluteSource = source.absoluteFilePath(file);

		// if file is located under the source directory
		if (!file.contains(QString("../"))) {

			QDir target(targetPath);
			QString absoluteTarget = target.absoluteFilePath(file);
			
			QFileInfo targetInfo(absoluteTarget);

			target.mkpath(targetInfo.absolutePath());
			QFile::copy(absoluteSource, absoluteTarget);
			 
		}
		// if file is higher in directory hierarchy than the source directory
		else {
			// update the file name
			changeFileName(file, absoluteSource);
		}
	}
}

void Component::changeFileName( const QString& from, const QString& to ) {

	foreach (QSharedPointer<FileSet> fileSet, fileSets_) {

		if (fileSet->contains(from)) {
			fileSet->changeFileName(from, to);
			return;
		}
	}
}

File* Component::getQuartusPinmap() const {

	foreach (QSharedPointer<FileSet> fileSet, fileSets_) {
		File* file = fileSet->getQuartusPinMap();
		if (file)
			return file;
	}

	// no pin map was found
	return 0;
}

void Component::setVlnv( const VLNV& vlnv ) {
	LibraryComponent::setVlnv(vlnv);
	LibraryComponent::vlnv_->setType(VLNV::COMPONENT);
}

KactusAttribute::ProductHierarchy Component::getComponentHierarchy() const {

	KactusAttribute::ProductHierarchy hierarchy = KactusAttribute::KTS_GLOBAL;

	// if attribute is not found
	if (!kactus2Attributes_.contains(QString("kts_productHier")))
		return hierarchy;
	else
		KactusAttribute::stringToValue(kactus2Attributes_.value(QString("kts_productHier")),
		hierarchy);

	return hierarchy;
}

KactusAttribute::Firmness Component::getComponentFirmness() const {
	KactusAttribute::Firmness firmness = KactusAttribute::KTS_MUTABLE;

	if (!kactus2Attributes_.contains(QString("kts_firmness")))
		return firmness;
	else
		KactusAttribute::stringToValue(kactus2Attributes_.value(QString("kts_firmness")),
		firmness);
	return firmness;
}

KactusAttribute::Implementation Component::getComponentImplementation() const {
	KactusAttribute::Implementation implementation = KactusAttribute::KTS_HW;

	if (!kactus2Attributes_.contains(QString("kts_implementation")))
		return implementation;
	else
		KactusAttribute::stringToValue(kactus2Attributes_.value(QString("kts_implementation")),
		implementation);
	return implementation;
}

//-----------------------------------------------------------------------------
// Function: getComponentSWType()
//-----------------------------------------------------------------------------
KactusAttribute::SWType Component::getComponentSWType() const
{
	KactusAttribute::SWType type = KactusAttribute::KTS_SW_MAPPING;

	if (kactus2Attributes_.contains(QString("kts_sw_type"))) {
		KactusAttribute::stringToValue(kactus2Attributes_.value(QString("kts_sw_type")), type);
	}

	return type;
}

//-----------------------------------------------------------------------------
// Function: setComponentHierarchy()
//-----------------------------------------------------------------------------
void Component::setComponentHierarchy(KactusAttribute::ProductHierarchy prodHier) {
    kactus2Attributes_.insert("kts_productHier", KactusAttribute::valueToString(prodHier));
}

void Component::setComponentImplementation( KactusAttribute::Implementation implementation ) {
	kactus2Attributes_.insert("kts_implementation", KactusAttribute::valueToString(implementation));
}

//-----------------------------------------------------------------------------
// Function: setComponentFirmness()
//-----------------------------------------------------------------------------
void Component::setComponentFirmness(KactusAttribute::Firmness firmness) {
    kactus2Attributes_.insert("kts_firmness", KactusAttribute::valueToString(firmness));
}

//-----------------------------------------------------------------------------
// Function: setComponentSWType()
//-----------------------------------------------------------------------------
void Component::setComponentSWType(KactusAttribute::SWType type) {
    kactus2Attributes_.insert("kts_sw_type", KactusAttribute::valueToString(type));
	
	// also set component type as software
	kactus2Attributes_.insert("kts_implementation", "SW");
}

bool Component::hasModelParameters() const {
	if (!model_)
		return false;
	else
		return model_->hasModelParameters();
}

bool Component::hasParameters() const {
	return !parameters_.isEmpty();
}

bool Component::hasPorts() const {
	if (!model_)
		return false;
	else
		return model_->hasPorts();
}

bool Component::isPhysicalPort( const QString& portName ) const {
	if (!model_) {
		return false;
	}
	else {
		return model_->isPhysicalPort(portName);
	}
}

bool Component::hasInterfaces() const {
	return !busInterfaces_.isEmpty();
}

bool Component::hasFileSets() const {
	return !fileSets_.isEmpty();
}

const QMap<QString, QSharedPointer<Port> > Component::getPorts() const {
	if (model_)
		return model_->getPorts();
	else
		return QMap<QString, QSharedPointer<Port> >();
}

bool Component::hasFile( const QString& fileName ) const {
	foreach (QSharedPointer<FileSet> fileSet, fileSets_) {
		if (fileSet->contains(fileName)) {
			return true;
		}
	}
	return false;
}

int Component::getPortLeftBound( const QString& port ) const {
	if (!hasPort(port)) {
		return -1;
	}
	else {
		return getPort(port)->getLeftBound();
	}
}

int Component::getPortRightBound( const QString& port ) const {
	if (!hasPort(port)) {
		return -1;
	}
	else {
		return getPort(port)->getRightBound();
	}
}








VLNV Component::getHierRef(const QString viewName) const {

	// if no model is specified
	if (!model_)
		return VLNV();

	// ask the model for a hierarchyRef with a given name
	return model_->getHierRef(viewName);
}


QList<VLNV> Component::getHierRefs() const {
	if (!model_)
		return QList<VLNV>();

	return model_->getHierarchyRefs();
}


void Component::setHierRef(const VLNV& vlnv, const QString& viewName /*= QString()*/ ) {
	if (!model_)
		return;
	model_->setHierRef(vlnv, viewName);
}

View* Component::findView(const QString name) const {
	return model_->findView(name);
}

void Component::addView(View* newView) {
	model_->addView(newView);
}

const QList<QSharedPointer<View> > Component::getViews() const {

	// if model exists then ask it for views
	if (model_)
		return model_->getViews();

	// if not then return empty list
	else
		return QList<QSharedPointer<View> >();
}

View* Component::createView() {

	// if model does not exist then create one
	if (!model_)
		model_ = QSharedPointer<Model>(new Model());

	return model_->createView();
}

QStringList Component::getViewNames() const {
	if (model_)
		return model_->getViewNames();
	else
		return QStringList();
}

int Component::viewCount() const {
	if (model_)
		return model_->viewCount();

	// if no model is defined then no views are either
	else
		return 0;
}

QStringList Component::getHierViews() const {

	if (model_)
		return model_->getHierViews();
	else
		return QStringList();
}

QString Component::getEntityName( const QString& viewName ) const {
	if (model_) {
		QString entityName = model_->getEntityName(viewName);

		// if view contained an entity name
		if (!entityName.isEmpty()) {
			return entityName;
		}
	
	}
	// if there are no views then the VLNV name is used
	return vlnv_->getName();
}

QString Component::getArchitectureName( const QString& viewName ) const {

	if (model_) {
		return model_->getArchitectureName(viewName);
	}
	// if there is no views
	return QString();
}

void Component::removeView( const QString& viewName ) {
	if (model_)
		model_->removeView(viewName);
}

bool Component::hasViews() const {
	if (!model_)
		return false;
	else
		return model_->hasViews();
}

bool Component::hasView( const QString& viewName ) const {
	if (!model_) {
		return false;
	}
	return model_->findView(viewName);
}

QString Component::getViewDescription( const QString& viewName ) const {
	View* view = findView(viewName);
	if (!view)
		return QString();

	return view->getDescription();
}

QMap<QString, QString> Component::getPortDefaultValues() const {
	if (model_) {
		return model_->getPortDefaultValues();
	}
	else {
		return QMap<QString, QString>();
	}
}

QStringList Component::getPortTypeDefinitions() const {
	if (model_) {
		return model_->getPortTypeDefinitions();
	}
	else {
		return QStringList();
	}
}
