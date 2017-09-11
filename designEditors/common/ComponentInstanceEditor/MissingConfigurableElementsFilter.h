//-----------------------------------------------------------------------------
// File: MissingConfigurableElementsFilter.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 24.08.2017
//
// Description:
// A filter class for displaying only the unknown configurable elements.
//-----------------------------------------------------------------------------

#ifndef MISSINGCONFIGURABLESELEMENTFILTER_H
#define MISSINGCONFIGURABLESELEMENTFILTER_H

#include <designEditors/common/ComponentInstanceEditor/ParameterConfigurableElementsFilter.h>

#include <QSortFilterProxyModel>
#include <QModelIndex>

//-----------------------------------------------------------------------------
//! A filter class for displaying unknown configurable elements.
//-----------------------------------------------------------------------------
class MissingConfigurableElementsFilter : public ParameterConfigurableElementsFilter
{
    Q_OBJECT

public:
   
    /*!
     *  The constructor.
     *
     *      @param [in] parent  The owner of this filter.
     */
    MissingConfigurableElementsFilter(QObject* parent);

    /*!
     *  Destructor.
     */
    virtual ~MissingConfigurableElementsFilter();

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

private:
   
    // Disable copying.
    MissingConfigurableElementsFilter(MissingConfigurableElementsFilter const& rhs);
    MissingConfigurableElementsFilter& operator=(MissingConfigurableElementsFilter const& rhs);
};

#endif // MISSINGCONFIGURABLESELEMENTFILTER_H
