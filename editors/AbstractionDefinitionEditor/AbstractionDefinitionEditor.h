//-----------------------------------------------------------------------------
// File: AbstractionDefinitionEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 20.6.2011
//
// Description:
// Bus defition editor is an editor for Abstraction definitions.
//-----------------------------------------------------------------------------

#ifndef ABSTRACTIONDEFINITIONEDITOR_H
#define ABSTRACTIONDEFINITIONEDITOR_H

#include <common/widgets/tabDocument/TabDocument.h>

#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>
#include <IPXACTmodels/BusDefinition/BusDefinition.h>

#include <KactusAPI/include/LibraryInterface.h>

#include "absdefgroup.h"

#include <QSharedPointer>

class AbstractionDefinitionValidator;
class BusDefinitionValidator;
class ExpressionParser;
class PortAbstractionInterface;

//-----------------------------------------------------------------------------
//! Bus Editor is editor for Abstraction definition.
//-----------------------------------------------------------------------------
class AbstractionDefinitionEditor : public TabDocument
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
	 *      @param [in] parent          The owner of this widget.
	 *      @param [in] libHandler      The instance that manages library.
	 *      @param [in] busDef          The bus definition to edit.
     *      @param [in] revision        Currently active IP-XACT revision.
     */
	AbstractionDefinitionEditor(QWidget *parent, LibraryInterface* libHandler, QSharedPointer<AbstractionDefinition> absDef, Document::Revision revision);
	
	/*!
     *  The destructor.
     */
	~AbstractionDefinitionEditor() override = default;

    //! No copying. No assignment.
    AbstractionDefinitionEditor(const AbstractionDefinitionEditor& other) = delete;
    AbstractionDefinitionEditor& operator=(const AbstractionDefinitionEditor& other) = delete;

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
     *  Sets the edited abstraction definition.
     * 
     *      @param [in] absDef   The bus definition to set. Must not be NULL.
     */
    void setAbsDef(QSharedPointer<AbstractionDefinition> absDef);


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

    /*!
     *  Create interface for accessing port abstractions.
     *
     *      @return Interface for accessing port abstractions.
     */
    PortAbstractionInterface* createPortAbstractionInterface();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! The original bus definition to use when saving.
	QSharedPointer<BusDefinition> busDef_;

	//! The original abstraction definition to use when saving.
	QSharedPointer<AbstractionDefinition> absDef_;

	//! Group containing elements to edit abstraction definition.
	AbsDefGroup absDefGroup_;

    //! Parser for expressions in definitions.
    QSharedPointer<ExpressionParser> expressionParser_;

    //! Validator for abstraction definition.
    QSharedPointer<AbstractionDefinitionValidator> absDefinitionValidator_;

};

#endif // ABSTRACTIONDEFINITIONEDITOR_H
