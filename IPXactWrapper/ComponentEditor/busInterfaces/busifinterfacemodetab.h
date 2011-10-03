/* 
 *
 *  Created on: 5.4.2011
 *      Author: Antti Kamppi
 * 		filename: busifinterfacemodetab.h
 */

#ifndef BUSIFINTERFACEMODETAB_H
#define BUSIFINTERFACEMODETAB_H

#include <models/generaldeclarations.h>

#include <QWidget>
#include <QComboBox>
#include <QStackedWidget>

class BusInterface;
class BusIfGeneralTab;
class LibraryInterface;
class Component;

/*! \brief Container for interface mode editors and to select them.
 *
 */
class BusIfInterfaceModeTab : public QWidget {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param libHandler Pointer to the library handler instance.
	 * \param generalTab Pointer to the general tab of bus interface editor.
	 * \param component Pointer to the component being edited.
	 * \param dataPointer Pointer to the bus interface being edited.
	 * \param parent Pointer to the owner of this widget.
	 *
	*/
	BusIfInterfaceModeTab(LibraryInterface* libHandler,
		BusIfGeneralTab* generalTab, 
		QSharedPointer<Component> component,
		void* dataPointer, 
		QWidget *parent);
	
	//! \brief The destructor
	virtual ~BusIfInterfaceModeTab();

	/*! \brief Check for the validity of the edited item.
	*
	* \return True if item is valid.
	*/
	virtual bool isValid() const;

	/*! \brief Restore the changes made in the editor back to ones in the model.
	*
	*/
	virtual void restoreChanges();

	/*! \brief Applies the changes made with the editor to the model.
	*
	* After calling this function it is no longer possible to automatically 
	* restore the previous state of the model.
	* 
	* Note: if the editor is not in valid state nothing is changed.
	*/
	virtual void applyChanges();

	/*! \brief Get the interface mode of the editor
	 * 
	 * \return General::InterfaceMode Specifies the interface mode.
	*/
	virtual General::InterfaceMode getInterfaceMode();

public slots:

	/*! \brief Handler for mode selector's currentIndexChanged() signal
	 * 
	 * This handler changes the interface mode editor to one that matches the
	 * new index.
	 * 
	 * \param index The new index of the selector.
	 *
	*/
	void onIndexChanged(int index);

signals:

	//! \brief Emitted when contents of the model change
	void contentChanged();

	//! \brief Prints an error message to the user.
	void errorMessage(const QString& msg) const;

	//! \brief Prints a notification to user.
	void noticeMessage(const QString& msg) const;

private:

	//! \brief No copying
	BusIfInterfaceModeTab(const BusIfInterfaceModeTab& other);

	//! No assignment
	BusIfInterfaceModeTab& operator=(const BusIfInterfaceModeTab& other);

	/*! \brief Set up the different interface mode editors.
	 *
	*/
	void setupEditors(BusIfGeneralTab* generalTab,
		LibraryInterface* libHandler, QSharedPointer<Component> component);
	
	//! \brief Pointer to the bus interface being edited.
	BusInterface* busif_;

	//! \brief Selector to select the interface mode type.
	QComboBox modeSelector_;

	//! \brief Contains the editors for different interface modes.
	QStackedWidget stack_;
};

#endif // BUSIFINTERFACEMODETAB_H
