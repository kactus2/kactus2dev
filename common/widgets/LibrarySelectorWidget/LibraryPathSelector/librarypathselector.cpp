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
// Function: LibraryPathSelector::LibraryPathSelector()
//-----------------------------------------------------------------------------
LibraryPathSelector::LibraryPathSelector(QWidget *parent):
QComboBox(parent),
settings_(this),
validator_(new LibraryPathValidator(this)),
libraryLocations_(),
pathEditor_(new LibraryPathEditor(this))
{
	refresh();

	QString defaultPath = settings_.value("Library/DefaultLocation", QString()).toString();
	if (!QFileInfo(defaultPath).isAbsolute())
    {
		defaultPath = QFileInfo(defaultPath).absoluteFilePath();
	}

	// if the default directory is found then select it
	int index = findText(defaultPath);
	if (index >= 0)
    {
		setCurrentIndex(index);
	}

	setEditable(true);

	setLineEdit(pathEditor_);
	pathEditor_->setUnmodifiablePath(defaultPath);

	setValidator(validator_);
	validator_->setUnmodifiablePath(defaultPath);

	setToolTip(tr("Select one of the active library locations you have listed in the settings."));

    connect(pathEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()));

    connect(this, SIGNAL(currentIndexChanged(int)),	this, SLOT(onIndexChanged()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: LibraryPathSelector::~LibraryPathSelector()
//-----------------------------------------------------------------------------
LibraryPathSelector::~LibraryPathSelector()
{

}

//-----------------------------------------------------------------------------
// Function: LibraryPathSelector::isValid()
//-----------------------------------------------------------------------------
bool LibraryPathSelector::isValid()
{
    Q_ASSERT(pathEditor_);
    return !currentText().isEmpty() && hasIndexFor(currentText());
}

//-----------------------------------------------------------------------------
// Function: LibraryPathSelector::currentLocation()
//-----------------------------------------------------------------------------
QString LibraryPathSelector::currentLocation() const
{
	int index = currentIndex();
	return libraryLocations_.value(index, QString());
}

//-----------------------------------------------------------------------------
// Function: LibraryPathSelector::refresh()
//-----------------------------------------------------------------------------
void LibraryPathSelector::refresh()
{
    // Clear previous known library locations.
    libraryLocations_.clear();
    clear();

    // the active library locations
    QStringList activeLocations = settings_.value("Library/ActiveLocations", QStringList()).toStringList();
    
    // make sure all locations are absolute paths
    foreach (QString location, activeLocations)
    {
        if (!QFileInfo(location).isAbsolute())
        {
            QString fullLocation = QFileInfo(location).absoluteFilePath();
            libraryLocations_.append(fullLocation);
        }
        else
        {
            libraryLocations_.append(location);
        }
    }

    addItems(libraryLocations_);
}

//-----------------------------------------------------------------------------
// Function: LibraryPathSelector::getLibraryLocations()
//-----------------------------------------------------------------------------
QStringList LibraryPathSelector::getLibraryLocations()
{
    return libraryLocations_;
}

//-----------------------------------------------------------------------------
// Function: LibraryPathSelector::hasIndexFor()
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
// Function: LibraryPathSelector::setIndexFor()
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
// Function: LibraryPathSelector::onIndexChanged()
//-----------------------------------------------------------------------------
void LibraryPathSelector::onIndexChanged()
{
	const QString path = currentLocation();

	pathEditor_->setUnmodifiablePath(path);
	validator_->setUnmodifiablePath(path);
}
