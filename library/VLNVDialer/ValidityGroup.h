//-----------------------------------------------------------------------------
// File: ValidityGroup.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 12.09.2023
//
// Description:
// Contains the check boxes to set the validity options for VLNVDialer.
//-----------------------------------------------------------------------------

#ifndef VALIDITYGROUP_H
#define VALIDITYGROUP_H

#include "FilterGroup.h"

#include <KactusAPI/include/utils.h>

#include <QGroupBox>
#include <QPushButton>

//-----------------------------------------------------------------------------
//! Contains the check boxes to set the re-usability options for VLNVDialer.
//-----------------------------------------------------------------------------
class ValidityGroup : public FilterGroup
{
	Q_OBJECT

public:

	/*!
     *  The constructor
	 *
	 *    @param [in] parent The owner of this widget.
     */
	ValidityGroup(QWidget *parent);
	
	/*!
     *  The destructor
     */
	virtual ~ValidityGroup() = default;

    //! No copying. No assignment.
    ValidityGroup(const ValidityGroup& other) = delete;
    ValidityGroup& operator=(const ValidityGroup& other) = delete;

    /*!
     *  Sets new validity options.
     *
     *    @param [in] options   The options to set.
     */
    void setValidity(Utils::ValidityOptions options);

    /*!
     *  Gets the selected validity options.
     *
     *    @return Selected validity options.
     */
    Utils::ValidityOptions getValidity() const;

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
	void optionsChanged(const Utils::ValidityOptions& options);

private slots:

	/*!
     *  Handler for valid check box state changes.
     */
	void onValidChanged(bool checked);

	/*!
     *  Handler for non-valid check box state changes.
     */
	void onNonvalidChanged(bool checked);


private:

	//! Check box to select valid items in/out of search results.
	QPushButton validBox_;

	//! Check box to select invalid items in/out of search results.
	QPushButton nonvalidBox_;

	//! Contains the current validity settings.
	Utils::ValidityOptions options_;
};

#endif // VALIDITYGROUP_H
