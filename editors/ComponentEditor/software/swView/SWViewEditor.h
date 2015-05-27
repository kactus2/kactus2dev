//-----------------------------------------------------------------------------
// File: SWViewEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 22.5.2012
//
// Description:
// SW view editor.
//-----------------------------------------------------------------------------

#ifndef SWVIEWEDITOR_H
#define SWVIEWEDITOR_H

#include <editors/ComponentEditor/itemeditor.h>
#include <editors/ComponentEditor/views/filesetrefeditor.h>

#include <common/widgets/nameGroupEditor/namegroupeditor.h>
#include <common/widgets/vlnvEditor/vlnveditor.h>

class SWView;
class SWBuildCommandEditor;
class BSPBuildEditor;

//-----------------------------------------------------------------------------
//! SW view editor.
//-----------------------------------------------------------------------------
class SWViewEditor : public ItemEditor
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] component    The component being edited.
     *      @param [in] swView		 The SW view being edited.
     *      @param [in] libHandler   The library handler.
     *      @param [in] parent       The parent widget.
     */
	SWViewEditor(QSharedPointer<Component> component,
		QSharedPointer<SWView> swView,
		LibraryInterface* libHandler, 
		QWidget *parent);

    /*!
     *  Destructor.
     */
    virtual ~SWViewEditor();

    /*!
     *  Checks for the validity of the edited API interface.
     *
	 *      @return True if the API interface is in valid state.
	 */
	bool isValid() const;

	/*! \brief Reload the information from the model to the editor.
	*/
	virtual void refresh();

protected:

	//! \brief Handler for widget's show event
	virtual void showEvent(QShowEvent* event);

private slots:

	//! \brief Handler for changes in hierarchy reference.
	void onHierRefChange();

	//! \brief Handler for changes in file set references.
	void onFileSetRefChange();

private:
    // Disable copying.
    SWViewEditor(SWViewEditor const& rhs);
    SWViewEditor& operator=(SWViewEditor const& rhs);

    /*!
     *  Setup the layout.
     */
    void setupLayout();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! \brief Pointer to the instance that manages the library.
    LibraryInterface* libHandler_;

    //! \brief Pointer to the view being edited.
    SWView* view_;

    //! \brief Editor to set the name, display name and description of the view.
    NameGroupEditor nameEditor_;

    //! VLNV editor for the hierarchy reference.
    VLNVEditor* hierRefEditor_;

	//! \brief Editor to set the file set references of the SW view.
	 FileSetRefEditor* fileSetRefEditor_;

	//! \brief Editor to set the SW build commands for the SW view.
	 SWBuildCommandEditor* swBuildCommands_;

	//! \brief Editor to set the BSP build commands for the SW view.
	 BSPBuildEditor* bspEditor_;
};

//-----------------------------------------------------------------------------

#endif // SWVIEWEDITOR_H
