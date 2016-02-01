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

#include <QSharedPointer>
#include <QGroupBox>

class Component;
class ComponentInstantiation;
class ComponentInstantiationDisplayer;
class DesignInstantiation;
class DesignConfigurationInstantiation;
class EnvIdentifierEditor;
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
	 *      @param [in] component               The component that contains the view.
	 *      @param [in] view                    The view being edited.
	 *      @param [in] libHandler              The instance that manages the library.
	 *      @param [in] parameterFinder         The parameter finder.
	 *      @param [in] expressionFormatter     The expression formatter.
	 *      @param [in] parent                  The owner of this editor.
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

    //! Called when the widget is shown.
    void showEvent(QShowEvent* event);

private slots:

    //! Called when a component instance is selected.
    void onComponentInstanceChanged(QString const& instanceName);

    //! Called when a design configuration instance is selected.
    void onDesignConfigurationInstanceChanged(QString const& instanceName);

    //! Called when a design instance is selected.
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

	//! The instance that manages the library.
	LibraryInterface* libHandler_;

	//! The view being edited.
	QSharedPointer<View> view_;
	
	//! Editor to set the name, display name and description of the view.
	NameGroupEditor* nameEditor_;

	//! The editor to edit the envIdentifier element.
	EnvIdentifierEditor* envIdentifier_;

    //! Selector for component instantiation reference.
    ReferenceSelector* componentInstantiationSelector_;
    
    //! Selector for design configuration instantiation reference.
    ReferenceSelector* designConfigurationInstantiationSelector_;
    
    //! Selector for design instantiation reference.
    ReferenceSelector* designInstantiationSelector_;

    //! Display widget for component instantiation details.
    ComponentInstantiationDisplayer* componentInstantiationDisplay_;

    //! Group box for design and design configuration VLNV references.
    QGroupBox* hierarchyGroup_;

    //! Display widget for design configuration instantiation VLNV.
    VLNVDisplayer* designConfigurationDisplay_;

    //! Display widget for design instantiation VLNV.
    VLNVDisplayer* designDisplay_;

    //! The display for module parameters of the component instance.
    ModuleParameterEditor* moduleParameterEditor_;
};

#endif // VIEWEDITOR_H
