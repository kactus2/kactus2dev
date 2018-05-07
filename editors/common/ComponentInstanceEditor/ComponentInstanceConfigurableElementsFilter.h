//-----------------------------------------------------------------------------
// File: ComponentInstanceConfigurableElementsFilter.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 23.08.2017
//
// Description:
// A filter class for component instance configurable elements.
//-----------------------------------------------------------------------------

#ifndef COMPONENTINSTANCECONFIGURABLESELEMENTFILTER_H
#define COMPONENTINSTANCECONFIGURABLESELEMENTFILTER_H

#include <QSortFilterProxyModel>
#include <QModelIndex>

//-----------------------------------------------------------------------------
//! A filter class for component instance configurable elements.
//-----------------------------------------------------------------------------
class ComponentInstanceConfigurableElementsFilter : public QSortFilterProxyModel
{
    Q_OBJECT

public:
   
    /*!
     *  The constructor.
     *
     *      @param [in] parent  The owner of this filter.
     */
    ComponentInstanceConfigurableElementsFilter(QObject* parent);

    /*!
     *  Destructor.
     */
    ~ComponentInstanceConfigurableElementsFilter();

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
     *  Set the immediate values to be shown or hidden.
     *
     *      @param [in] show    Flag for displaying the immediate values.
     */
    void setShowImmediateValues(bool show);

    /*!
     *  Invalidate the filter.
     */
    void onInvalidateFilter();

private:
   
    // Disable copying.
    ComponentInstanceConfigurableElementsFilter(ComponentInstanceConfigurableElementsFilter const& rhs);
    ComponentInstanceConfigurableElementsFilter& operator=(ComponentInstanceConfigurableElementsFilter const& rhs);

    /*!
     *  Check if the selected index contains editable children.
     *
     *      @param [in] index   The selected model index.
     *
     *      @return True, if the selected index contains editable children, false otherwise.
     */
    bool indexContainsEditableChildren(QModelIndex const& index) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Flag for displaying configurable elements with immediate values.
    bool showImmediateValues_;
};

#endif // COMPONENTINSTANCECONFIGURABLESELEMENTFILTER_H
