/* 
 *  	Created on: 27.2.2012
 *      Author: Antti Kamppi
 * 		filename: cpueditor.h
 *		Project: Kactus 2
 */

#ifndef CPUEDITOR_H
#define CPUEDITOR_H

#include <IPXactWrapper/ComponentEditor/itemeditor.h>

#include <models/component.h>
#include <models/cpu.h>

#include <common/widgets/nameGroupBox/namegroupbox.h>
#include <IPXactWrapper/ComponentEditor/cpus/addressspacerefeditor.h>
#include <common/widgets/ParameterGroupBox/parametergroupbox.h>

#include <QSharedPointer>

/*! \brief Editor to modify a cpu element within component.
 *
 */
class CpuEditor : public ItemEditor {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param component Pointer to the component being edited.
	 * \param dataPointer Pointer to the cpu being edited.
	 * \param parent Pointer to the owner of the editor.
	 *
	*/
	CpuEditor(QSharedPointer<Component> component,
		void* dataPointer,
		QWidget *parent = 0);

	/*! \brief The constructor
	 *
	 * \param component Pointer to the component being edited.
	 * \param cpu Pointer to the cpu being edited.
	 * \param parent Pointer to the owner of the editor.
	 *
	*/
	CpuEditor(QSharedPointer<Component> component,
		QSharedPointer<Cpu> cpu,
		QWidget *parent = 0);
	
	//! \brief The destructor
	virtual ~CpuEditor();

	/*! \brief Check for the validity of the edited cpu.
	* 
	* \return True if item is valid.
	*/
	virtual bool isValid() const;

	/*! \brief Remove the cpu from the model.
	 *
	*/
	virtual void removeModel();

	/*! \brief Make the changes from the cpu editor to the IPXact model.
	 *
	*/
	virtual void makeChanges();

	/*! \brief Read the settings from the model to the editor fields.
	 *
	*/
	virtual void restore();

	/*! \brief Reload the information from the model to the editor.
	*/
	virtual void refresh();

private:
	//! \brief No copying
	CpuEditor(const CpuEditor& other);

	//! \brief No assignment
	CpuEditor& operator=(const CpuEditor& other);

	//! \brief Set up the cpu editor.
	void initialize();

	//! \brief Pointer to the cpu being edited.
	Cpu* cpu_;

	//! \brief The editor to edit name and description.
	NameGroupBox nameGroup_;

	//! \brief The editor to set the address space references.
	AddressSpaceRefEditor addrSpaceRefs_;

	//! \brief The editor to set the parameters of a cpu.
	ParameterGroupBox parameters_;
};

#endif // CPUEDITOR_H
