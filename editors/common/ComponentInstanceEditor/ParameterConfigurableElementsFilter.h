//-----------------------------------------------------------------------------
// File: ParameterConfigurableElementsFilter.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 05.09.2017
//
// Description:
// A filter class for displaying only the configurable elements with existing referenced parameters.
//-----------------------------------------------------------------------------

#ifndef PARAMETERCONFIGURABLESELEMENTFILTER_H
#define PARAMETERCONFIGURABLESELEMENTFILTER_H

#include <editors/common/ComponentInstanceEditor/ConfigurableElementsFilter.h>

#include <QSortFilterProxyModel>
#include <QModelIndex>

//-----------------------------------------------------------------------------
//! A filter class for displaying only the configurable elements with existing referenced parameters.
//-----------------------------------------------------------------------------
class ParameterConfigurableElementsFilter : public ConfigurableElementsFilter
{
    Q_OBJECT

public:
   
    /*!
     *  The constructor.
     *
     *      @param [in] parent  The owner of this filter.
     */
    ParameterConfigurableElementsFilter(QObject* parent);

    /*!
     *  Destructor.
     */
    virtual ~ParameterConfigurableElementsFilter() = default;

    // Disable copying.
    ParameterConfigurableElementsFilter(ParameterConfigurableElementsFilter const& rhs) = delete;
    ParameterConfigurableElementsFilter& operator=(ParameterConfigurableElementsFilter const& rhs) = delete;

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
     *  Check if the default value of the indexed item is empty.
     *
     *      @param [in] sourceRow   Row of the indexed item.
     *
     *      @return True, if the default value of the indexed item is empty, false otherwise.
     */
    bool isIndexedDefaultValueEmpty(int sourceRow) const;
   
};

#endif // PARAMETERCONFIGURABLESELEMENTFILTER_H
