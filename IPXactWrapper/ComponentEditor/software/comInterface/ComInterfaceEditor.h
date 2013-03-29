//-----------------------------------------------------------------------------
// File: ComInterfaceEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 16.4.2012
//
// Description:
// COM interface editor for the component editor.
//-----------------------------------------------------------------------------

#ifndef COMINTERFACEEDITOR_H
#define COMINTERFACEEDITOR_H

#include <IPXactWrapper/ComponentEditor/software/PropertyValueEditor.h>
#include <IPXactWrapper/ComponentEditor/itemeditor.h>

#include <common/widgets/vlnvEditor/vlnveditor.h>
#include <common/widgets/nameGroupEditor/namegroupeditor.h>
#include <models/ComInterface.h>
#include <models/component.h>

#include <QComboBox>
#include <QGroupBox>
#include <QSharedPointer>

class LibraryInterface;

//-----------------------------------------------------------------------------
//! COM interface editor.
//-----------------------------------------------------------------------------
class ComInterfaceEditor : public ItemEditor
{
    Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param libHandler Pointer to the instance that manages the library.
	 * \param component Pointer to component being edited.
	 * \param comInterface Pointer to COM interface being edited.
	 * \param parent The parent widget.
	 *
	*/
	ComInterfaceEditor(LibraryInterface* libHandler, 
		QSharedPointer<Component> component, 
		QSharedPointer<ComInterface> comInterface,
		QWidget *parent = 0);

    /*!
     *  Destructor.
     */
    virtual ~ComInterfaceEditor();

    /*!
     *  Checks for the validity of the edited COM interface.
     *
	 *      @return True if the COM interface is in valid state.
	 */
	bool isValid() const;

	/*! \brief Reload the information from the model to the editor.
	*/
	virtual void refresh();

protected:

	//! \brief Handler for widget's show event
	virtual void showEvent(QShowEvent* event);
    
private slots:

    /*!
     *  Called when the COM definition has been changed.
     */
    void onComDefinitionChanged();

	//! \brief Handler for changes in transfer type.
	void onTransferTypeChange();

	//! \brief Handler for changes in direction.
	void onDirectionChange(int index);

	//! \brief Handler for changes in properties editor.
	void onPropertiesChange();

	//! \brief Handler for changes in COM implementation reference.
	void onComImplementationChange();

private:
    // Disable copying.
    ComInterfaceEditor(ComInterfaceEditor const& rhs);
    ComInterfaceEditor& operator=(ComInterfaceEditor const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The library handler.
    LibraryInterface* libInterface_;

    //! Pointer to the bus interface being edited.
    ComInterface* comIf_;

    //! Contains the name, display name and description of the COM interface.
    NameGroupEditor nameEditor_;

    //! Editor for setting the COM definition.
    VLNVEditor* comType_;

    //! Details group box.
    QGroupBox detailsGroup_;

    //! Data type combo box.
    QComboBox transferTypeCombo_;

    //! Direction combo box.
    QComboBox directionCombo_;

    //! The property value editor.
    PropertyValueEditor propertyValueEditor_;

	//! \brief Editor to set the COM implementation reference.
	 VLNVEditor* comImplementation_;
};

//-----------------------------------------------------------------------------

#endif // COMINTERFACEEDITOR_H
