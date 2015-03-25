/* 
 *
 *  Created on: 15.4.2011
 *      Author: Antti Kamppi
 * 		filename: flatviewgeneraltab.h
 */

#ifndef FLATVIEWGENERALTAB_H
#define FLATVIEWGENERALTAB_H

#include "filesetrefeditor.h"
#include "ModuleParameterEditor.h"

#include <common/widgets/ParameterGroupBox/parametergroupbox.h>

#include <editors/ComponentEditor/fileBuilders/filebuilderseditor.h>

#include <IPXACTmodels/view.h>

#include <QWidget>
#include <QLineEdit>
#include <QSharedPointer>
#include <QCheckBox>

class Component;
class ExpressionFormatter;
class ParameterFinder;

//-----------------------------------------------------------------------------
//! Contains the GUI items to edit the general settings of non-hierarchical view.
//-----------------------------------------------------------------------------
class FlatViewGeneralTab : public QWidget
{
	Q_OBJECT

public:

	/*! The constructor
	 *
	 *        @param [in] component             The component that contains the view.
	 *        @param [in] view                  The view being edited.
     *        @param [in] parameterFinder       Parameter finder for view parameter expressions.
     *        @param [in] expressionFormatter   Formatter for view parameter expressions.
	 *        @param [in] parent                The owner of this widget.
	 *
	*/
	FlatViewGeneralTab(QSharedPointer<Component> component, 
        QSharedPointer<View> view, QSharedPointer<ParameterFinder> parameterFinder, 
        QSharedPointer<ExpressionFormatter> expressionFormatter,
		QWidget *parent);
	
	//! The destructor
	virtual ~FlatViewGeneralTab();

	/*! Check for the validity of the edited view
	*
	*       @return True if all model parameters are in valid state.
	*/
	virtual bool isValid() const;

	//! Restore the settings from the model to this editor.
	void refresh();

signals:

	//! Emit an error message to user.
	void errorMessage(const QString& msg);

	//! Emit a notice to user.
	void noticeMessage(const QString& msg);

	//! Emitted when contents of the editor change
	void contentChanged();

	//! Emitted when a help page should be changed in the context help window.
	void helpUrlRequested(const QString& url);
    
    /*!
     *  Increase the amount of references to a parameter with a matching id.
     *
     *      @param [in] id      Id of the parameter, whose references are being increased.
     */
    void increaseReferences(QString id);

    /*!
     *  Decrease the amount of references to a parameter with a matching id.
     *
     *      @param [in] id      Id of the parameter, whose references are being increased.
     */
    void decreaseReferences(QString id);

    /*!
     *  Open the reference tree of the selected parameter.
     *
     *      @param [in] id      Id of the selected parameter.
     */
    void openReferenceTree(QString const& id) const;

protected:

	//! Handler for widget's show event
	virtual void showEvent(QShowEvent* event);

private slots:

	//! Handler for changes in language setting.
	void onLanguageChange();

	//! Handler for changes in model name.
	void onModelNameChange(const QString& newName);

	//! Handler for changes in file set references.
	void onFileSetRefChange();

private:
	
	//! No copying
	FlatViewGeneralTab(const FlatViewGeneralTab& other);

	//! No assignment
	FlatViewGeneralTab& operator=(const FlatViewGeneralTab& other);

    //! Sets the widget layout.
    void setupLayout();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! Pointer to the component being edited.
	QSharedPointer<Component> component_;

	//! Pointer to the view being edited.
	QSharedPointer<View> view_;

	//! Editor to set the hardware description language.
	QLineEdit language_;

	//! Set the strict definition to the language.
	QCheckBox languageStrict_;

	//! Editor to set the model name element value.
	QLineEdit modelName_;

	//! Editor to set the file set references for the flat view.
	FileSetRefEditor fileSetRefs_;

    //! Editor to set the default file builders of flat view.
    FileBuildersEditor fileBuilders_;

    //! Editor to set the module parameters of flat view.
    ModuleParameterEditor moduleParameters_;

    //! Editor to set the parameters of flat view.
    ParameterGroupBox parameters_;
};

#endif // FLATVIEWGENERALTAB_H
