/* 
 *  	Created on: 26.1.2012
 *      Author: Antti Kamppi
 * 		filename: librarypathselector.cpp
 *		Project: Kactus 2
 */

#include "librarypathselector.h"

#include <QStringList>
#include <QVariant>

LibraryPathSelector::LibraryPathSelector(QWidget *parent):
QComboBox(parent),
settings_(this),
validator_(new LibraryPathValidator(this)),
libraryLocations_(settings_.value("Library/Locations", QStringList()).toStringList()),
pathEditor_(NULL) {

	addItems(libraryLocations_);

	QString defaultPath = settings_.value("Library/DefaultLocation", QString()).toString();

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

	setToolTip(tr("Select one of the library locations you have listed in the settings."));
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
