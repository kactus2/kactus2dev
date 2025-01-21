//-----------------------------------------------------------------------------
// File: filterwidget.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 11.07.2011
//
// Description:
// Contains the widgets to set the filter options for VLNVDialer.
//-----------------------------------------------------------------------------

#ifndef FILTERWIDGET_H
#define FILTERWIDGET_H

#include <KactusAPI/include/utils.h>

#include "firmnessgroup.h"
#include "implementationgroup.h"
#include "typegroup.h"
#include "hierarchygroup.h"
#include "RevisionGroup.h"
#include "ValidityGroup.h"

#include <QWidget>

//-----------------------------------------------------------------------------
//! Contains the widgets to set the filter options for VLNVDialer.
//-----------------------------------------------------------------------------
class FilterWidget : public QWidget
{
	Q_OBJECT

public:

	/*!
     *  The constructor
	 *
	 *    @param [in] parent The owner of this widget.
	 */
	FilterWidget(QWidget *parent);
	
	/*!
     *  The destructor.
     */
	virtual ~FilterWidget() = default;

    //! No copying. No assignment.
    FilterWidget(const FilterWidget& other) = delete;
    FilterWidget& operator=(const FilterWidget& other) = delete;

    /*!
     *  Setup the filters.
     *
     *    @param [in] options     New filter options.
     */
    void setFilters(Utils::FilterOptions options);

    /*!
     *  Get the filters.
     *
     *    @return The current filter options.
     */
    Utils::FilterOptions getFilters() const;

    /*!
     *  Select/clear all filters.
     *
     *    @param [in] select     If true, all filters are set, otherwise all filters are cleared.
     */
	void selectAll(bool select);

signals:

	/*!
     *  Emitted when re-usability options change.
     */
	void firmnessChanged(const Utils::FirmnessOptions& options);

	/*!
     *  Emitted when firmness options change.
     */
	void implementationChanged(const Utils::ImplementationOptions& options);

	/*!
     *  Emitted when type options change.
     */
	void typeChanged(const Utils::TypeOptions& options);

	/*!
     *  Emitted when hierarchy options change.
     */
	void hierarchyChanged(const Utils::HierarchyOptions& options);

    /*!
     *  Emitted when revision options change.
     */
    void revisionChanged(const Utils::RevisionOptions& options);

    /*!
     *  Emitted when validity options change.
     */
    void validityChanged(const Utils::ValidityOptions& options);

    /*!
     *  Emitted when all options change.
     */
    void optionsChanged(Utils::FilterOptions const& options);

private:

	//! Contains items to select the re-usability options.
	FirmnessGroup firmness_;

	//! Contains items to select the firmness options.
	ImplementationGroup implementation_;

	//! Contains items to select the type options.
	TypeGroup type_;

	//! Contains items to select the hierarchy options.
	HierarchyGroup hierarchy_;

    //! Contains items to select the validity options.
    ValidityGroup validity_;

    //! Contains items to select the revision options.
    RevisionGroup revision_;
};

#endif // FILTERWIDGET_H
