/* 
 *
 *  Created on: 5.4.2011
 *      Author: Antti Kamppi
 * 		filename: busifgeneraltab.h
 */

#ifndef BUSIFGENERALTAB_H
#define BUSIFGENERALTAB_H

#include <common/widgets/nameGroupEditor/namegroupeditor.h>
#include <common/widgets/vlnvEditor/vlnveditor.h>
#include <common/widgets/ParameterGroupBox/parametergroupbox.h>
#include <common/widgets/interfaceModeSelector/interfacemodeselector.h>
#include "busifgeneraldetails.h"
#include <models/component.h>
#include <models/businterface.h>
#include "interfacemodestack.h"

#include <QWidget>
#include <QSharedPointer>
#include <QStackedWidget>

class LibraryInterface;

/*! \brief Container for editor on the general tab of a bus interface editor.
 *
 */
class BusIfGeneralTab : public QWidget {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param libHandler Pointer to the instance that manages the library.
	 * \param busif Pointer to the bus interface being edited.
	 * \param component Pointer to the component being edited.
	 * \param parent Pointer to the owner of this editor.
	 *
	*/
	BusIfGeneralTab(LibraryInterface* libHandler,
		QSharedPointer<BusInterface> busif,
		QSharedPointer<Component> component,
		QWidget* parent);
	
	//! \brief The destructor
	virtual ~BusIfGeneralTab();

	/*! \brief Check for the validity of the edited item.
	*
	* \return True if item is valid.
	*/
	virtual bool isValid() const;

	/*! \brief Restore the changes made in the editor back to ones in the model.
	*
	*/
	virtual void refresh();

	/*! \brief Get the currently set bus type.
	 *
	 * \return VLNV that is set for the current bus type.
	*/
	VLNV getBusType() const;

	/*! \brief Get the currently set abstraction type.
	 *
	 * \return VLNV that is set for the current abstraction type.
	*/
	VLNV getAbsType() const;

signals:

	//! \brief Emitted when contents of the model change
	void contentChanged();

	//! \brief Prints an error message to the user.
	void errorMessage(const QString& msg) const;

	//! \brief Prints a notification to user.
	void noticeMessage(const QString& msg) const;

	//! Emitted when a help page should be changed in the context help window.
	void helpUrlRequested(QString const& url);

protected:

	//! \brief Handler for widget's show event
	virtual void showEvent(QShowEvent* event);

private slots:

	//! \brief Handler for changes in the bus type.
	void onBusTypeChanged();

	//! \brief Handler for changes in the abstraction type.
	void onAbsTypeChanged();

	//! \brief Handler for changes in interface mode.
	void onModeChanged(General::InterfaceMode mode);

private:

	//! \brief No copying
	BusIfGeneralTab(const BusIfGeneralTab& other);

	//! No assignment
	BusIfGeneralTab& operator=(const BusIfGeneralTab& other);
	
	//! \brief Pointer to the bus interface being edited.
	QSharedPointer<BusInterface> busif_;

	//! \brief Contains the name, display name and description of bus interface
	NameGroupEditor nameEditor_;

	//! \brief Contains the vlnv of the bus type for this bus interface
	VLNVEditor busType_;

	//! \brief Contains the vlnv of the abstraction type for this interface
	VLNVEditor absType_;

	//! \brief The selector to select the interface mode.
	InterfaceModeSelector modeSelector_;

	//! \brief Contains the mode-specific editors
	InterfaceModeStack modeStack_;

	//! \brief Contains the details of this bus interface
	BusIfGeneralDetails details_;

	//! \brief Contains the parameters of bus interface.
	ParameterGroupBox parameters_;

	//! \brief Pointer to the instance that manages the library
	LibraryInterface* libHandler_;
};

#endif // BUSIFGENERALTAB_H
