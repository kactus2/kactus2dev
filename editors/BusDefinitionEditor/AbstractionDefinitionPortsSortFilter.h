//-----------------------------------------------------------------------------
// File: AbstractionDefinitionPortsSortFilter.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 21.11.2017
//
// Description:
// Sort filter proxy model for abstraction definition ports.
//-----------------------------------------------------------------------------

#ifndef ABSTRACTIONDEFINITIONPORTSSORTFILTER_H
#define ABSTRACTIONDEFINITIONPORTSSORTFILTER_H

#include <QSortFilterProxyModel>
#include <QColor>

//-----------------------------------------------------------------------------
//! Sort filter proxy model for abstraction definition ports.
//-----------------------------------------------------------------------------
class AbstractionDefinitionPortsSortFilter : public QSortFilterProxyModel
{

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] parent   Pointer to the owner of this model.
	 */
	AbstractionDefinitionPortsSortFilter(QObject *parent);
	
	/*!
	 *  The destructor.
	 */
    virtual ~AbstractionDefinitionPortsSortFilter();

	/*!
	 *  Get the data for the specified item.
	 *
	 *      @param [in] index   Specifies the item that's data is requested.
	 *      @param [in] role    The role that defines what kind of data is requested.
     *
     *      @return QVariant contains the data for the item.
	 */
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

private:
	//! No copying. No assignment.
	AbstractionDefinitionPortsSortFilter(const AbstractionDefinitionPortsSortFilter& other);
	AbstractionDefinitionPortsSortFilter& operator=(const AbstractionDefinitionPortsSortFilter& other);

    /*!
     *  Get the background color for the selected index.
     *
     *      @param [in] index   Index of the selected item.
     *
     *      @return Background color for the selected index.
     */
    QColor getBackgroundColorForIndex(QModelIndex const& index) const;

    /*!
     *  Check if the system group column is mandatory for the selected index.
     *
     *      @param [in] index   The selected index.
     *
     *      @return True, if the system group is mandatory, false otherwise.
     */
    bool isSystemGroupMandatory(QModelIndex const& index) const;
};

#endif // ABSTRACTIONDEFINITIONPORTSSORTFILTER_H
