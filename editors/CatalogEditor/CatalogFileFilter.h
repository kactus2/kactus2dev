//-----------------------------------------------------------------------------
// File: CatalogFileFilter.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 03.02.2017
//
// Description:
// Class for filtering catalog files model.
//-----------------------------------------------------------------------------

#ifndef CatalogFileFILTER_H
#define CatalogFileFILTER_H

#include <KactusAPI/include/utils.h>
#include <IPXACTmodels/common/Document.h>

#include <QSharedPointer>
#include <QSortFilterProxyModel>
#include <QString>

//-----------------------------------------------------------------------------
//! Class for filtering catalog files model.
//-----------------------------------------------------------------------------
class CatalogFileFilter : public QSortFilterProxyModel
{
	Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *    @param [in] docRevision   The standard revision of the document.
     *    @param [in] parent        The parent object.
     */
    CatalogFileFilter(Document::Revision docRevision, QObject *parent = 0);

	//! The destructor
    virtual ~CatalogFileFilter();

public slots:

    //! Called when an item should be added in the given index.
    void onAddItem(QModelIndex const& index);

    //! Called when an item should be removed from the given index.
    void onRemoveItem(QModelIndex const& index);

    //! Called when an item should be opened in the given index.
    void onOpenItem(QModelIndex const& index);

signals:

    //! Emitted when an item should be added in the given index.
    void addItem(QModelIndex const& index);

    //! Emitted when an item should be removed from the given index.
    void removeItem(QModelIndex const& index);

    //! Emitted when an item should be opened in the given index.
    void openItem(QModelIndex const& index);
protected:

    /*!
     *  Implementation of the row filtering.
     *
     *    @param [in] source_row      The row to check for filtering.
     *    @param [in] source_parent   The parent index of the row.
     *
     *    @return True, if the row passes the filters, otherwise false.
     */
    bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const override;

private:

	//! No copying
	CatalogFileFilter(const CatalogFileFilter& other);

	//! No assignment
	CatalogFileFilter& operator=(const CatalogFileFilter& other);

    //! The std revision of the catalog document.
    Document::Revision docRevision_;

};

#endif // CatalogFileFILTER_H
