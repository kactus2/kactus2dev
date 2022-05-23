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
	 *      @param [in] parent The owner of this widget.
	 */
	FilterWidget(QWidget *parent);
	
	/*!
     *  The destructor.
     */
	virtual ~FilterWidget();

    /*!
     *  Setup the filters.
     *
     *      @param [in] options     New filter options.
     */
    void setFilters(Utils::FilterOptions options);

    /*!
     *  Get the filters.
     *
     *      @return The current filter options.
     */
    Utils::FilterOptions getFilters() const;

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
     *  Emitted when all options change.
     */
    void optionsChanged(Utils::FilterOptions const& options);

private:
	//! No copying. No assignment.
	FilterWidget(const FilterWidget& other);
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
