/* 
 *  	Created on: 11.7.2011
 *      Author: Antti Kamppi
 * 		filename: hierarchygroup.h
 *		Project: Kactus 2
 */

#ifndef HIERARCHYGROUP_H
#define HIERARCHYGROUP_H

#include <common/utils.h>

#include <QGroupBox>
#include <QCheckBox>

/*! \brief Contains the check boxes to set the hierarchy options for VLNVDialer.
 *
 */
class HierarchyGroup : public QGroupBox {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this widget.
	 *
	*/
	HierarchyGroup(QWidget *parent);
	
	//! \brief The destructor
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

	//! \brief Emitted when hierarchy options change.
	void optionsChanged(const Utils::HierarchyOptions& options);

private slots:

	//! \brief Handler for global box state changes.
	void onGlobalChange(bool checked);

	//! \brief Handler for product box state changes.
	void onProductChange(bool checked);

	//! \brief Handler for board box state changes.
	void onBoardChange(bool checked);

	//! \brief Handler for chip box state changes.
	void onChipChange(bool checked);

	//! \brief Handler for soc box state changes.
	void onSocChange(bool checked);

	//! \brief Handler for ip box state changes.
	void onIpChange(bool checked);

private:
	//! \brief No copying
	HierarchyGroup(const HierarchyGroup& other);

	//! \brief No assignment
	HierarchyGroup& operator=(const HierarchyGroup& other);

	//! \brief Check box to select global components in/out of search results.
	QCheckBox globalBox_;

	//! \brief Check box to select product components in/out of search results.
	QCheckBox productBox_;

	//! \brief Check box to select board components in/out of search results.
	QCheckBox boardBox_;

	//! \brief Check box to select chip components in/out of search results.
	QCheckBox chipBox_;

	//! \brief Check box to select SoC components in/out of search results.
	QCheckBox socBox_;

	//! \brief Check box to select IP components in/out of search results.
	QCheckBox ipBox_;

	//! \brief Contains the current search settings for hierarchy
	Utils::HierarchyOptions options_;
};

#endif // HIERARCHYGROUP_H
