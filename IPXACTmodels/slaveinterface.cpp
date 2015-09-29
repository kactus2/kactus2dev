/* 
 *
 *  Created on: 31.8.2010
 *      Author: Antti Kamppi
 */

#include "slaveinterface.h"
#include "generaldeclarations.h"

#include <QString>
#include <QList>
#include <QSharedPointer>
#include <QObject>

SlaveInterface::Bridge::Bridge():
masterRef_(), isPresent_()
{
}

SlaveInterface::SlaveInterface():
memoryMapRef_(), 
bridges_(), 
fileSetRefGroup_() {
}

SlaveInterface::SlaveInterface( const SlaveInterface& other ):
memoryMapRef_(other.memoryMapRef_),
bridges_(),
fileSetRefGroup_() {

	foreach (QSharedPointer<Bridge> bridge, other.bridges_) {
		if (bridge) {
			QSharedPointer<Bridge> copy = QSharedPointer<Bridge>(
				new Bridge(*bridge));
			bridges_.append(copy);
		}
	}

	foreach (QSharedPointer<FileSetRefGroup> refGroup, other.fileSetRefGroup_) {
		if (refGroup) {
			QSharedPointer<FileSetRefGroup> copy = QSharedPointer<FileSetRefGroup>(
				new FileSetRefGroup(*refGroup.data()));
			fileSetRefGroup_.append(copy);
		}
	}
}

SlaveInterface& SlaveInterface::operator=( const SlaveInterface& other ) {
	if (this != &other) {
		memoryMapRef_ = other.memoryMapRef_;

		bridges_.clear();
		foreach (QSharedPointer<Bridge> bridge, other.bridges_) {
			if (bridge) {
				QSharedPointer<Bridge> copy = QSharedPointer<Bridge>(
					new Bridge(*bridge));
				bridges_.append(copy);
			}
		}

		fileSetRefGroup_.clear();
		foreach (QSharedPointer<FileSetRefGroup> refGroup, other.fileSetRefGroup_) {
			if (refGroup) {
				QSharedPointer<FileSetRefGroup> copy = QSharedPointer<FileSetRefGroup>(
					new FileSetRefGroup(*refGroup.data()));
				fileSetRefGroup_.append(copy);
			}
		}
	}
	return *this;
}

SlaveInterface::~SlaveInterface() {
	bridges_.clear();
	fileSetRefGroup_.clear();
}

bool SlaveInterface::isValid( QStringList& errorList, const QString& parentIdentifier ) const {
	bool valid = true;

	foreach (QSharedPointer<Bridge> bridge, bridges_) {
		if (bridge->masterRef_.isEmpty()) {
			errorList.append(QObject::tr("Bridge did not contain a master ref"
				" within %1").arg(parentIdentifier));
			valid = false;
		}
	}

	return valid;
}

bool SlaveInterface::isValid() const {
	foreach (QSharedPointer<Bridge> bridge, bridges_) {
		if (bridge->masterRef_.isEmpty()) {
			return false;
		}
	}

	return true;
}

const QList<QSharedPointer<SlaveInterface::Bridge> >& SlaveInterface::getBridges() const {
	return bridges_;
}

QList<QSharedPointer<SlaveInterface::Bridge> >& SlaveInterface::getBridges() {
	return bridges_;
}

void SlaveInterface::setMemoryMapRef(const QString& memoryMapRef) {
	memoryMapRef_ = memoryMapRef;
}

void SlaveInterface::setFileSetRefGroup(const
		QList<QSharedPointer<FileSetRefGroup> >& fileSetRefGroup) {
	// delete old fileSetRefs
	fileSetRefGroup_.clear();

	// save new fileSetRefs
	fileSetRefGroup_ = fileSetRefGroup;
}

void SlaveInterface::setBridges(const QList<QSharedPointer<Bridge> >& bridges) {
	// delete old bridge elements
	bridges_.clear();

	// save new elements
	bridges_ = bridges;
}

const QList<QSharedPointer<SlaveInterface::FileSetRefGroup> >&
SlaveInterface::getFileSetRefGroup() {
	return fileSetRefGroup_;
}

QString SlaveInterface::getMemoryMapRef() const {
	return memoryMapRef_;
}

bool SlaveInterface::hasBridge() const {
	return !bridges_.isEmpty();
}

QStringList SlaveInterface::getMasterReferences() const {
	QStringList names;
	foreach (QSharedPointer<SlaveInterface::Bridge> bridge, bridges_) {
		names.append(bridge->masterRef_);
	}
	return names;
}
