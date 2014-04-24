/* 
 *  	Created on: 1.7.2011
 *      Author: Antti Kamppi
 * 		filename: hierarchyfilter.h
 *		Project: Kactus 2
 */

#ifndef HIERARCHYFILTER_H
#define HIERARCHYFILTER_H

#include <common/utils.h>

#include <IPXACTmodels/component.h>

#include <QSharedPointer>
#include <QSortFilterProxyModel>
#include <QString>
#include <QRegExpValidator>

class VLNVDialer;

/*! \brief Used to filter and sort the contents of hierarchyModel
 *
 */
class HierarchyFilter : public QSortFilterProxyModel {
	Q_OBJECT

public:

	/*! \brief The constructor
	 * 
	 * \param dialer Pointer to the VLNVDialer that provides search options.
	 * \param parent Pointer to the owner of this filter.
	 *
	 * \return 
	*/
	HierarchyFilter(VLNVDialer* dialer, QObject *parent);
	
	//! \brief The destructor
	virtual ~HierarchyFilter();

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

	/*! \brief Validate the given row on a parent and check if item should be shown.
	 *
	 * \param source_row The row number of the child on the parent.
	 * \param source_parent Model index that identifies the parent object.
	 *
	 * \return bool True if object should be displayed.
	*/
	virtual bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const;

private:
	//! \brief No copying
	HierarchyFilter(const HierarchyFilter& other);

	//! \brief No assignment
	HierarchyFilter& operator=(const HierarchyFilter& other);

	//! \brief Check the firmness matching.
	bool checkFirmness(QSharedPointer<Component const> component) const;

	//! \brief Check the implementation matching.
	bool checkImplementation(QSharedPointer<Component const> component) const;

	//! \brief Check the hierarchy matching.
	bool checkHierarchy(QSharedPointer<Component const> component) const;

	/*! \brief Check the list of vlnvs if they match the search rules
	 *
	 * \param list Contains the vlnvs to check. 
	 *
	 * \return bool True if at least one match is found.
	*/
	bool checkVLNVs(const QList<VLNV>& list) const;

	//! \brief Pointer to the vlnv dialer that provides search settings.
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

#endif // HIERARCHYFILTER_H
