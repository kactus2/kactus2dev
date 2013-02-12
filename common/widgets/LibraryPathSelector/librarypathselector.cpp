/* 
 *  	Created on: 26.1.2012
 *      Author: Antti Kamppi
 * 		filename: librarypathselector.cpp
 *		Project: Kactus 2
 */

#include "librarypathselector.h"

#include <QStringList>
#include <QVariant>
#include <QFileInfo>

LibraryPathSelector::LibraryPathSelector(QWidget *parent):
QComboBox(parent),
settings_(this),
validator_(new LibraryPathValidator(this)),
libraryLocations_(),
pathEditor_(NULL) {

	// the active library locations
	QStringList activeLocations = settings_.value("Library/ActiveLocations", QStringList()).toStringList();
	// make sure all locations are absolute paths
	foreach (QString location, activeLocations) {

		// if the path is not absolute
		if (!QFileInfo(location).isAbsolute()) {
			QString fullLocation = QFileInfo(location).absoluteFilePath();
			libraryLocations_.append(fullLocation);
		}
		// if already absolute
		else {
			libraryLocations_.append(location);
		}
	}

	addItems(libraryLocations_);

	QString defaultPath = settings_.value("Library/DefaultLocation", QString()).toString();
	if (!QFileInfo(defaultPath).isAbsolute()) {
		defaultPath = QFileInfo(defaultPath).absoluteFilePath();
	}

	// if the default directory is found then select it
	int index = findText(defaultPath);
	if (index >= 0) {
		setCurrentIndex(index);
	}

	setEditable(true);

	pathEditor_ = new LibraryPathEditor(this);
	setLineEdit(pathEditor_);
	pathEditor_->setUnmodifiablePath(defaultPath);

	setValidator(validator_);
	validator_->setUnmodifiablePath(defaultPath);

	connect(this, SIGNAL(currentIndexChanged(int)),
		this, SLOT(onIndexChanged()), Qt::UniqueConnection);

	setToolTip(tr("Select one of the active library locations you have listed in the settings."));
}

LibraryPathSelector::~LibraryPathSelector() {
}

QString LibraryPathSelector::currentLocation() const {
	int index = currentIndex();
	return libraryLocations_.value(index, QString());
}

void LibraryPathSelector::onIndexChanged() {
	const QString path = currentLocation();
	pathEditor_->setUnmodifiablePath(path);
	validator_->setUnmodifiablePath(path);
}
