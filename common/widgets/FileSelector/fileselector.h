//-----------------------------------------------------------------------------
// File: fileselector.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 11.6.2013
//
// Description:
// Used to select one file from the component's file sets.
//-----------------------------------------------------------------------------

#ifndef FILESELECTOR_H
#define FILESELECTOR_H

#include <QComboBox>
#include <QSharedPointer>
#include <QStringList>

class Component;
class FileSet;

//-----------------------------------------------------------------------------
//! Used to select one file from the component's file sets.
//-----------------------------------------------------------------------------
class FileSelector : public QComboBox
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
	 *      @param [in] component   Pointer to the component whose files are selected.
	 *      @param [in] parent      Pointer to the owner of the combo box.
	 */
	FileSelector(QSharedPointer<Component> component, QWidget *parent);
	
	//! The destructor.
	virtual ~FileSelector();

	/*!
     *  Add a file suffix to be used as filter when selecting files to display.
	 *
	 *      @param [in] suffix  The file suffix to use as filter.
	 */
	void addFilter(const QString& suffix);

    /*!
     *  Clear the filters.
     */
    void clearFilters();

public slots:

	//! Refresh the items to be selected in the combo box.
	void refresh();

	/*!
     *  Set the specified file as selected.
	 *
	 *      @param [in] filePath    The file path of the file to select.
	 */
	void selectFile(const QString& filePath);

signals:

	//! Emitted when a file is selected.
	void fileSelected(const QString& filePath, QSharedPointer<FileSet> fileSet);

private slots:

	//! Handler for selected index changes.
	void onIndexChange(int newIndex);

private:
	
    /*!
     *  Get the file names.
     *
     *      @param [in] component   The component to search the files from.
     */
    QStringList getFileNames(QSharedPointer<Component> component);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! No copying.
	FileSelector(const FileSelector& other);

	//! No assignment.
	FileSelector& operator=(const FileSelector& other);

	//! Pointer to the component containing the files.
	QSharedPointer<Component> component_;

	//! Contains the file suffixes used to filter file types.
	QStringList filters_;

	//! Contains IP-XACT file sets indexed by the file name/path.
	QMap<QString,QSharedPointer<FileSet> > fileSets_;
};

#endif // FILESELECTOR_H
