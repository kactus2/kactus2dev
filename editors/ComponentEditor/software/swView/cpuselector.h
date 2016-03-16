//-----------------------------------------------------------------------------
// File: cpuselector.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 03.04.2013
//
// Description:
// Combo box to select one of the CPU elements of a component.
//-----------------------------------------------------------------------------

#ifndef CPUSELECTOR_H
#define CPUSELECTOR_H

#include <QComboBox>
#include <QSharedPointer>

class Component;
//-----------------------------------------------------------------------------
//! Combo box to select one of the CPU elements of a component.
//-----------------------------------------------------------------------------
class CpuSelector : public QComboBox
{
	Q_OBJECT

public:

	/*! The constructor.
	 *
	 *      @param [in] component Pointer to the component to select the CPU from.
	 *      @param [in] parent Pointer to the owner of the combo box.
	*/
	CpuSelector(QSharedPointer<Component> component, QWidget* parent);
	
	//! The destructor.
	virtual ~CpuSelector();

public slots:

	//! Refresh the items to be selected in the combo box.
	void refresh();

	/*! Set the specified address space as selected.
	 *
	 *      @param [in] cpuName The name of the address space to select.
	*/
	void selectCpu(QString const& cpuName);

signals:

	//! Emitted when CPU was selected by user.
	void cpuSelected(QString const& cpuName);

private slots:

	//! Handler for selected index changes.
	void onIndexChange(int newIndex);

private:
	
	//! No copying
	CpuSelector(const CpuSelector& other);

	//! No assignment
	CpuSelector& operator=(const CpuSelector& other);

	//! Pointer to the component to select the CPU from.
	QSharedPointer<Component> component_;
};

#endif // CPUSELECTOR_H
