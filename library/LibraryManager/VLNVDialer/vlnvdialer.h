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

#include <common/utils.h>

#include <QWidget>
#include <QPushButton>
#include <QString>

class LibraryItem;

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
	virtual ~VLNVDialer();

	/*! Set the root item used to create the suggestions for dialer.
	 *
	 *      @param [in] rootItem    The root item of the library.
	 *
	*/
	void setRootItem(LibraryItem const* rootItem);

    void setFilters(FilterWidget::FilterOptions options);

    FilterWidget::FilterOptions getFilters() const;

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

public slots:

	//! Refresh the list of vendors on the library
	void refreshLibrary();

private slots:

	//! Handler for hide/show button clicks.
	void onHideShowClick();

private:
	//! No copying
	VLNVDialer(VLNVDialer const& other);

	//! No assignment
	VLNVDialer& operator=(VLNVDialer const& other);

	//! Contains the items to set filters for search.
	FilterWidget filters_;

	//! Contains the items to set the rules for vlnv search
	DialerWidget dialer_;

	//! Button to hide/show filters.
	QPushButton hideButton_;

	//! Contains the value to know if filters are currently hidden or visible
	bool hidden_;
};

#endif // VLNVDIALER_H
