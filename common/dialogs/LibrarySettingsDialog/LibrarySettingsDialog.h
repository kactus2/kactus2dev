//-----------------------------------------------------------------------------
// File: LibrarySettingsDialog.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 17.08.2012
//
// Description:
// Dialog for configuring library locations.
//-----------------------------------------------------------------------------

#ifndef LIBRARYSETTINGSDIALOG_H
#define LIBRARYSETTINGSDIALOG_H

#include <QDialog>
#include <QSettings>
#include <QPushButton>

#include <QTableWidget>
#include <QTableWidgetItem>

#include <QListWidgetItem>

//-----------------------------------------------------------------------------
//! Library locations dialog.
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
    //! Adds a new directory to the library locations.
    void addLocation();

    //! Removes the currently selected library location.
    void removeLocation();

    //! Called when a location is selected in the locations list.
    void onSelectLocation(QListWidgetItem* cur, QListWidgetItem* prev);

	/*! \brief Called when a location is selected in the locations list.
	 *
	 * Method: 		onSelectLocation
	 * Full name:	LibrarySettingsDialog::onSelectLocation
	 * Access:		public 
	 *
	 * \param cur Pointer to the selected item.
	 * \param prev Pointer to the previously selected item.
	 *
	*/
	void onSelectLocation(QTableWidgetItem* cur, QTableWidgetItem* prev);

signals:
	//! \brief Emitted when the library settings has changed and a scan should be performed.
	void scanLibrary();

private slots:

	/*! \brief Handler for item clicks on the lib locations list.
	 *
	 * \param item Pointer to the clicked item.
	 *
	*/
	void onItemClicked(QListWidgetItem* item);

	/*! \brief Handler for item clicks on the lib locations table.
	 *
	 * Method: 		onItemClicked
	 * Full name:	LibrarySettingsDialog::onItemClicked
	 * Access:		private 
	 *
	 * \param item Pointer to the clicked item.
	 *
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

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The settings store.
    QSettings& settings_;

    //! The library location list.
    //QListWidget* libLocationsList_;

	//! \brief The library locations table.
	QTableWidget* libLocationsTable_;

    //! Add location button.
    QPushButton* addLocationButton_;

    //! Remove location button.
    QPushButton* removeLocationButton_;

    //! OK button.
    QPushButton* okButton_;

	//! \brief Holds the info on if the user has changed the library or not.
	bool changed_;
};

//-----------------------------------------------------------------------------

#endif // LIBRARYSETTINGSDIALOG_H
