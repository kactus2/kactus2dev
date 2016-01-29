//-----------------------------------------------------------------------------
// File: vieweditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 14.4.2011
//
// Description:
// Editor to edit a view within a component.
//-----------------------------------------------------------------------------

#ifndef VIEWEDITOR_H
#define VIEWEDITOR_H

#include <editors/ComponentEditor/itemeditor.h>

#include "envidentifiereditor.h"
#include "ComponentInstantiationEditor.h"

#include <QSharedPointer>

class Component;
class ComponentInstantiation;
class ComponentInstantiationDisplayer;
class DesignInstantiation;
class DesignConfigurationInstantiation;
class ExpressionFormatter;
class LibraryInterface;
class NameGroupEditor;
class ModuleParameterEditor;
class ParameterFinder;
class ReferenceSelector;
class View;
class VLNVDisplayer;
//-----------------------------------------------------------------------------
//! Editor to edit a view within a component.
//-----------------------------------------------------------------------------
class ViewEditor : public ItemEditor
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] component               Pointer to the component that contains the view.
	 *      @param [in] view                    Pointer to the view being edited.
	 *      @param [in] libHandler              Pointer to the instance that manages the library.
	 *      @param [in] parameterFinder         Pointer to the parameter finder.
	 *      @param [in] expressionFormatter     Pointer to the expression formatter.
	 *      @param [in] parent                  Pointer to the owner of this editor.
	 */
	ViewEditor(QSharedPointer<Component> component,
        QSharedPointer<View> view,
        LibraryInterface* libHandler,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        QWidget *parent = 0);
	
	//! The destructor.
	virtual ~ViewEditor();

	/*!
     *  Reload the information from the model to the editor.
	 */
	virtual void refresh();

protected:

    void showEvent(QShowEvent* event);

private slots:

    void onComponentInstanceChanged(QString const& instanceName);

    void onDesignConfigurationInstanceChanged(QString const& instanceName);

    void onDesignInstanceChanged(QString const& instanceName);

private:

	//! No copying.
	ViewEditor(const ViewEditor& other);

	//! No assignment.
	ViewEditor& operator=(const ViewEditor& other);

	//! Set up the layout for the editor.
	void setupLayout();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! Pointer to the instance that manages the library.
	LibraryInterface* libHandler_;

	//! Pointer to the view being edited.
	QSharedPointer<View> view_;
	
	//! Editor to set the name, display name and description of the view.
	NameGroupEditor nameEditor_;

	//! The editor to edit the envIdentifier element.
	EnvIdentifierEditor envIdentifier_;


    ReferenceSelector* componentInstantiationSelector_;
    
    ReferenceSelector* designConfigurationInstantiationSelector_;
    
    ReferenceSelector* designInstantiationSelector_;

    ComponentInstantiationDisplayer* componentInstantiationDisplay_;

    QGroupBox* hierarchyGroup_;

    VLNVDisplayer* designConfigurationDisplay_;

    VLNVDisplayer* designDisplay_;

    //! The display for module parameters of the component instance.
    ModuleParameterEditor* moduleParameterEditor_;
};

#endif // VIEWEDITOR_H
