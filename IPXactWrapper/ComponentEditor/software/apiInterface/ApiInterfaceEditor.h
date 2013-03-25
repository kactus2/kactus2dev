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

#include <IPXactWrapper/ComponentEditor/itemeditor.h>

#include <common/widgets/vlnvEditor/vlnveditor.h>
#include <common/widgets/nameGroupEditor/namegroupeditor.h>

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

	/*! \brief Reload the information from the model to the editor.
	*/
	virtual void refresh();

private slots:

	//! \brief Handler for changes in api type.
	void onAPITypeChange();

	//! \brief Handler for changes in dependency editor.
	void onDependencyChange(int index);

protected:

	//! \brief Handler for widget's show event
	virtual void showEvent(QShowEvent* event);

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
    NameGroupEditor nameEditor_;

    //! Editor for setting the API definition.
    VLNVEditor* apiType_;

    //! Details group box.
    QGroupBox detailsGroup_;

    //! Dependency direction combo box.
    QComboBox dependencyCombo_;
};

//-----------------------------------------------------------------------------

#endif // APIINTERFACEEDITOR_H
