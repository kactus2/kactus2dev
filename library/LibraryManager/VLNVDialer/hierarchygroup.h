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

#include <common/utils.h>

#include <QGroupBox>
#include <QCheckBox>

//-----------------------------------------------------------------------------
//! Contains the check boxes to set the hierarchy options for VLNVDialer.
//-----------------------------------------------------------------------------
class HierarchyGroup : public QGroupBox
{
	Q_OBJECT

public:

	/*! The constructor
	 *
	 *      @param [in] parent The owner of this widget.
	 *
	*/
	HierarchyGroup(QWidget *parent);
	
	//! The destructor
	virtual ~HierarchyGroup();

    /*!
     *  Sets new hierarchy options.
     *
     *      @param [in] options   The options to set.
     */
    void setHierarchy(Utils::HierarchyOptions options);

    /*!
     *  Gets the selected hierarchy options.
     *
     *      @return Selected hierarchy options.
     */
    Utils::HierarchyOptions getHierarchy() const;

signals:

	//! Emitted when hierarchy options change.
	void optionsChanged(const Utils::HierarchyOptions& options);

private slots:

	//! Handler for global box state changes.
	void onFlatChange(bool checked);

	//! Handler for product box state changes.
	void onProductChange(bool checked);

	//! Handler for board box state changes.
	void onBoardChange(bool checked);

	//! Handler for chip box state changes.
	void onChipChange(bool checked);

	//! Handler for soc box state changes.
	void onSocChange(bool checked);

	//! Handler for ip box state changes.
	void onIpChange(bool checked);

private:
	//! No copying
	HierarchyGroup(const HierarchyGroup& other);

	//! No assignment
	HierarchyGroup& operator=(const HierarchyGroup& other);

	//! Check box to select global components in/out of search results.
	QCheckBox flatBox_;

	//! Check box to select product components in/out of search results.
	QCheckBox productBox_;

	//! Check box to select board components in/out of search results.
	QCheckBox boardBox_;

	//! Check box to select chip components in/out of search results.
	QCheckBox chipBox_;

	//! Check box to select SoC components in/out of search results.
	QCheckBox socBox_;

	//! Check box to select IP components in/out of search results.
	QCheckBox ipBox_;

	//! Contains the current search settings for hierarchy
	Utils::HierarchyOptions options_;
};

#endif // HIERARCHYGROUP_H
