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

#include <editors/ComponentEditor/itemeditor.h>

#include <common/widgets/vlnvEditor/vlnveditor.h>
#include <common/widgets/nameGroupEditor/namegroupeditor.h>

#include <editors/ComponentEditor/software/PropertyValueEditor.h>

#include <QComboBox>
#include <QGroupBox>
#include <QSharedPointer>

class ComInterface;
class Component;
class LibraryInterface;

//-----------------------------------------------------------------------------
//! COM interface editor.
//-----------------------------------------------------------------------------
class ComInterfaceEditor : public ItemEditor
{
    Q_OBJECT

public:

	/*! The constructor
	 *
	 *      @param [in] libHandler      The instance that manages the library.
	 *      @param [in] component       The component being edited.
	 *      @param [in] comInterface    The COM interface being edited.
	 *      @param [in] parent          The parent widget.
	 *
	*/
	ComInterfaceEditor(LibraryInterface* libHandler, QSharedPointer<Component> component, 
		QSharedPointer<ComInterface> comInterface, QWidget *parent = 0);

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

	/*! Reload the information from the model to the editor.
	*/
	virtual void refresh();

protected:

	//! Handler for widget's show event
	virtual void showEvent(QShowEvent* event);
    
private slots:

    /*!
     *  Called when the COM definition has been changed.
     */
    void onComDefinitionChanged();

	//! Handler for changes in transfer type.
	void onTransferTypeChange();

	//! Handler for changes in direction.
	void onDirectionChange(int index);

	//! Handler for changes in properties editor.
	void onPropertiesChange();

	//! Handler for changes in COM implementation reference.
	void onComImplementationChange();

private:
    // Disable copying.
    ComInterfaceEditor(ComInterfaceEditor const& rhs);
    ComInterfaceEditor& operator=(ComInterfaceEditor const& rhs);
    
    /*!
     *  Updates the editors for showing values associated with the given COM definition.
     *
     *      @param [in] comDefinitionVLNV   The COM definition to update the editors for.
     */
    void updateEditorsForComDefinition(VLNV const& comDefinitionVLNV);
    
    //! Clears the editors when no COM definition has been set.
    void clearEditors();

    //! Sets the editor layout.
    void setupLayout();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The library handler.
    LibraryInterface* libInterface_;

    //! The COM interface being edited.
    ComInterface* comIf_;

    //! Contains the name, display name and description of the COM interface.
    NameGroupEditor nameEditor_;

    //! Editor for setting the COM definition.
    VLNVEditor* comTypeEditor_;

    //! Details group box.
    QGroupBox detailsGroup_;

    //! Data type combo box.
    QComboBox transferTypeCombo_;

    //! Direction combo box.
    QComboBox directionCombo_;

    //! The property value editor.
    PropertyValueEditor propertyValueEditor_;

	//! Editor to set the COM implementation reference.
	 VLNVEditor* comImplementation_;
};

//-----------------------------------------------------------------------------

#endif // COMINTERFACEEDITOR_H
