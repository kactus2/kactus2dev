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

//-----------------------------------------------------------------------------
// Function: LibraryPathSelector()
//-----------------------------------------------------------------------------
LibraryPathSelector::LibraryPathSelector(QWidget *parent):
QComboBox(parent),
settings_(this),
validator_(new LibraryPathValidator(this)),
libraryLocations_(),
pathEditor_(NULL) {

	refresh();

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

//-----------------------------------------------------------------------------
// Function: ~LibraryPathSelector()
//-----------------------------------------------------------------------------
LibraryPathSelector::~LibraryPathSelector() {
}

//-----------------------------------------------------------------------------
// Function: isValid()
//-----------------------------------------------------------------------------
bool LibraryPathSelector::isValid()
{
    Q_ASSERT(pathEditor_);
    return !currentText().isEmpty() && hasIndexFor(currentText());
}

//-----------------------------------------------------------------------------
// Function: currentLocation()
//-----------------------------------------------------------------------------
QString LibraryPathSelector::currentLocation() const {
	int index = currentIndex();
	return libraryLocations_.value(index, QString());
}

//-----------------------------------------------------------------------------
// Function: refresh()
//-----------------------------------------------------------------------------
void LibraryPathSelector::refresh()
{
    // Clear previous known library locations.
    libraryLocations_.clear();
    clear();

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
}


//-----------------------------------------------------------------------------
// Function: getLibraryLocations()
//-----------------------------------------------------------------------------
QStringList LibraryPathSelector::getLibraryLocations()
{
    return libraryLocations_;
}


//-----------------------------------------------------------------------------
// Function: hasIndexFor()
//-----------------------------------------------------------------------------
bool LibraryPathSelector::hasIndexFor(QString const& path)
{
    foreach(QString libraryPath, libraryLocations_)
    {
        if (path.startsWith(libraryPath))
        {

            return true;
        }
    }
    return false;
}


//-----------------------------------------------------------------------------
// Function: setIndexFor()
//-----------------------------------------------------------------------------
bool LibraryPathSelector::setIndexFor(QString const& path)
{
    foreach(QString libraryPath, libraryLocations_)
    {
        if (path.startsWith(libraryPath))
        {
            setCurrentIndex(findText(libraryPath));
            setCurrentText(path);
            return true;
        }
    }
    return false;
}

//-----------------------------------------------------------------------------
// Function: onIndexChanged()
//-----------------------------------------------------------------------------
void LibraryPathSelector::onIndexChanged() {
	const QString path = currentLocation();
	pathEditor_->setUnmodifiablePath(path);
	validator_->setUnmodifiablePath(path);
}
