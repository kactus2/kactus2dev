//-----------------------------------------------------------------------------
// File: FilterGroup.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 24.04.2023
//
// Description:
// Base class for library filter groups
//-----------------------------------------------------------------------------

#ifndef FILTERGROUP_H
#define FILTERGROUP_H

#include <QGroupBox>
#include <QPushButton>

//-----------------------------------------------------------------------------
//! Base class for library filter groups.
//-----------------------------------------------------------------------------
class FilterGroup : public QGroupBox
{
	Q_OBJECT

public:

	/*!
     *  The constructor
	 *
	 *      @param [in] parent The owner of this widget.
	 */
	FilterGroup(QString const& title, QWidget *parent);

    //! No copying. No assignment.
	FilterGroup(const FilterGroup& other) = delete;
	FilterGroup& operator=(const FilterGroup& other) = delete;

	/*!
     *  The destructor
     */
	virtual ~FilterGroup() = default;

	/*!
	 *  Select/clear all filters.
	 *
	 *      @param [in] select     If true, all filters are set, otherwise all filters are cleared.
	 */
	virtual void selectAll(bool select) = 0;

protected:

	//! Setups the filter button.
    void setupButton(QPushButton* button, QString const& tooltip);
};

#endif // FILTERGROUP_H
