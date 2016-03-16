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

#include <common/utils.h>

#include "firmnessgroup.h"
#include "implementationgroup.h"
#include "typegroup.h"
#include "hierarchygroup.h"

#include <QWidget>

//-----------------------------------------------------------------------------
//! Contains the widgets to set the filter options for VLNVDialer.
//-----------------------------------------------------------------------------
class FilterWidget : public QWidget
{
	Q_OBJECT

public:

    struct FilterOptions
    {
	    //! Contains Type settings.
        Utils::TypeOptions type;

	    //! Contains Implementation settings.
        Utils::ImplementationOptions implementation;

	    //! Contains Hierarchy settings.
        Utils::HierarchyOptions hierarchy;

	    //! Contains Firmness settings.
        Utils::FirmnessOptions firmness;

		/*! The default constructor
		 *
		 * Constructs struct with all options set to true.
		*/
        FilterOptions() : type(), implementation(), hierarchy(), firmness() {};
    };

	/*! The constructor
	 *
	 *      @param [in] parent The owner of this widget.
	 *
	*/
	FilterWidget(QWidget *parent);
	
	//! The destructor
	virtual ~FilterWidget();

    void setFilters(FilterOptions options);

    FilterOptions getFilters() const;

signals:

	//! Emitted when re-usability options change.
	void firmnessChanged(const Utils::FirmnessOptions& options);

	//! Emitted when firmness options change.
	void implementationChanged(const Utils::ImplementationOptions& options);

	//! Emitted when type options change.
	void typeChanged(const Utils::TypeOptions& options);

	//! Emitted when hierarchy options change.
	void hierarchyChanged(const Utils::HierarchyOptions& options);

private:
	//! No copying
	FilterWidget(const FilterWidget& other);

	//! No assignment
	FilterWidget& operator=(const FilterWidget& other);

	//! Contains items to select the re-usability options.
	FirmnessGroup firmness_;

	//! Contains items to select the firmness options.
	ImplementationGroup implementation_;

	//! Contains items to select the type options.
	TypeGroup type_;

	//! Contains items to select the hierarchy options.
	HierarchyGroup hierarchy_;
};

#endif // FILTERWIDGET_H
