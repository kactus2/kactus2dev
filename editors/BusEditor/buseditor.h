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

#include <library/LibraryManager/libraryinterface.h>

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

	/*! \brief Get the VLNV that can be used to identify the Bus.
	 *
	 * \return The VLNV that identifies the Bus.
	*/
	virtual VLNV getIdentifyingVLNV() const;

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
	virtual VLNV getDocumentVLNV() const;

    /*! \brief Sets the edited bus definition.
     * 
     * \param busDef   The bus definition to set. Must not be NULL.
     */
    virtual void setBusDef(QSharedPointer<BusDefinition> busDef);

    /*!
     *  Sets the edited abstraction definition.
     *
     *  \param absDef   The absraction definition to set.
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
    virtual bool validate(QStringList& errorList);

	//! Saves the document and resets the modified state.
	virtual bool save();

	//! \brief Saves the document as new object and resets modifies state
	virtual bool saveAs();

signals:

    //! \brief Inform that a port abstraction has been renamed.
    void portRenamed(const QString& oldName, const QString& newName);

    //! \brief Inform that a port abstraction has been removed.
    void portRemoved(const QString& portName, const General::InterfaceMode mode);

protected:
    //! Called when the editor is shown.
    virtual void showEvent(QShowEvent* event);

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

	//! \brief Group containing elements to edit bus definition
	BusDefGroup busDefGroup_;

	//! \brief Group containing elements to edit abstraction definition.
	AbsDefGroup absDefGroup_;
};

#endif // BUSEDITOR_H
