//-----------------------------------------------------------------------------
// File: dialerwidget.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 11.07.2011
//
// Description:
// Contains the combo boxes to set search settings for vlnv-fields.
//-----------------------------------------------------------------------------

#ifndef DIALERWIDGET_H
#define DIALERWIDGET_H

#include <QWidget>
#include <QString>
#include <QComboBox>
#include <QList>

class LibraryItem;

//-----------------------------------------------------------------------------
//! Contains the combo boxes to set search settings for vlnv-fields.
//-----------------------------------------------------------------------------
class DialerWidget : public QWidget
{
	Q_OBJECT

public:

	/*! The constructor
	 *
	 *      @param [in] parent  The owner of this widget.
	 *
	*/
	DialerWidget(QWidget *parent);
	
	//! The destructor
	virtual ~DialerWidget();

	/*! Set the root item used to create the suggestions for dialer.
	 *
	 *      @param [in] rootItem    The root item of the library.
	 *
	*/
	void setRootItem(const LibraryItem* rootItem);

signals:

	//! Emitted when the text in vendor combobox is changed.
	void vendorChanged(QString const& vendorText);

	//! Emitted when the text in library combobox is changed.
	void libraryChanged(QString const& libraryText);

	//! Emitted when the text in name combobox is changed.
	void nameChanged(QString const& nameText);

	//! Emitted when the text in version combobox is changed.
	void versionChanged(QString const& versionText);

public slots:

	//! Refresh the vendor list
	void refreshVendors();

private slots:

	//! Handler when the text in vendor combobox is changed.
	void onVendorChanged(QString const& vendorText);

	//! Handler when the text in library combobox is changed.
	void onLibraryChanged(QString const& libraryText);

	//! Handler when the text in name combobox is changed.
	void onNameChanged(QString const& nameText);

	//! Handler when the text in version combobox is changed.
	void onVersionChanged(QString const& versionText);

private:
	//! No copying
	DialerWidget(const DialerWidget& other);
	DialerWidget& operator=(const DialerWidget& other);

    void setupLayout();

	//! Connect signals from combo boxes.
	void connectBoxes();

	//! Box to set the vendor rules for search.
	QComboBox vendorBox_;

	//! Box to set the library rules for search.
	QComboBox libraryBox_;

	//! Box to set the name rules for search.
	QComboBox nameBox_;

	//! Box to set the version rules for search.
	QComboBox versionBox_;

	//! The root item of the library.
	const LibraryItem* root_;

	//! The library items that represent vendors.
	QList<LibraryItem*> vendors_;

	//! The library items that represent libraries.
	QList<LibraryItem*> libraries_;

	//! The library items that represent names.
	QList<LibraryItem*> names_;

	//! The library items that represent versions.
	QList<LibraryItem*> versions_;
};

#endif // DIALERWIDGET_H
