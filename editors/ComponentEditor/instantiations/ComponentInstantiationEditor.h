//-----------------------------------------------------------------------------
// File: ComponentInstantiationEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 26.10.2015
//
// Description:
// Contains the GUI items to edit the settings of a view component instance.
//-----------------------------------------------------------------------------

#ifndef FLATVIEWGENERALTAB_H
#define FLATVIEWGENERALTAB_H

#include "filesetrefeditor.h"
#include "ModuleParameterEditor.h"

#include <common/widgets/nameGroupEditor/namegroupeditor.h>

#include <editors/ComponentEditor/itemeditor.h>

#include <editors/ComponentEditor/fileBuilders/filebuilderseditor.h>

#include <QWidget>
#include <QLineEdit>
#include <QSharedPointer>
#include <QCheckBox>

class Component;
class View;
class ComponentInstantiation;
class ExpressionFormatter;
class ParameterFinder;
class ExpressionParser;

//-----------------------------------------------------------------------------
//! Contains the GUI items to edit the general settings of non-hierarchical view.
//-----------------------------------------------------------------------------
class ComponentInstantiationEditor : public ItemEditor
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
	 *      @param [in] component               The component that contains the view.
	 *      @param [in] view                    The view being edited.
     *      @param [in] parameterFinder         Parameter finder for view parameter expressions.
     *      @param [in] parameterFinder         Finder used to identify parameters.
     *      @param [in] expressionParser        Parser used to calculate expressions.
     *      @param [in] expressionFormatter     Formatter for view parameter expressions.
	 *      @param [in] parent                  The owner of this widget.
	 */
	ComponentInstantiationEditor(QSharedPointer<Component> component,
        LibraryInterface* library,
        QSharedPointer<ComponentInstantiation> componentInstantiation,
        QSharedPointer<ParameterFinder> parameterFinder, QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<ExpressionFormatter> expressionFormatter, QWidget *parent);
	
	//! The destructor.
	virtual ~ComponentInstantiationEditor();

	//! Restore the settings from the model to this editor.
	void refresh();

    //! Gets the component instance being edited.
    QSharedPointer<ComponentInstantiation> getComponentInstance() const;

protected:

	//! Handler for widget's show event
	virtual void showEvent(QShowEvent* event);

private slots:

	//! Handler for changes in language setting.
	void onLanguageChange();

    //! Handler for changes in library setting.
    void onLibraryChange();

    //! Handler for changes in package setting.
    void onPackageChange();

	//! Handler for changes in model name.
	void onModelNameChange();

    //! Handler for changes in package setting.
    void onArchitectureChange();

    //! Handler for changes in package setting.
    void onConfigurationChange();

	//! Handler for changes in file set references.
	void onFileSetRefChange();

private:
	
	//! No copying.
	ComponentInstantiationEditor(const ComponentInstantiationEditor& other);

	//! No assignment.
	ComponentInstantiationEditor& operator=(const ComponentInstantiationEditor& other);

    //! Sets the widget layout.
    void setupLayout();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! Pointer to the component being edited.
	QSharedPointer<Component> component_;

    //! Pointer to the edited component instantiation.
    QSharedPointer<ComponentInstantiation> componentInstantiation_;

    //! Editor for component instantiation name group.
    NameGroupEditor nameGroupEditor_;

	//! Editor to set the hardware description language.
	QLineEdit languageEditor_;

    //! Editor to set the compilation library.
    QLineEdit libraryEditor_;

    //! Editor to set the VHDL package for the model.
    QLineEdit packageEditor_;

	//! Set the strict definition to the language.
	QCheckBox languageStrict_;

	//! Editor to set the model name element value.
	QLineEdit modulelNameEditor_;

    //! Editor to set the VHDL architecture name for the model.
    QLineEdit architectureEditor_;

    //! Editor to set the configuration name for the model.
    QLineEdit configurationEditor_;

	//! Editor to set the file set references for the flat view.
	FileSetRefEditor fileSetRefs_;

    //! Editor to set the default file builders of flat view.
    FileBuildersEditor fileBuilders_;

    //! Editor to set the module parameters of flat view.
    ModuleParameterEditor moduleParameters_;
};

#endif // FLATVIEWGENERALTAB_H
