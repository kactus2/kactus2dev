//-----------------------------------------------------------------------------
// File: DesignInstantiationEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 26.01.2016
//
// Description:
// Editor for design configuration instantiation.
//-----------------------------------------------------------------------------

#ifndef DESIGNINSTANTIATIONEDITOR_H
#define DESIGNINSTANTIATIONEDITOR_H

#include <editors/ComponentEditor/itemeditor.h>

#include <common/widgets/vlnvEditor/vlnveditor.h>

class DesignInstantiation;
class NameGroupEditor;

//-----------------------------------------------------------------------------
//! Editor for design configuration instantiation.
//-----------------------------------------------------------------------------
class DesignInstantiationEditor : public ItemEditor
{
    Q_OBJECT

public:

    /*!
     *  Constructor.
     *
     *      @param [in] component       The component being edited.
     *      @param [in] instantiation	The instantiation being edited.	
     *      @param [in] libHandler      The available IP-XACT library.
     *      @param [in] parent          The parent widget.
     */
	DesignInstantiationEditor(QSharedPointer<Component> component, 
        QSharedPointer<DesignInstantiation> instantiation,
		LibraryInterface* libHandler, QWidget* parent);

    //! Destructor.
    virtual ~DesignInstantiationEditor();

	//! Reload the information from the model to the editor.
	virtual void refresh();

protected:

	//! Handler for widget's show event.
	virtual void showEvent(QShowEvent* event);

private slots:

	//! Handler for changes in design configuration reference.
	void onHierRefChange();

private:
    // Disable copying.
    DesignInstantiationEditor(DesignInstantiationEditor const& rhs);
    DesignInstantiationEditor& operator=(DesignInstantiationEditor const& rhs);

    //! Setup the layout.
    void setupLayout();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The design instantiation being edited.
    QSharedPointer<DesignInstantiation> instantiation_;

    //! Editor to set the name, display name and description of the view.
    NameGroupEditor* nameGroupEditor_;

    //! VLNV editor for the hierarchy reference.
    VLNVEditor* designEditor_;
};

//-----------------------------------------------------------------------------

#endif // DESIGNINSTANTIATIONEDITOR_H
