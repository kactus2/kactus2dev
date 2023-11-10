//-----------------------------------------------------------------------------
// File: ConfigurableElementsFilter.h
//-----------------------------------------------------------------------------
// Project: Kactus2
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
    virtual ~ConfigurableElementsFilter() = default;

    // Disable copying.
    ConfigurableElementsFilter(ConfigurableElementsFilter const& rhs) = delete;
    ConfigurableElementsFilter& operator=(ConfigurableElementsFilter const& rhs) = delete;
   
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
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

    /*!
     *  Check if the immediate values are visible.
     *
     *      @return True, if the immediate values are visible, false otherwise.
     */
    bool immediateValuesAreVisible() const;

    /*!
     *  Check if the item at the selected index is visible.
     *
     *      @param [in] sourceRow       Row of the selected index.
     *      @param [in] sourceParent    Parent index of the selected item.
     *
     *      @return True, if the indexed item is visible, false otherwise.
     */
    bool itemAtIndexIsEditable(int sourceRow, QModelIndex const& sourceParent) const;

public slots:
   
     /*!
      *  Set the flag for displaying the immediate values.
      *
      *      @param [in] show   Flag for displaying the immediate values.
      */
     void setShowImmediateValues(bool show);

    /*!
     *  Removes an item of the given index from the model.
     *
     *      @param [in] filterIndex     The index of the item to be removed.
     */
    void onRemoveItem(QModelIndex const& filterIndex);

private:
   

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Flag for displaying the immediate values.
    bool showImmediateValues_ = false;
};

#endif // CONFIGURABLESELEMENTFILTER_H
