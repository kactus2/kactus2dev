//-----------------------------------------------------------------------------
// File: BusDefinitionEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 20.6.2011
//
// Description:
// Bus defition editor is an editor for Bus- and Abstraction definitions.
//-----------------------------------------------------------------------------

#ifndef BUSDEFINITIONEDITOR_H
#define BUSDEFINITIONEDITOR_H

#include <common/widgets/tabDocument/TabDocument.h>

#include <IPXACTmodels/BusDefinition/BusDefinition.h>

#include <KactusAPI/include/LibraryInterface.h>

#include <editors/ComponentEditor/referenceCounter/ParameterReferenceCounter.h>

#include "busdefgroup.h"

#include <QSharedPointer>

class BusDefinitionValidator;
class ExpressionParser;
class ExpressionFormatter;
class BusDefinitionParameterReferenceTree;
class ParameterReferenceTreeWindow;
class ListParameterFinder;

//-----------------------------------------------------------------------------
//! Bus Editor is editor for Bus- and Abstraction definition.
//-----------------------------------------------------------------------------
class BusDefinitionEditor : public TabDocument
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
	 *      @param [in] parent          The owner of this widget.
	 *      @param [in] libHandler      The instance that manages library.
	 *      @param [in] busDef          The bus definition to edit.
     */
	BusDefinitionEditor(QWidget *parent, LibraryInterface* libHandler, QSharedPointer<BusDefinition> busDef);
	
	/*!
     *  The destructor.
     */
	~BusDefinitionEditor() override = default;

    //! No copying. No assignment.
    BusDefinitionEditor(const BusDefinitionEditor& other) = delete;
    BusDefinitionEditor& operator=(const BusDefinitionEditor& other) = delete;

	/*!
     *  Get the VLNV that can be used to identify the Bus.
	 *
	 *      @return The VLNV that identifies the Bus.
     */
	VLNV getIdentifyingVLNV() const override;

    /*!
     *  Refreshes the editor.
     */
    void refresh() override;

    /*!
     *  Sets the protection state of the document.
     *
     *      @param [in] locked True for locked state; false for unlocked.
     */
    void setProtection(bool locked) override;

	/*!
     *  Get the vlnv of the current bus definition.
	 *
	 *      @return VLNV of the bus definition/abstraction definition being edited.
	 */
	VLNV getDocumentVLNV() const override;

    /*!
     *  Sets the edited bus definition.
     * 
     *      @param [in] busDef   The bus definition to set. Must not be NULL.
     */
    void setBusDef(QSharedPointer<BusDefinition> busDef);


public slots:

    /*!
     *  Validates the document against the IP-XACT standard.
     *
     *      @param [out] errorList Error message list for reporting standard violations.
     *
     *      @return True if the document is valid. False if there were any violations.
     */
    virtual bool validate(QVector<QString>& errorList);

	/*!
     *  Saves the document and resets the modified state.
     */
	virtual bool save();

	/*!
     *  Saves the document as new object and resets modifies state
     */
	virtual bool saveAs();

signals:

    /*!
     *  Inform that a port abstraction has been removed.
     *
     *      @param [in] portName    Name of the removed port abstraction.
     *      @param [in] mode        Mode of the removed port abstraction.
     */
    void portRemoved(QString const& portName, General::InterfaceMode const mode);

protected:
    
    /*!
     *  Called when the editor is shown.
     *
     *      @param [in] event   The show event.
     */
    virtual void showEvent(QShowEvent* event);

private:

	/*!
     *  Set the layout of the widget
     */
	void setupLayout();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! The original bus definition to use when saving.
	QSharedPointer<BusDefinition> busDef_ = nullptr;

    //! Parser for expressions in definitions.
    QSharedPointer<ExpressionParser> expressionParser_ = nullptr;

    //! Validator for bus definition.
    QSharedPointer<BusDefinitionValidator> busDefinitionValidator_ =nullptr;

    //! Flag for save status on bus definition.
    bool busDefinitionSaved_= false;

    //! Parameter finder for bus definitions.
    QSharedPointer<ListParameterFinder> busDefParameterFinder_;
    
    //! Expression formatter to use.
    QSharedPointer<ExpressionFormatter> expressionFormatter_;

    //! Reference counter for the bus definition parameter references.
    QSharedPointer<ParameterReferenceCounter> referenceCounter_;
    
    //! The parameter reference tree to use.
    BusDefinitionParameterReferenceTree* parameterReferenceTree_;

    //! The parameter reference window.
    ParameterReferenceTreeWindow* referenceTreeWindow_;

    //! Group containing elements to edit bus definition
	BusDefGroup busDefGroup_;
};

#endif // BUSDEFINITIONEDITOR_H
