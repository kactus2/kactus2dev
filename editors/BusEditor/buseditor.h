//-----------------------------------------------------------------------------
// File: buseditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 20.6.2011
//
// Description:
// Bus Editor is an editor for Bus- and Abstraction definitions.
//-----------------------------------------------------------------------------

#ifndef BUSEDITOR_H
#define BUSEDITOR_H

#include <common/widgets/tabDocument/TabDocument.h>

#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>
#include <IPXACTmodels/BusDefinition/BusDefinition.h>

#include <library/LibraryManager/libraryinterface.h>

#include "busdefgroup.h"
#include "absdefgroup.h"

#include <QSharedPointer>

class AbstractionDefinitionValidator;
class BusDefinitionValidator;
class ExpressionParser;
//-----------------------------------------------------------------------------
//! Bus Editor is editor for Bus- and Abstraction definition.
//-----------------------------------------------------------------------------
class BusEditor : public TabDocument
{
	Q_OBJECT

public:

	/*! The constructor
	 *
	 *      @param [in] parent          The owner of this widget.
	 *      @param [in] libHandler      The instance that manages library.
	 *      @param [in] busDef          The bus definition to edit.
	 *      @param [in] absDef          The abstraction definition to edit.
	 *      @param [in] disableBusDef If true then the BusDefGroup is disabled.
	*/
	BusEditor(QWidget *parent, LibraryInterface* libHandler,
        QSharedPointer<BusDefinition> busDef,
		QSharedPointer<AbstractionDefinition> absDef = QSharedPointer<AbstractionDefinition>(),
		bool disableBusDef = false);
	
	//! The destructor
	virtual ~BusEditor();

	/*! Get the VLNV that can be used to identify the Bus.
	 *
	 *      @return The VLNV that identifies the Bus.
	*/
	virtual VLNV getIdentifyingVLNV() const;

    //! Refreshes the editor.
    virtual void refresh();

    /*!
     *  Sets the protection state of the document.
     *
     *      @param [in] locked True for locked state; false for unlocked.
     */
    virtual void setProtection(bool locked);

	/*! Get the vlnv of the current bus definition.
	 *
	 *      @return VLNV of the bus definition/abstraction definition being edited.
	*/
	virtual VLNV getDocumentVLNV() const;

    /*! Sets the edited bus definition.
     * 
     *      @param [in] busDef   The bus definition to set. Must not be NULL.
     */
    virtual void setBusDef(QSharedPointer<BusDefinition> busDef);

    /*!
     *  Sets the edited abstraction definition.
     *
     *       @param [in] absDef   The abstraction definition to set.
     */
    virtual void setAbsDef(QSharedPointer<AbstractionDefinition> absDef);

public slots:
    /*!
     *  Validates the document against the IP-XACT standard.
     *
     *      @param [out] errorList Error message list for reporting standard violations.
     *
     *      @return True if the document is valid. False if there were any violations.
     */
    virtual bool validate(QVector<QString>& errorList);

	//! Saves the document and resets the modified state.
	virtual bool save();

	//! Saves the document as new object and resets modifies state
	virtual bool saveAs();

signals:

    //! Inform that a port abstraction has been renamed.
    void portRenamed(QString const& oldName, QString const& newName);

    //! Inform that a port abstraction has been removed.
    void portRemoved(QString const& portName, General::InterfaceMode const mode);

protected:
    //! Called when the editor is shown.
    virtual void showEvent(QShowEvent* event);

private:
	//! No copying
	BusEditor(const BusEditor& other);

	//! No assignment
	BusEditor& operator=(const BusEditor& other);

	//! Set the layout of the widget
	void setupLayout();

	//! The instance that handles the library
	LibraryInterface* libHandler_;

	//! The original bus definition to use when saving.
	QSharedPointer<BusDefinition> busDef_;

	//! The original abstraction definition to use when saving.
	QSharedPointer<AbstractionDefinition> absDef_;

	//! Group containing elements to edit bus definition
	BusDefGroup busDefGroup_;

	//! Group containing elements to edit abstraction definition.
	AbsDefGroup absDefGroup_;

    //! Parser for expressions in definitions.
    QSharedPointer<ExpressionParser> expressionParser_;

    //! Validator for bus definition.
    QSharedPointer<BusDefinitionValidator> busDefinitionValidator_;

    //! Validator for abstraction definition.
    QSharedPointer<AbstractionDefinitionValidator> absDefinitionValidator_;

};

#endif // BUSEDITOR_H
