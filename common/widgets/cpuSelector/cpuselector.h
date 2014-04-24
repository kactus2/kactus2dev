/* 
 *	Created on:	3.4.2013
 *	Author:		Antti Kamppi
 *	File name:	cpuselector.h
 *	Project:		Kactus 2
*/

#ifndef CPUSELECTOR_H
#define CPUSELECTOR_H

#include <IPXACTmodels/component.h>

#include <QComboBox>
#include <QSharedPointer>

/*! \brief Combo box to select one of the CPU elements of a component.
 *
 */
class CpuSelector : public QComboBox {
	Q_OBJECT

public:

	/*! \brief The constructor.
	 *
	 * Method: 		CpuSelector
	 * Full name:	CpuSelector::CpuSelector
	 * Access:		private 
	 *
	 * \param component Pointer to the component to select the CPU from.
	 * \param parent Pointer to the owner of the combo box.
	 *
	*/
	CpuSelector(QSharedPointer<Component> component, QWidget *parent);
	
	//! \brief The destructor.
	virtual ~CpuSelector();

public slots:

	//! \brief Refresh the items to be selected in the combo box.
	void refresh();

	/*! \brief Set the specified address space as selected.
	 *
	 * \param cpuName The name of the address space to select.
	 *
	*/
	void selectCpu(const QString& cpuName);

signals:

	//! \brief Emitted when CPU was selected by user.
	void cpuSelected(const QString& cpuName);

private slots:

	//! \brief Handler for selected index changes.
	void onIndexChange(int newIndex);

private:
	
	//! \brief No copying
	CpuSelector(const CpuSelector& other);

	//! \brief No assignment
	CpuSelector& operator=(const CpuSelector& other);

	//! \brief Pointer to the component to select the CPU from.
	QSharedPointer<Component> component_;
};

#endif // CPUSELECTOR_H
