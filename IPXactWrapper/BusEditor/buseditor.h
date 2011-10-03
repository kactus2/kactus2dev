/* 
 *  Created on: 20.6.2011
 *      Author: Antti Kamppi
 * 		filename: buseditor.h
 */

#ifndef BUSEDITOR_H
#define BUSEDITOR_H

#include <common/widgets/tabDocument/TabDocument.h>

#include <models/abstractiondefinition.h>
#include <models/busdefinition.h>

#include <LibraryManager/libraryinterface.h>

#include "busdefgroup.h"
#include "absdefgroup.h"

#include <QSharedPointer>

/*! \brief Bus Editor is editor for Bus- and Abstraction definition.
 * 
 * Bus Editor inherits TabDocument so it can be opened into a tab in kactus workspace.
 */
class BusEditor : public TabDocument {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this widget.
	 * \param libHandler Pointer to the instance that manages library.
	 * \param busDef Pointer to the bus definition to edit.
	 * \param absDef Pointer to the abstraction definition to edit.
	 * \param disableBusDef If true then the BusDefGroup is disabled so editing
	 * of bus definition is not possible.
	*/
	BusEditor(QWidget *parent, 
		LibraryInterface* libHandler,
		QSharedPointer<BusDefinition> busDef,
		QSharedPointer<AbstractionDefinition> absDef = QSharedPointer<AbstractionDefinition>(),
		bool disableBusDef = false);
	
	//! \brief The destructor
	virtual ~BusEditor();

    /*!
     *  Sets the protection state of the document.
     *
     *      @param [in] locked True for locked state; false for unlocked.
     */
    virtual void setProtection(bool locked);

	/*! \brief Get the vlnv of the current bus definition.
	 *
	 * \return VLNV of the bus definition/abstraction definition being edited.
	*/
	virtual VLNV getComponentVLNV() const;

public slots:

	//! Saves the document and resets the modified state.
	virtual bool save();

	//! \brief Saves the document as new object and resets modifies state
	virtual bool saveAs();

private:
	//! \brief No copying
	BusEditor(const BusEditor& other);

	//! \brief No assignment
	BusEditor& operator=(const BusEditor& other);

	//! \brief Set the layout of the widget
	void setupLayout();

	//! \brief Pointer to the instance that handles the library
	LibraryInterface* libHandler_;

	//! \brief Pointer to the original bus definition to use when saving.
	QSharedPointer<BusDefinition> busDef_;

	//! \brief Pointer to the original abstraction definition to use when saving.
	QSharedPointer<AbstractionDefinition> absDef_;

	//! \brief Pointer to the bus definition being edited.
	QSharedPointer<BusDefinition> editableBusDef_;

	//! \brief Pointer to the abstraction definition being edited.
	QSharedPointer<AbstractionDefinition> editableAbsDef_;

	//! \brief Group containing the signal buttons
	//SignalsGroup signalsGroup_;

	//! \brief Group containing the rename buttons
	//RenameGroup renameGroup_;

	//! \brief Group containing the import/export buttons
	//ImpExpGroup impExpGroup_;

	//! \brief Group containing elements to edit bus definition
	BusDefGroup busDefGroup_;

	//! \brief Group containing elements to edit abstraction definition.
	AbsDefGroup absDefGroup_;
};

#endif // BUSEDITOR_H
