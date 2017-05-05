//-----------------------------------------------------------------------------
// File: DesignConfigurationInstantiationEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 26.01.2016
//
// Description:
// Editor for design configuration instantiations.
//-----------------------------------------------------------------------------

#ifndef DESIGNCONFIGURATIONINSTANTIATIONEDITOR_H
#define DESIGNCONFIGURATIONINSTANTIATIONEDITOR_H

#include <editors/ComponentEditor/itemeditor.h>
#include <common/widgets/ParameterGroupBox/parametergroupbox.h>

class DesignConfigurationInstantiation;
class NameGroupEditor;
class VLNVEditor;
class ParameterFinder;
class ExpressionFormatter;

//-----------------------------------------------------------------------------
//! Editor for design configuration instantiations.
//-----------------------------------------------------------------------------
class DesignConfigurationInstantiationEditor : public ItemEditor
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] component               The component being edited.
     *      @param [in] instantiation	        The instantiation being edited.	
     *      @param [in] libHandler              The available IP-XACT library.
     *      @param [in] parameterFinder         Pointer to the parameter finder.
     *      @param [in] expressionFormatter     Pointer to the expression formatter.
     *      @param [in] parent                  The parent widget.
     */
	DesignConfigurationInstantiationEditor(QSharedPointer<Component> component, 
        QSharedPointer<DesignConfigurationInstantiation> instantiation,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
		LibraryInterface* libHandler,
        QWidget* parent);

    //! Destructor.
    virtual ~DesignConfigurationInstantiationEditor();

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
    DesignConfigurationInstantiationEditor(DesignConfigurationInstantiationEditor const& rhs);
    DesignConfigurationInstantiationEditor& operator=(DesignConfigurationInstantiationEditor const& rhs);

    //! Setup the layout.
    void setupLayout();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The design configuration instantiation being edited.
    QSharedPointer<DesignConfigurationInstantiation> instantiation_;

    //! Editor to set the name, display name and description of the design configuration instantiation.
    NameGroupEditor* nameGroupEditor_;

    //! VLNV editor for the design configuration reference.
    VLNVEditor* designConfigurationEditor_;

    //! Contains the parameters editor.
    ParameterGroupBox parameters_;
};

//-----------------------------------------------------------------------------

#endif // DESIGNCONFIGURATIONINSTANTIATIONEDITOR_H
