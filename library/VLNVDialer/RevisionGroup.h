//-----------------------------------------------------------------------------
// File: RevisionGroup.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 05.12.2023
//
// Description:
// Contains the check boxes to set the revision options for VLNVDialer.
//-----------------------------------------------------------------------------

#ifndef REVISION_GROUP_H
#define REVISION_GROUP_H

#include "FilterGroup.h"

#include <KactusAPI/include/utils.h>

#include <QGroupBox>
#include <QPushButton>

//-----------------------------------------------------------------------------
//! Contains the check boxes to set the re-usability options for VLNVDialer.
//-----------------------------------------------------------------------------
class RevisionGroup : public FilterGroup
{
    Q_OBJECT

public:

	/*!
     *  The constructor
	 *
	 *    @param [in] parent The owner of this widget.
     */
	explicit RevisionGroup(QWidget *parent);
	
	/*!
     *  The destructor
     */
	virtual ~RevisionGroup() = default;

    /*!
     *  Sets new validity options.
     *
     *    @param [in] options   The options to set.
     */
    void setRevision(Utils::RevisionOptions options);

    /*!
     *  Gets the selected validity options.
     *
     *    @return Selected validity options.
     */
    [[nodiscard]] Utils::RevisionOptions getRevision() const;

    /*!
     *  Select/clear all filters.
     *
     *    @param [in] select     If true, all filters are set, otherwise all filters are cleared.
     */
	void selectAll(bool select) final;

signals:

	/*!
     *  Emitted when one of the check boxes changes state.
     */
	void optionsChanged(Utils::RevisionOptions const& options);

private slots:

	/*!
     *  Handler for valid check box state changes.
     */
	void onStd14Changed(bool checked);

	/*!
     *  Handler for non-valid check box state changes.
     */
	void onStd22Changed(bool checked);


private:

	//! Check box to select std 14 items in/out of search results.
    QPushButton std14Button_{ QString("2014"), this };

	//! Check box to select std 22 items in/out of search results.
    QPushButton std22Button_{ QString("2022"), this };

	//! Contains the current revision settings.
    Utils::RevisionOptions options_{};
};

#endif // REVISION_GROUP_H
