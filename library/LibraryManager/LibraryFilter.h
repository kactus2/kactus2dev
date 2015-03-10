//-----------------------------------------------------------------------------
// File: LibraryFilter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 10.03.2015
//
// Description:
// Base class for library filters.
//-----------------------------------------------------------------------------

#ifndef LIBRARYFILTER_H
#define LIBRARYFILTER_H

#include <common/utils.h>
#include <IPXACTmodels/component.h>

#include <IPXACTmodels/kactusExtensions/KactusAttribute.h>

#include <QSortFilterProxyModel>
#include <QString>
#include <QRegExpValidator>
#include <QSharedPointer>

class VLNVDialer;
class LibraryInterface;

//-----------------------------------------------------------------------------
//! Base class for library filters.
//-----------------------------------------------------------------------------
class LibraryFilter : public QSortFilterProxyModel
{
	Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] dialer   The dialer for selecting the filters.
     *      @param [in] parent   The parent object.
     *
     *      @return <Description>.
     */
    LibraryFilter(VLNVDialer* dialer, QObject *parent = 0);

	//! The destructor
    virtual ~LibraryFilter();

public slots:

	//! Handler for when the text in vendor combobox is changed.
	void onVendorChanged(QString const&vendorText);

	//! Handler for when the text in library combobox is changed.
	void onLibraryChanged(QString const&libraryText);

	//! Handler for when the text in name combobox is changed.
	void onNameChanged(QString const&nameText);

	//! Handler for when the text in version combobox is changed.
	void onVersionChanged(QString const&versionText);

	//! Handler for when firmness options change.
	void onFirmnessChanged(Utils::FirmnessOptions const& options);

	//! Handler for when implementation options change.
	void onImplementationChanged(Utils::ImplementationOptions const& options);

	//! Handler for when type options change.
	void onTypeChanged(Utils::TypeOptions const& options);

	//! Handler for when hierarchy options change.
	void onHierarchyChanged(Utils::HierarchyOptions const& options);

protected:

	/*! Checks if the row should be shown in the view or not.
	 *
	 *     @param [in]  sourceRow       Specifies the row number of the item
	 *     @param [in]  sourceParent    Specifies the parent of the item.
	 *
	 *     @return True if item is shown.
	*/
	virtual bool filterAcceptsRow(int sourceRow, QModelIndex const& sourceParent) const = 0;
    
    /*!
     *  Gets the current firmness filter.
     *
     *      @return The firmness filter.
     */
    Utils::FirmnessOptions firmness() const;
    
    /*!
     *  Gets the current implementation filter.
     *
     *      @return The implementation filter.
     */
    Utils::ImplementationOptions implementation() const;
         
    /*!
     *  Gets the current VLNV type filter.
     *
     *      @return The VLNV type filter.
     */   
    Utils::TypeOptions type() const;

    /*!
     *  Checks if no VLVN filters have been set.
     *
     *      @return True, if all fields of VLNV filters are empty, otherwise false.
     */
    bool hasEmptyVLNVfilter() const;

    //! Check the firmness matching.
	bool checkFirmness(QSharedPointer<Component const> component) const;

	//! Check the implementation matching.
	bool checkImplementation(QSharedPointer<Component const> component) const;

	//! Check the hierarchy matching.
	bool checkHierarchy(QSharedPointer<Component const> component) const;

	/*! Check the list of vlnvs if they match the search rules
	 *
	 *     @param [in]  list Contains the vlnvs to check. 
	 *
	 * \return bool True if at least one match is found.
	*/
	bool checkVLNVs(QList<VLNV> const& list) const;

private:

	//! No copying
	LibraryFilter(const LibraryFilter& other);

	//! No assignment
	LibraryFilter& operator=(const LibraryFilter& other);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! Contains filters for re-usability
	Utils::FirmnessOptions firmness_;

	//! Contains filters for firmness
	Utils::ImplementationOptions implementation_;

	//! Contains filters for type
	Utils::TypeOptions type_;
 
	//! Contains filters for hierarchy
	Utils::HierarchyOptions hierarchy_; //!

	//! The validator that checks vendor rules
	QRegExpValidator vendorValidator_;

	//! The validator that checks library rules
	QRegExpValidator libraryValidator_;

	//! The validator that checks name rules
	QRegExpValidator nameValidator_;

	//! The validator that checks version rules
	QRegExpValidator versionValidator_;

	//! Contains filters for vendor
	QString vendorFilter_;

	//! Contains filters for library
	QString libraryFilter_; 

	//! Contains filters for name
    QString nameFilter_;  

	//! Contains filters for version
	QString versionFilter_; 

};

#endif // LIBRARYFILTER_H
