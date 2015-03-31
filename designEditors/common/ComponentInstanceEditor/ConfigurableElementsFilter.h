//-----------------------------------------------------------------------------
// File: ConfigurableElementsFilter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 30.03.2015
//
// Description:
// A filter class for configurable elements.
//-----------------------------------------------------------------------------

#ifndef CONFIGURABLESELEMENTFILTER_H
#define CONFIGURABLESELEMENTFILTER_H

#include <QSortFilterProxyModel>
#include <QModelIndex>

//-----------------------------------------------------------------------------
//! A filter class for configurable elements.
//-----------------------------------------------------------------------------
class ConfigurableElementsFilter : public QSortFilterProxyModel
{
    Q_OBJECT

public:
   
    /*!
     *  The constructor.
     *
     *      @param [in] parent  The owner of this filter.
     */
    ConfigurableElementsFilter(QObject* parent);

    /*!
     *  Destructor.
     */
    ~ConfigurableElementsFilter();

signals:

    /*!
     *  Emitted when the user attempts to remove an item.
     *
     *      @param [in] index   The index of the item to be removed.
     */
    void removeItem(QModelIndex const& index);

protected:

    /*!
     *  Validate the given row on a parent and check if the item should be shown.
     *
     *      @param [in] source_row      The row number of the child on the parent.
     *      @param [in] source_parent   Model index that identifies the parent object.
     *
     *      @return True, if the object should be displayed, false otherwise.
     */
    virtual bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;

public slots:
   
    /*!
     *  Removes an item of the given index from the model.
     *
     *      @param [in] filterIndex     The index of the item to be removed.
     */
    virtual void onRemoveItem(QModelIndex const& filterIndex);

private:
   
    // Disable copying.
    ConfigurableElementsFilter(ConfigurableElementsFilter const& rhs);
    ConfigurableElementsFilter& operator=(ConfigurableElementsFilter const& rhs);

    //! The index of the item to be removed.
    QModelIndex indexOfRemovedElement_;
};

#endif // CONFIGURABLESELEMENTFILTER_H
