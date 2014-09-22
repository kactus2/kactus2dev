/* 
 *	Created on:	11.6.2013
 *	Author:		Antti Kamppi
 *	File name:	fileselector.cpp
 *	Project:		Kactus 2
*/

#include "fileselector.h"

#include <QFileInfo>

FileSelector::FileSelector( QSharedPointer<Component> component, QWidget *parent ):
QComboBox(parent),
component_(component),
filters_() {
	Q_ASSERT(component_);

	setEditable(false);

	connect(this, SIGNAL(currentIndexChanged(int)),
		this, SLOT(onIndexChange(int)), Qt::UniqueConnection);
}

FileSelector::~FileSelector() {
}

//-----------------------------------------------------------------------------
// Function: FileSelector::addFilter()
//-----------------------------------------------------------------------------
void FileSelector::addFilter( const QString& suffix )
{
    if (suffix.isEmpty()) {
        return;
    }

    // add the new filter
    filters_.append(suffix);

    // and refresh the combo box to display changes
    refresh();
}

//-----------------------------------------------------------------------------
// Function: FileSelector::clearFilters()
//-----------------------------------------------------------------------------
void FileSelector::clearFilters()
{
    filters_.clear();
    refresh();
}

void FileSelector::refresh() {
	disconnect(this, SIGNAL(currentIndexChanged(int)),
		this, SLOT(onIndexChange(int)));

	// the file which is currently selected
	QString previouslySelected = currentText();

	// remove the previous items
	clear();

	// ask component for it's files
	QStringList allFiles = component_->getFiles();
	// all files that matched filters
	QStringList results;

	// filter the files according to given filters
	foreach (QString filePath, allFiles) {
		if (filePath.isEmpty()) {
			continue;
        }

        if ( filters_.empty() )
        {
            results.append(filePath);
        }
        else
        {
            // if the file suffix is found in the filters
            QFileInfo fileInfo(filePath);
            if (filters_.contains(fileInfo.suffix())) {
                results.append(filePath);
            }
        }
	}
	// add also an empty item
	addItem("");
	addItems(results);

	int index = findText(previouslySelected);

	// if the item is not found
	if (index < 0) {
		// select the empty file
		index = 0;
	}

	setCurrentIndex(index);

	connect(this, SIGNAL(currentIndexChanged(int)),
		this, SLOT(onIndexChange(int)), Qt::UniqueConnection);
}

void FileSelector::selectFile( const QString& filePath ) {
	disconnect(this, SIGNAL(currentIndexChanged(int)),
		this, SLOT(onIndexChange(int)));

	int index = findText(filePath);

	// if the item is not found
	if (index < 0) {
		// select the empty file
		index = 0;
	}

	setCurrentIndex(index);

	connect(this, SIGNAL(currentIndexChanged(int)),
		this, SLOT(onIndexChange(int)), Qt::UniqueConnection);
}

void FileSelector::onIndexChange( int newIndex ) {
	// find the text for the index
	QString filePath = itemText(newIndex);
	emit fileSelected(filePath);
}

