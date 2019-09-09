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

#include <IPXACTmodels/common/TagData.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/kactusExtensions/KactusAttribute.h>

#include <QRegExpValidator>
#include <QSharedPointer>
#include <QSortFilterProxyModel>
#include <QString>

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
     *      @param [in] libraryAccess   Access to the library.
     *      @param [in] parent          The parent object.
     *
     *      @return <Description>.
     */
    LibraryFilter(LibraryInterface* libraryAccess, QObject *parent = 0);

	//! The destructor
    virtual ~LibraryFilter() = default;

public slots:

	//! Handler for when the text in vendor combobox is changed.
	void onVendorChanged(QString const& vendorText);

	//! Handler for when the text in library combobox is changed.
	void onLibraryChanged(QString const& libraryText);

	//! Handler for when the text in name combobox is changed.
	void onNameChanged(QString const& nameText);

	//! Handler for when the text in version combobox is changed.
	void onVersionChanged(QString const& versionText);

    //! Handler for when all options are changed.
    void onFiltersChanged(Utils::FilterOptions const& filters);

	//! Handler for when firmness options change.
	void onFirmnessChanged(Utils::FirmnessOptions const& options);

	//! Handler for when implementation options change.
	void onImplementationChanged(Utils::ImplementationOptions const& options);

	//! Handler for when type options change.
	void onTypeChanged(Utils::TypeOptions const& options);

	//! Handler for when hierarchy options change.
	void onHierarchyChanged(Utils::HierarchyOptions const& options);

    /*!
     *  Handler for changing the filtered tags.
     *
     *      @param [in] filteredTags    The new visible tags.
     */
    void onTagFilterChanged(QVector<TagData> filteredTags);

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

    /*!
     *  Check the tag matching of the selected document.
     *
     *      @param [in] document    The selected document.
     *
     *      @return True, if the tags match the visible tags, false otherwise.
     */
    bool checkTags(QSharedPointer<Document const> document) const;

	/*! Check the list of vlnvs if they match the search rules
	 *
	 *      @param [in]  list Contains the vlnvs to check. 
	 *
	 *      @return True if at least one match is found.
	*/
	bool checkVLNVs(QVector<VLNV> const& list) const;

    /*!
     *  Get the access to the library.
     *
     *      @return Access to the library.
     */
    LibraryInterface* getLibraryInterface() const;

private:

	//! No copying
	LibraryFilter(const LibraryFilter& other);

	//! No assignment
	LibraryFilter& operator=(const LibraryFilter& other);

    /*!
     *  Check if a document contains the selected tag.
     *
     *      @param [in] document    The selected document.
     *      @param [in] tag         The selected tag.
     *
     *      @return True, if the selected document contains the selected tag, false otherwise.
     */
    bool documentContainsTag(QSharedPointer<Document const> document, TagData const& tag) const;

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

    //! List of the visible tags.
    QVector<TagData> tags_;

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

    //! Access to the library.
    LibraryInterface* libraryAccess_;
};

#endif // LIBRARYFILTER_H
