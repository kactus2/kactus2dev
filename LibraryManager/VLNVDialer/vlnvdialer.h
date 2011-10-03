/* 
 *  	Created on: 11.7.2011
 *      Author: Antti Kamppi
 * 		filename: vlnvdialer.h
 *		Project: Kactus 2
 */

#ifndef VLNVDIALER_H
#define VLNVDIALER_H

#include "dialerwidget.h"
#include "filterwidget.h"

#include <common/utils.h>

#include <QWidget>
#include <QPushButton>
#include <QString>

class LibraryItem;

/*! \brief VLNVDialer is used to set options that specify what objects to display in library.
 *
 */
class VLNVDialer : public QWidget {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this widget.
	 *
	*/
	VLNVDialer(QWidget *parent);
	
	//! \brief The destructor
	virtual ~VLNVDialer();

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

	//! \brief Emitted when re-usability options change.
	void firmnessChanged(const Utils::FirmnessOptions& options);

	//! \brief Emitted when firmness options change.
	void implementationChanged(const Utils::ImplementationOptions& options);

	//! \brief Emitted when type options change.
	void typeChanged(const Utils::TypeOptions& options);

	//! \brief Emitted when hierarchy options change.
	void hierarchyChanged(const Utils::HierarchyOptions& options);

public slots:

	//! \brief Refresh the list of vendors on the library
	void refreshLibrary();

private slots:

	//! \brief Handler for hide/show button clicks.
	void onHideShowClick();

private:
	//! \brief No copying
	VLNVDialer(const VLNVDialer& other);

	//! \brief No assignment
	VLNVDialer& operator=(const VLNVDialer& other);

	//! \brief Contains the items to set filters for search.
	FilterWidget filters_;

	//! \brief Contains the items to set the rules for vlnv search
	DialerWidget dialer_;

	//! \brief Button to hide/show filters.
	QPushButton hideButton_;

	//! \brief Contains the value to know if filters are currently hidden or visible
	bool hidden_;
};

#endif // VLNVDIALER_H
