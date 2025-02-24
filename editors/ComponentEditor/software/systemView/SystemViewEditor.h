//-----------------------------------------------------------------------------
// File: SystemViewEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti Maatta
// Date: 22.5.2012
//
// Description:
// System view editor.
//-----------------------------------------------------------------------------

#ifndef SYSTEMVIEWEDITOR_H
#define SYSTEMVIEWEDITOR_H

#include <editors/ComponentEditor/itemeditor.h>
#include <editors/ComponentEditor/instantiations/filesetrefeditor.h>

#include <common/widgets/nameGroupEditor/namegroupeditor.h>
#include <common/widgets/vlnvEditor/vlnveditor.h>

class SystemView;
class ViewSelector;

//-----------------------------------------------------------------------------
//! System view editor.
//-----------------------------------------------------------------------------
class SystemViewEditor : public ItemEditor
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *    @param [in] fileSetInterface    Interface for accessing file sets.
     *    @param [in] component           The component being edited.
     *    @param [in] systemView	        The system view being edited.
     *    @param [in] libHandler          The library handler.
     *    @param [in] parent              The parent widget.
     */
	SystemViewEditor(FileSetInterface* fileSetInterface, QSharedPointer<Component> component,
        QSharedPointer<SystemView> systemView, LibraryInterface* libHandler, QWidget* parent);

    //! The destructor.
    virtual ~SystemViewEditor();

    /*!
     *  Checks for the validity of the edited API interface.
     *
	 *    @return True if the API interface is in valid state.
	 */
	bool isValid() const;

	/*! Reload the information from the model to the editor.
	*/
	virtual void refresh();

protected:

	//! Handler for widget's show event
	virtual void showEvent(QShowEvent* event);

private slots:

	//! Handler for changes in hierarchy reference.
	void onHierRefChange();

	//! Handler for changes in file set references.
	void onFileSetRefChange();

	//! Handler for changes in HW view.
	void onHWViewChange(const QString& viewName);

private:
    // Disable copying.
    SystemViewEditor(SystemViewEditor const& rhs);
    SystemViewEditor& operator=(SystemViewEditor const& rhs);

    /*!
     *  Setup the layout.
     */
    void setupLayout();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Pointer to the instance that manages the library.
    LibraryInterface* libHandler_;

    //! Pointer to the view being edited.
    SystemView* view_;

    //! Editor to set the name, display name and description of the view.
    NameGroupEditor nameEditor_;

    //! VLNV editor for the hierarchy reference.
    VLNVEditor* hierRefEditor_;

	//! Editor to select the HW view.
	 ViewSelector* HWViewRefEditor_;

	 //! Editor to set the file set references of the system view.
	 FileSetRefEditor* fileSetRefEditor_;
};

//-----------------------------------------------------------------------------

#endif // SYSTEMVIEWEDITOR_H
