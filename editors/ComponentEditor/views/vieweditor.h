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
#include "flatviewgeneraltab.h"
#include "hierarchyrefwidget.h"

#include <common/widgets/nameGroupEditor/namegroupeditor.h>
#include <common/widgets/ParameterGroupBox/parametergroupbox.h>
#include <editors/ComponentEditor/fileBuilders/filebuilderseditor.h>
#include <editors/ComponentEditor/common/ParameterFinder.h>
#include <editors/ComponentEditor/common/ExpressionFormatter.h>

#include <IPXACTmodels/view.h>

#include <QComboBox>
#include <QStackedWidget>
#include <QTabWidget>
#include <QSharedPointer>

class Component;
class LibraryInterface;

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
	
	//! The destructor
	virtual ~ViewEditor();

	/*! Check for the validity of the edited view.
	*
	* \return True if all model parameters are in valid state.
	*/
	virtual bool isValid() const;

	/*! Reload the information from the model to the editor.
	*/
	virtual void refresh();

private slots:

	//! Handler for changes on the hierarchical/flat selector.
	void onStackChange(int index);

private:

	//! No copying
	ViewEditor(const ViewEditor& other);

	//! No assignment
	ViewEditor& operator=(const ViewEditor& other);

	//! Set up the layout for the editor.
	void setupLayout();

	//! Pointer to the instance that manages the library.
	LibraryInterface* libHandler_;

	//! Pointer to the view being edited.
	QSharedPointer<View> view_;
	
	//! Editor to set the name, display name and description of the view.
	NameGroupEditor nameEditor_;

	//! Combo box to select between hierarchical/flat views.
	QComboBox viewTypeSelector_;

	//! The editor to edit the envIdentifier element.
	EnvIdentifierEditor envIdentifier_;

	//! Widget that contains the flat and hierarchical editors.
	QStackedWidget typeDependentEditors_;

	//! Editor to set general settings of flat view.
	FlatViewGeneralTab generalTab_;

	//! Editor to set the parameters of flat view.
	ParameterGroupBox parameters_;

	//! Editor to set the default file builders of flat view.
	FileBuildersEditor fileBuilders_;

	//! The widget to edit the hierarchical reference
	HierarchyRefWidget hierarchyRef_;
};

#endif // VIEWEDITOR_H
