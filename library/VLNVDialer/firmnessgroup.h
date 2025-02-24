//-----------------------------------------------------------------------------
// File: firmnessgroup.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 11.07.2011
//
// Description:
// Contains the check boxes to set the re-usability options for VLNVDialer.
//-----------------------------------------------------------------------------

#ifndef FIRMNESSGROUP_H
#define FIRMNESSGROUP_H

#include "FilterGroup.h"

#include <KactusAPI/include/utils.h>

#include <QGroupBox>
#include <QPushButton>

//-----------------------------------------------------------------------------
//! Contains the check boxes to set the re-usability options for VLNVDialer.
//-----------------------------------------------------------------------------
class FirmnessGroup : public FilterGroup
{
	Q_OBJECT

public:

	/*!
     *  The constructor
	 *
	 *    @param [in] parent The owner of this widget.
     */
	FirmnessGroup(QWidget *parent);
	
	/*!
     *  The destructor
     */
	virtual ~FirmnessGroup() = default;

    //! No copying. No assignment.
    FirmnessGroup(const FirmnessGroup& other) = delete;
    FirmnessGroup& operator=(const FirmnessGroup& other) = delete;

    /*!
     *  Sets new firmness options.
     *
     *    @param [in] options   The options to set.
     */
    void setFirmness(Utils::FirmnessOptions options);

    /*!
     *  Gets the selected firmness options.
     *
     *    @return Selected firmness options.
     */
    Utils::FirmnessOptions getFirmness() const;

    /*!
     *  Select/clear all filters.
     *
     *    @param [in] select     If true, all filters are set, otherwise all filters are cleared.
     */
	void selectAll(bool select) override final;

signals:

	/*!
     *  Emitted when one of the check boxes changes state.
     */
	void optionsChanged(const Utils::FirmnessOptions& options);

private slots:

	/*!
     *  Handler for template check box state changes.
     */
	void onTemplateChanged(bool checked);

	/*!
     *  Handler for mutable check box state changes.
     */
	void onMutableChanged(bool checked);

	/*!
     *  Handler for configuration check box state changes.
     */
	void onConfigurationChanged(bool checked);

private:

	//! Check box to select templates in/out of search results.
	QPushButton templateBox_;

	//! Check box to select mutable in/out of search results.
	QPushButton mutableBox_;

	//! Check box to select fixed in/out of search results.
	QPushButton fixedBox_;

	//! Contains the current re-usability settings.
	Utils::FirmnessOptions options_;
};

#endif // FIRMNESSGROUP_H
