//-----------------------------------------------------------------------------
// File: LibrarySettingsDialog.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti Maatta
// Date: 17.08.2012
//
// Description:
// Dialog for configuring library locations.
//-----------------------------------------------------------------------------

#ifndef LIBRARYSETTINGSDIALOG_H
#define LIBRARYSETTINGSDIALOG_H

#include <QDialog>
#include <QFileIconProvider>
#include <QPushButton>
#include <QSettings>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QIcon>

//-----------------------------------------------------------------------------
//! The dialog to display library locations and select active and default library paths.
//-----------------------------------------------------------------------------
class LibrarySettingsDialog : public QDialog
{
    Q_OBJECT

public:

    /*!
     *  Constructor.
     *
     *      @param [in/out] settings  The settings store.
     *      @param [in]     parent    The parent widget.
     */
    LibrarySettingsDialog(QSettings& settings, QWidget* parent = 0);

    /*!
     *  Destructor.
     */
    ~LibrarySettingsDialog();

    /*!
     *  Validates the contents of the page thoroughly.
     *
     *      @return True, if the contents are valid. False, if they are invalid.
     */
    bool validate();

    /*!
     *  Applies the changes that were done in the page.
     */
    void apply();

public slots:

    /*!
     *  Adds a new directory to the library locations.
     */
    void addLocation();

    /*!
     *  Removes the currently selected library location.
     */
    void removeLocation();

	/*!
     *  Called when a location is selected in the locations list.
	 *
	 *      @param [in] cur     Pointer to the selected item.
	 *      @param [in] prev    Pointer to the previously selected item.
     */
	void onSelectLocation(QTableWidgetItem* cur, QTableWidgetItem* prev);

signals:
	
    /*!
     *  Emitted when the library settings has changed and a scan should be performed.
     */
	void scanLibrary();

private slots:

	/*!
     *  Handler for item clicks on the lib locations list.
	 *
	 *      @param [in] item    Pointer to the clicked item.
     */
	void onItemClicked(QTableWidgetItem* item);

    /*!
     *  Called when the user presses OK.
     */
    virtual void accept();

private:

    // Disable copying.
    LibrarySettingsDialog(LibrarySettingsDialog const& rhs);
    LibrarySettingsDialog& operator=(LibrarySettingsDialog const& rhs);

    /*!
     *  Loads the settings from the settings store.
     */
    void loadSettings();

    /*!
     *  Creates a row in the library path table for the given directory.
     *
     *      @param [in] directory   The directory to add as library path.
     *      @param [in] isActive    Is the directory is set active or not.
     *      @param [in] isDefault   Is the directory is set default or not.
     */
    void createRowForDirectory(QString const& directory, bool isActive, bool isDefault);

    /*!
     *  Change the default path for the library.
     *
     *      @param [in] item    Table item for the new default path.
     */
    void changeDefaultPath(QTableWidgetItem* item);

    /*!
     *  Change the active status of the selected library path.
     *
     *      @param [in] item    The selected table item.
     */
    void changeActivePathStatus(QTableWidgetItem* item);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

     //! The settings store.
	 QSettings& settings_;

     //! Icon provider for directory folders.
     QFileIconProvider iconProvider_;

	 //! Library locations table
	 QTableWidget* libLocationsTable_;

	 //! Add location button.
	 QPushButton* addLocationButton_;

	 //! Remove location button.
	 QPushButton* removeLocationButton_;

	 //! OK button.
	 QPushButton* okButton_;

	 //! Holds the info on if the user has changed the library or not.
	 bool changed_;

     //! Check icon for the active and default library paths.
     QIcon checkMarkIcon_;
};

//-----------------------------------------------------------------------------

#endif // LIBRARYSETTINGSDIALOG_H
