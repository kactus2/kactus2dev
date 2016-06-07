//-----------------------------------------------------------------------------
// File: fileselector.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 11.6.2013
//
// Description:
// Used to select one file from the component's file sets.
//-----------------------------------------------------------------------------

#include "fileselector.h"

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/FileSet.h>

#include <QFileInfo>

//-----------------------------------------------------------------------------
// Function: fileselector::FileSelector()
//-----------------------------------------------------------------------------
FileSelector::FileSelector( QSharedPointer<Component> component, QWidget *parent ):
QComboBox(parent),
component_(component),
filters_()
{
	Q_ASSERT(component_);

	setEditable(false);

	connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(onIndexChange(int)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: fileselector::~FileSelector()
//-----------------------------------------------------------------------------
FileSelector::~FileSelector()
{

}

//-----------------------------------------------------------------------------
// Function: FileSelector::addFilter()
//-----------------------------------------------------------------------------
void FileSelector::addFilter( const QString& suffix )
{
    if (suffix.isEmpty())
    {
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

//-----------------------------------------------------------------------------
// Function: fileselector::refresh()
//-----------------------------------------------------------------------------
void FileSelector::refresh()
{
	disconnect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(onIndexChange(int)));

	// the file which is currently selected
	QString previouslySelected = currentText();

	// remove the previous items
	clear();

	// ask component for it's files
    QStringList allFiles = getFileNames(component_);

	// all files that matched filters
	QStringList results;

	// filter the files according to given filters
	foreach (QString filePath, allFiles)
    {
		if (filePath.isEmpty())
        {
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
            if (filters_.contains(fileInfo.suffix()))
            {
                results.append(filePath);
            }
        }
	}
	// add also an empty item
	addItem("");
	addItems(results);

	int index = findText(previouslySelected);

	// if the item is not found
	if (index < 0)
    {
		// select the empty file
		index = 0;
	}

	setCurrentIndex(index);

	connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(onIndexChange(int)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: fileselector::selectFile()
//-----------------------------------------------------------------------------
void FileSelector::selectFile( const QString& filePath )
{
	disconnect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(onIndexChange(int)));

	int index = findText(filePath);

	// if the item is not found
	if (index < 0)
    {
		// select the empty file
		index = 0;
	}

	setCurrentIndex(index);

	connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(onIndexChange(int)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: fileselector::onIndexChange()
//-----------------------------------------------------------------------------
void FileSelector::onIndexChange( int newIndex )
{
	// Find the text for the index.
	QString filePath = itemText(newIndex);

	// Find corresponding IP-XACT objects.
	QSharedPointer<FileSet> fileSet = fileSets_[filePath];

	// Emit signal with complete data.
	emit fileSelected(filePath,fileSet);
}

//-----------------------------------------------------------------------------
// Function: fileselector::getFileNames()
//-----------------------------------------------------------------------------
QStringList FileSelector::getFileNames(QSharedPointer<Component> component)
{
    QStringList fileNames;

    foreach (QSharedPointer<FileSet> fileSet, *component->getFileSets())
	{
		foreach (QSharedPointer<File> file, *fileSet->getFiles())
		{
			fileSets_[file->name()] = fileSet;
			fileNames.append(file->name());
		}
    }

    return fileNames;
}