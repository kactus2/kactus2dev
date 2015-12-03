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
#include "hierarchyrefwidget.h"

#include <common/widgets/nameGroupEditor/namegroupeditor.h>

#include <QSharedPointer>

class ExpressionFormatter;
class LibraryInterface;
class ParameterFinder;
class Component;
class View;
class ComponentInstantiation;
class DesignInstantiation;
class DesignConfigurationInstantiation;

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
     *  Check for the validity of the edited view.
	 *
	 *      @return True if all model parameters are in valid state.
	 */
	virtual bool isValid() const;

	/*!
     *  Reload the information from the model to the editor.
	 */
	virtual void refresh();

private slots:

    void onComponentInstanceChanged();

    void onHierarchyChanged();

   
private:

	//! No copying.
	ViewEditor(const ViewEditor& other);

	//! No assignment.
	ViewEditor& operator=(const ViewEditor& other);

    /*!
     *  Get the component instantiation referenced by the view.
     *
     *      @return The referenced component instantiation.
     */
    QSharedPointer<ComponentInstantiation> getComponentInstantiation() const;

    /*!
     *  Get the design instantiation referenced by the view.
     *
     *      @return The referenced design instantiation.
     */
    QSharedPointer<DesignInstantiation> getDesignInstantiation() const;

    /*!
     *  Get the design configuration instantiation referenced by the view.
     *
     *      @return The referenced design configuration instantiation.
     */
    QSharedPointer<DesignConfigurationInstantiation> getDesignConfigurationInstantiation() const;

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

	//! Editor to set general settings of flat view.
    ComponentInstantiationEditor* componentInstantiationEditor_;

	//! The widget to edit the hierarchical reference
	HierarchyRefWidget* hierarchyReferenceEditor_;
};

#endif // VIEWEDITOR_H
