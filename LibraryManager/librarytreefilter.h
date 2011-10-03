/* 
 *
 *  Created on: 20.12.2010
 *      Author: Antti Kamppi
 */

#ifndef LIBRARYTREEFILTER_H
#define LIBRARYTREEFILTER_H

#include <common/utils.h>
#include <models/component.h>

#include <QSortFilterProxyModel>
#include <QString>
#include <QRegExpValidator>
#include <QSharedPointer>

class VLNVDialer;
class LibraryInterface;

/*! \brief The class that does the filtering and sorting of items for the tree view.
 *
 */
class LibraryTreeFilter : public QSortFilterProxyModel {

	Q_OBJECT

public:

    /*! \brief The constructor
     *
     * \param handler Pointer to the instance that manages the library.
     * \param dialer Pointer to the VLNVDialer that provides search options.
     * \param sourceModel Pointer to the model used as source for this proxy.
     * \param parent Pointer to the owner of this widget.
     *
    */
    LibraryTreeFilter(LibraryInterface* handler,
		VLNVDialer* dialer, 
		QAbstractItemModel* sourceModel,
		QObject *parent = 0);

	//! \brief The destructor
    virtual ~LibraryTreeFilter();

public slots:

	//! \brief Handler for when the text in vendor combobox is changed.
	void onVendorChanged(const QString& vendorText);

	//! \brief Handler for when the text in library combobox is changed.
	void onLibraryChanged(const QString& libraryText);

	//! \brief Handler for when the text in name combobox is changed.
	void onNameChanged(const QString& nameText);

	//! \brief Handler for when the text in version combobox is changed.
	void onVersionChanged(const QString& versionText);

	//! \brief Handler for when firmness options change.
	void onFirmnessChanged(const Utils::FirmnessOptions& options);

	//! \brief Handler for when implementation options change.
	void onImplementationChanged(const Utils::ImplementationOptions& options);

	//! \brief Handler for when type options change.
	void onTypeChanged(const Utils::TypeOptions& options);

	//! \brief Handler for when hierarchy options change.
	void onHierarchyChanged(const Utils::HierarchyOptions& options);

protected:

	/*! \brief Checks if the row should be shown in the view or not.
	 *
	 * \param sourceRow Specifies the row number of the item
	 * \param sourceParent Specifies the parent of the item.
	 *
	 * \return True if item is shown.
	*/
	virtual bool filterAcceptsRow(int sourceRow,
			const QModelIndex& sourceParent) const;

private:

	//! \brief No copying
	LibraryTreeFilter(const LibraryTreeFilter& other);

	//! \brief No assignment
	LibraryTreeFilter& operator=(const LibraryTreeFilter& other);

	//! \brief Check the firmness matching.
	bool checkFirmness(QSharedPointer<Component> component) const;

	//! \brief Check the implementation matching.
	bool checkImplementation(QSharedPointer<Component> component) const;

	//! \brief Check the hierarchy matching.
	bool checkHierarchy(QSharedPointer<Component> component) const;

	/*! \brief Check the list of vlnvs if they match the search rules
	 *
	 * \param list Contains the vlnvs to check. 
	 *
	 * \return bool True if at least one match is found.
	*/
	bool checkVLNVs(const QList<VLNV*>& list) const;
	 
	//! \brief Pointer to the instance that manages the library.
	LibraryInterface* handler_;

	//! \brief Pointer to the dialer that provides search options.
	VLNVDialer* dialer_;

	//! \brief Contains filters for re-usability
	Utils::FirmnessOptions firmness_;

	//! \brief Contains filters for firmness
	Utils::ImplementationOptions implementation_;

	//! \brief Contains filters for type
	Utils::TypeOptions type_;

	//! \brief Contains filters for hierarchy
	Utils::HierarchyOptions hierarchy_;

	//! \brief The validator that checks vendor rules
	QRegExpValidator vendorValidator_;

	//! \brief The validator that checks library rules
	QRegExpValidator libraryValidator_;

	//! \brief The validator that checks name rules
	QRegExpValidator nameValidator_;

	//! \brief The validator that checks version rules
	QRegExpValidator versionValidator_;

	//! \brief Contains filters for vendor
	QString vendor_;

	//! \brief Contains filters for library
	QString library_;

	//! \brief Contains filters for name
	QString name_;

	//! \brief Contains filters for version
	QString version_;
};

#endif // LIBRARYTREEFILTER_H
