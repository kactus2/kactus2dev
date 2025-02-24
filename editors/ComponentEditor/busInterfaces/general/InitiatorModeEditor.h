//-----------------------------------------------------------------------------
// File: InitiatorModeEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 29.6.2023
//
// Description:
// Editor to edit initiator settings of a bus interface.
//-----------------------------------------------------------------------------

#ifndef INITIATOR_MODE_EDITOR_H
#define INITIATOR_MODE_EDITOR_H

#include "ModeEditorBase.h"

#include <editors/ComponentEditor/common/ReferenceSelector/ReferenceSelector.h>

#include <IPXACTmodels/generaldeclarations.h>

#include <QString>
#include <QSharedPointer>
#include <QLineEdit>

class BusInterface;
class Component;
class ExpressionEditor;
class ExpressionParser;
class InitiatorInterface;
class ParameterFinder;

//-----------------------------------------------------------------------------
//! Editor to edit initiator settings of a bus interface.
//-----------------------------------------------------------------------------
class InitiatorModeEditor : public ModeEditorBase
{
	Q_OBJECT

public:

    /*!
	 *  The constructor.
	 *
	 *    @param [in] busInterface        Interface for accessing bus interfaces.
     *    @param [in] busName             Name of the edited bus interface.
	 *    @param [in] component           The component being edited.
	 *    @param [in] parameterFinder     The parameter finder.
	 *    @param [in] expressionParser    The expression parser.
	 *    @param [in] parent              The owner of this editor.
	 */
    InitiatorModeEditor(
        BusInterfaceInterface* busInterface,
        std::string const& busName,
        QSharedPointer<Component> component,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionParser> expressionParser,
        QWidget *parent);

	//! The destructor
	virtual ~InitiatorModeEditor() = default;

    //! No copying. No assignment.
    InitiatorModeEditor(const InitiatorModeEditor& other) = delete;
    InitiatorModeEditor& operator=(const InitiatorModeEditor& other) = delete;

	/*! Check for the validity of the edited item.
	*
	*    @return True if item is valid.
	*/
	bool isValid() const final;

	/*! Restore the changes made in the editor back to ones in the model.
	*
	*/
	void refresh() final;

	/*! Get the interface mode of the editor
	 * 
	 *    @return General::InterfaceMode Specifies the interface mode.
	*/
	General::InterfaceMode getInterfaceMode() const final;

	//! Save the interface mode-specific details to the bus interface.
	void saveModeSpecific() final;

    /*!
     *  Remove the references from the expressions.
     */
    void removeReferencesFromExpressions();

signals:

    /*!
     *  Increase the number of references to the given parameter.
     *
     *    @param [in] id  The id of the target parameter.
     */
    void increaseReferences(QString const& id);

    /*!
     *  Decrease the number of references to the given parameter.
     *
     *    @param [in] id  The id of the target parameter.
     */
    void decreaseReferences(QString const& id);

private slots:

	/*!
	 *  Handler for changes in address space reference.
	 *
	 *    @param [in] addrSpaceName   The name of the referenced address space.
	 */
	void onAddressSpaceChange(QString const& addrSpaceName);

    /*!
     *  Handler for changes in base address.
     */
    void onBaseAddressChange();

private:

    //-----------------------------------------------------------------------------
    //! Data.
    //-----------------------------------------------------------------------------

	//! Combo box to select an address space within component
	ReferenceSelector addressSpaceReferenceSelector_;

    //! Editor for the base address of an address space.
    ExpressionEditor* baseAddressEditor_;

    //! The parameter finder.
    QSharedPointer<ParameterFinder> parameterFinder_;

    //! The containing component.
    QSharedPointer<Component> component_;
};

#endif // INITIATOR_MODE_EDITOR_H
