//-----------------------------------------------------------------------------
// File: implementationgroup.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 11.07.2011
//
// Description:
// Contains check boxes to set the implementation options for VLNVDialer.
//-----------------------------------------------------------------------------

#ifndef IMPLEMENTATIONGROUP_H
#define IMPLEMENTATIONGROUP_H

#include "FilterGroup.h"

#include <KactusAPI/include/utils.h>

#include <QPushButton>

//-----------------------------------------------------------------------------
//! Contains check boxes to set the implementation options for VLNVDialer.
//-----------------------------------------------------------------------------
class ImplementationGroup : public FilterGroup
{
	Q_OBJECT

public:

	/*!
     *  The constructor
	 *
	 *    @param [in] parent The owner of this widget.
	 */
	ImplementationGroup(QWidget *parent);
	
	/*!
     *  The destructor.
     */
	virtual ~ImplementationGroup() = default;

    //! No copying. No assignment.
    ImplementationGroup(const ImplementationGroup& other) = delete;
    ImplementationGroup& operator=(const ImplementationGroup& other) = delete;

    /*!
     *  Sets new implementation options.
     *
     *    @param [in] options   The options to set.
     */
    void setImplementation(Utils::ImplementationOptions options);

    /*!
     *  Gets the selected implementation options.
     *
     *    @return Selected implementation options.
     */
    Utils::ImplementationOptions getImplementation() const;

	/*!
	 *  Select/clear all filters.
	 *
	 *    @param [in] select     If true, all filters are set, otherwise all filters are cleared.
	 */
    void selectAll(bool select) override final;

signals:

	/*!
     *  Emitted when implementation options changes state.
     */
	void optionsChanged(const Utils::ImplementationOptions& options);

private slots:

	/*!
     *  Handler for hwBox state changes.
     */
	void onHWChanged(bool checked);

	/*!
     *  Handler for swBox state changes.
     */
	void onSWChanged(bool checked);

	/*!
     *  Handler for systemBox state changes.
     */
	void onSystemChanged(bool checked);

private:

	//! Setup the widget layout.
    void setupLayout();

	//! Check box to select hardware components in/out of search results.
	QPushButton hwBox_;

	//! Check box to select software component in/out of search results.
	QPushButton swBox_;

	//! Check box to select system components in/out of search results.
	QPushButton systemBox_;

	//! Contains the current search settings.
	Utils::ImplementationOptions options_;
};

#endif // IMPLEMENTATIONGROUP_H
