/* 
 *  	Created on: 11.7.2011
 *      Author: Antti Kamppi
 * 		filename: filterwidget.h
 *		Project: Kactus 2
 */

#ifndef FILTERWIDGET_H
#define FILTERWIDGET_H

#include <common/utils.h>

#include "firmnessgroup.h"
#include "implementationgroup.h"
#include "typegroup.h"
#include "hierarchygroup.h"

#include <QWidget>

/*! \brief Contains the widgets to set the filter options for VLNVDialer.
 *
 */
class FilterWidget : public QWidget {
	Q_OBJECT

public:

    struct FilterOptions
    {
	    //! \brief Contains Type settings.
        Utils::TypeOptions type;

	    //! \brief Contains Implementation settings.
        Utils::ImplementationOptions implementation;

	    //! \brief Contains Hierarchy settings.
        Utils::HierarchyOptions hierarchy;

	    //! \brief Contains Firmness settings.
        Utils::FirmnessOptions firmness;

		/*! \brief The default constructor
		 *
		 * Constructs struct with all options set to true.
		*/
        FilterOptions() : type(), implementation(), hierarchy(), firmness() {};
    };

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this widget.
	 *
	*/
	FilterWidget(QWidget *parent);
	
	//! \brief The destructor
	virtual ~FilterWidget();

    void setFilters(FilterOptions options);

    FilterOptions getFilters() const;

signals:

	//! \brief Emitted when re-usability options change.
	void firmnessChanged(const Utils::FirmnessOptions& options);

	//! \brief Emitted when firmness options change.
	void implementationChanged(const Utils::ImplementationOptions& options);

	//! \brief Emitted when type options change.
	void typeChanged(const Utils::TypeOptions& options);

	//! \brief Emitted when hierarchy options change.
	void hierarchyChanged(const Utils::HierarchyOptions& options);

private:
	//! \brief No copying
	FilterWidget(const FilterWidget& other);

	//! \brief No assignment
	FilterWidget& operator=(const FilterWidget& other);

	//! \brief Contains items to select the re-usability options.
	FirmnessGroup firmness_;

	//! \brief Contains items to select the firmness options.
	ImplementationGroup implementation_;

	//! \brief Contains items to select the type options.
	TypeGroup type_;

	//! \brief Contains items to select the hierarchy options.
	HierarchyGroup hierarchy_;
};

#endif // FILTERWIDGET_H
