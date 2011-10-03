/* 
 *  	Created on: 11.7.2011
 *      Author: Antti Kamppi
 * 		filename: dialerwidget.h
 *		Project: Kactus 2
 */

#ifndef DIALERWIDGET_H
#define DIALERWIDGET_H

#include <QWidget>
#include <QString>
#include <QComboBox>
#include <QList>

class LibraryItem;

/*! \brief Contains the combo boxes to set search settings for vlnv-fields.
 *
 */
class DialerWidget : public QWidget {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this widget.
	 *
	*/
	DialerWidget(QWidget *parent);
	
	//! \brief The destructor
	virtual ~DialerWidget();

	/*! \brief Set the root item used to create the suggestions for dialer.
	 *
	 * \param rootItem Pointer to the root item of the library.
	 *
	*/
	void setRootItem(const LibraryItem* rootItem);

signals:

	//! \brief Emitted when the text in vendor combobox is changed.
	void vendorChanged(const QString& vendorText);

	//! \brief Emitted when the text in library combobox is changed.
	void libraryChanged(const QString& libraryText);

	//! \brief Emitted when the text in name combobox is changed.
	void nameChanged(const QString& nameText);

	//! \brief Emitted when the text in version combobox is changed.
	void versionChanged(const QString& versionText);

public slots:

	//! \brief Refresh the vendor list
	void refreshVendors();

private slots:

	//! \brief Handler when the text in vendor combobox is changed.
	void onVendorChanged(const QString& vendorText);

	//! \brief Handler when the text in library combobox is changed.
	void onLibraryChanged(const QString& libraryText);

	//! \brief Handler when the text in name combobox is changed.
	void onNameChanged(const QString& nameText);

	//! \brief Handler when the text in version combobox is changed.
	void onVersionChanged(const QString& versionText);

private:
	//! \brief No copying
	DialerWidget(const DialerWidget& other);

	//! \brief No assignment
	DialerWidget& operator=(const DialerWidget& other);

	//! \brief Disconnect the signals from combo boxes.
	void disconnectBoxes();

	//! \brief Connect signals from combo boxes.
	void connectBoxes();

	//! \brief Box to set the vendor rules for search.
	QComboBox vendorBox_;

	//! \brief Box to set the library rules for search.
	QComboBox libraryBox_;

	//! \brief Box to set the name rules for search.
	QComboBox nameBox_;

	//! \brief Box to set the version rules for search.
	QComboBox versionBox_;

	//! \brief Pointer to the root item of the library.
	const LibraryItem* root_;

	//! \brief Pointers to library items that represent vendors.
	QList<LibraryItem*> vendors_;

	//! \brief Pointers to library items that represent libraries.
	QList<LibraryItem*> libraries_;

	//! \brief Pointers to library items that represent names.
	QList<LibraryItem*> names_;

	//! \brief Pointers to library items that represent versions.
	QList<LibraryItem*> versions_;
};

#endif // DIALERWIDGET_H
