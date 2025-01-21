//-----------------------------------------------------------------------------
// File: hierarchygroup.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 11.07.2011
//
// Description:
// Contains the check boxes to set the hierarchy options for VLNVDialer.
//-----------------------------------------------------------------------------

#ifndef HIERARCHYGROUP_H
#define HIERARCHYGROUP_H

#include "FilterGroup.h"

#include <KactusAPI/include/utils.h>

#include <QGroupBox>
#include <QPushButton>

//-----------------------------------------------------------------------------
//! Contains the check boxes to set the hierarchy options for VLNVDialer.
//-----------------------------------------------------------------------------
class HierarchyGroup : public FilterGroup
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
	 *    @param [in] parent The owner of this widget.
	 */
	HierarchyGroup(QWidget *parent);
	
	/*!
     *  The destructor.
     */
	virtual ~HierarchyGroup() = default;

    //! No copying. No assignment.
    HierarchyGroup(const HierarchyGroup& other) = delete;
    HierarchyGroup& operator=(const HierarchyGroup& other) = delete;

    /*!
     *  Sets new hierarchy options.
     *
     *    @param [in] options   The options to set.
     */
    void setHierarchy(Utils::HierarchyOptions options);

    /*!
     *  Gets the selected hierarchy options.
     *
     *    @return Selected hierarchy options.
     */
    Utils::HierarchyOptions getHierarchy() const;

	/*!
	 *  Select/clear all filters.
	 *
	 *    @param [in] select     If true, all filters are set, otherwise all filters are cleared.
	 */
	void selectAll(bool select) override final;

signals:

	/*!
     *  Emitted when hierarchy options change.
     */
	void optionsChanged(const Utils::HierarchyOptions& options);

private slots:

	/*!
     *  Handler for global box state changes.
     */
	void onFlatChange(bool checked);

	/*!
     *  Handler for product box state changes.
     */
	void onProductChange(bool checked);

	/*!
     *  Handler for board box state changes.
     */
	void onBoardChange(bool checked);

	/*!
     *  Handler for chip box state changes.
     */
	void onChipChange(bool checked);

	/*!
     *  Handler for soc box state changes.
     */
	void onSocChange(bool checked);

	/*!
     *  Handler for ip box state changes.
     */
	void onIpChange(bool checked);

private:

	//! Check box to select global components in/out of search results.
	QPushButton flatBox_;

	//! Check box to select product components in/out of search results.
	QPushButton productBox_;

	//! Check box to select board components in/out of search results.
	QPushButton boardBox_;

	//! Check box to select chip components in/out of search results.
	QPushButton chipBox_;

	//! Check box to select SoC components in/out of search results.
	QPushButton socBox_;

	//! Check box to select IP components in/out of search results.
	QPushButton ipBox_;

	//! Contains the current search settings for hierarchy
	Utils::HierarchyOptions options_;
};

#endif // HIERARCHYGROUP_H
