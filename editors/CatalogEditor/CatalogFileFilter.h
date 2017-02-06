//-----------------------------------------------------------------------------
// File: CatalogFileFilter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 03.02.2017
//
// Description:
// Class for filtering catalog files model.
//-----------------------------------------------------------------------------

#ifndef CatalogFileFILTER_H
#define CatalogFileFILTER_H

#include <common/utils.h>

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
     *      @param [in] parent   The parent object.
     */
    CatalogFileFilter(QObject *parent = 0);

	//! The destructor
    virtual ~CatalogFileFilter();

public slots:

    //! Called when an item should be added in the given index.
    void onAddItem(QModelIndex const& index);

    //! Called when an item should be removed from the given index.
    void onRemoveItem(QModelIndex const& index);

signals:

    //! Emitted when an item should be added in the given index.
    void addItem(QModelIndex const& index);

    //! Emitted when an item should be removed from the given index.
    void removeItem(QModelIndex const& index);

private:

	//! No copying
	CatalogFileFilter(const CatalogFileFilter& other);

	//! No assignment
	CatalogFileFilter& operator=(const CatalogFileFilter& other);

};

#endif // CatalogFileFILTER_H
