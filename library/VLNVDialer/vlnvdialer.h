//-----------------------------------------------------------------------------
// File: vlnvdialer.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 11.07.2011
//
// Description:
// VLNVDialer is used to set options that specify what objects to display in library.
//-----------------------------------------------------------------------------

#ifndef VLNVDIALER_H
#define VLNVDIALER_H

#include "dialerwidget.h"
#include "filterwidget.h"

#include <IPXACTmodels/common/TagData.h>

#include <KactusAPI/include/utils.h>

#include <QWidget>
#include <QGroupBox>
#include <QPushButton>
#include <QString>
#include <QSettings>

class LibraryItem;
class TagContainer;

//-----------------------------------------------------------------------------
//! VLNVDialer is used to set options that specify what objects to display in library.
//-----------------------------------------------------------------------------
class VLNVDialer : public QWidget
{
	Q_OBJECT

public:

	/*! The constructor
	 *
	 *      @param [in] parent The owner of this widget.
	 *
	*/
	VLNVDialer(QWidget *parent);
	
	//! The destructor
	virtual ~VLNVDialer() = default;

    //! No copying
    VLNVDialer(VLNVDialer const& other) = delete;

    //! No assignment
    VLNVDialer& operator=(VLNVDialer const& other) = delete;

	/*! Set the root item used to create the suggestions for dialer.
	 *
	 *      @param [in] rootItem    The root item of the library.
	 *
	*/
	void setRootItem(LibraryItem const* rootItem);

	/*!
	 *  Set filter settings for the library.
	 *
	 *      @param [in] settings    Settings containing the library filters.
	 */
	void loadFilterSettings(QSettings& settings);

	/*!
	 *  Save settings for dock widget filters.
	 *
	 *      @param [in] settings    The settings.
	 */
	void saveFilterSettings(QSettings& settings) const;

signals:

	//! Emitted when the text in vendor combobox is changed.
	void vendorChanged(QString const& vendorText);

	//! Emitted when the text in library combobox is changed.
	void libraryChanged(QString const& libraryText);

	//! Emitted when the text in name combobox is changed.
	void nameChanged(QString const& nameText);

	//! Emitted when the text in version combobox is changed.
	void versionChanged(QString const& versionText);

	//! Emitted when re-usability options change.
	void firmnessChanged(Utils::FirmnessOptions const& options);

	//! Emitted when firmness options change.
	void implementationChanged(Utils::ImplementationOptions const& options);

	//! Emitted when type options change.
	void typeChanged(Utils::TypeOptions const& options);

	//! Emitted when hierarchy options change.
	void hierarchyChanged(Utils::HierarchyOptions const& options);

    //! Emitted when all options change.
    void filtersChanged(Utils::FilterOptions const&);

    /*!
     *  Informs of tag filter change.
     *
     *      @param [in] tagFilters  The new visible tags.
     */
    void tagFiltersChanged(QVector<TagData> tagFilters);

public slots:

	//! Refresh the list of vendors on the library
	void refreshLibrary();

private slots:

    /*!
     *  Handler for changing visible tags.
     */
    void onHandleTagFilterChange();

	//! Handler for hide/show button clicks.
	void onHideShowClick();

	//! Handler for select/clear button clicks.
	void onSelectAll();

private:

	//! Setup the widget layout.
    void setupLayout();

    //-----------------------------------------------------------------------------
	// Data.
	//-----------------------------------------------------------------------------
		
    //! Button to hide/show filters.
    QPushButton hideButton_;

    //! Button to hide/show filters.
    QPushButton selectAllButton_;

	//! Contains the items to set filters for search.
	FilterWidget filters_;

	//! Contains the items to set the rules for vlnv search
	DialerWidget dialer_;

	//! Group box for tag filters.
    QGroupBox tagGroup_;

    //! Tag filter.
    TagContainer* tagFilter_;

	//! Contains the value to know if filters are currently hidden or visible.
	bool hideFilters_ = false;

	//! Contains the value to know if all filters are selected or cleared.
	bool allSelected_ = false;
};

#endif // VLNVDIALER_H
