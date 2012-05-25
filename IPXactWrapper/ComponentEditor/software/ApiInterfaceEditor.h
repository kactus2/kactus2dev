//-----------------------------------------------------------------------------
// File: ApiInterfaceEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 16.4.2012
//
// Description:
// API interface editor for the component editor.
//-----------------------------------------------------------------------------

#ifndef APIINTERFACEEDITOR_H
#define APIINTERFACEEDITOR_H

#include "../itemeditor.h"

#include <common/widgets/vlnvEditor/vlnveditor.h>
#include <common/widgets/nameGroupBox/namegroupbox.h>

#include <QComboBox>
#include <QGroupBox>

class Component;
class ApiInterface;
class LibraryInterface;

//-----------------------------------------------------------------------------
//! API interface editor.
//-----------------------------------------------------------------------------
class ApiInterfaceEditor : public ItemEditor
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] libHandler   The library handler.
     *      @param [in] component    The component being edited.
     *      @param [in] APIInterface The API interface being edited.
     *      @param [in] parent       The parent widget.
	 */
	ApiInterfaceEditor(LibraryInterface* libHandler,
		QSharedPointer<Component> component, 
		QSharedPointer<ApiInterface> APIInterface,
		QWidget *parent);

    /*!
     *  Destructor.
     */
    virtual ~ApiInterfaceEditor();

    /*!
     *  Checks for the validity of the edited API interface.
     *
	 *      @return True if the API interface is in valid state.
	 */
	bool isValid() const;

    /*!
     *  Removes the edited API interface from the model.
	 *
	 *      @remarks This function should be used when user wants to remove an element from the component.
	 */
	virtual void removeModel();

	/*!
     *  Applies the changes from the widgets editors to the IP-Xact model.
     */
	void makeChanges();

private:
    // Disable copying.
    ApiInterfaceEditor(ApiInterfaceEditor const& rhs);
    ApiInterfaceEditor& operator=(ApiInterfaceEditor const& rhs);

    /*!
     *  Restores changes made to the contents.
     */
    void restoreChanges();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The library handler.
    LibraryInterface* libInterface_;

    //! Pointer to the bus interface being edited.
    ApiInterface* apiIf_;

    //! Contains the name, display name and description of the API interface.
    NameGroupBox nameGroup_;

    //! Editor for setting the API definition.
    VLNVEditor apiType_;

    //! Details group box.
    QGroupBox detailsGroup_;

    //! Dependency direction combo box.
    QComboBox dependencyCombo_;
};

//-----------------------------------------------------------------------------

#endif // APIINTERFACEEDITOR_H
