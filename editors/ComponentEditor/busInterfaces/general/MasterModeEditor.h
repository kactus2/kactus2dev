//-----------------------------------------------------------------------------
// File: MasterModeEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 7.4.2011
//
// Description:
// Editor to edit master or mirrored master settings of a bus interface.
//-----------------------------------------------------------------------------

#ifndef MASTER_MODE_EDITOR_H
#define MASTER_MODE_EDITOR_H

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
//! Editor to edit master or mirrored master settings of a bus interface.
//-----------------------------------------------------------------------------
class MasterModeEditor : public ModeEditorBase
{
	Q_OBJECT

public:

    /*!
	 *  The constructor.
	 *
	 *      @param [in] busInterface        Interface for accessing bud interfaces.
     *      @param [in] busName             Name of the edited bus interface.
	 *      @param [in] component           The component being edited.
	 *      @param [in] parameterFinder     The parameter finder.
	 *      @param [in] expressionParser    The expression parser.
	 *      @param [in] parent              The owner of this editor.
	 */
    MasterModeEditor(
        BusInterfaceInterface* busInterface,
        std::string const& busName,
        QSharedPointer<Component> component,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionParser> expressionParser,
        QWidget *parent);

	//! The destructor
	virtual ~MasterModeEditor() = default;

    //! No copying. No assignment.
    MasterModeEditor(const MasterModeEditor& other) = delete;
    MasterModeEditor& operator=(const MasterModeEditor& other) = delete;

	/*! Check for the validity of the edited item.
	*
	*       @return True if item is valid.
	*/
	bool isValid() const final;

	/*! Restore the changes made in the editor back to ones in the model.
	*
	*/
	void refresh() final;

	/*! Get the interface mode of the editor
	 * 
	 *      @return General::InterfaceMode Specifies the interface mode.
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
     *      @param [in] id  The id of the target parameter.
     */
    void increaseReferences(QString id);

    /*!
     *  Decrease the number of references to the given parameter.
     *
     *      @param [in] id  The id of the target parameter.
     */
    void decreaseReferences(QString id);

private slots:

	/*!
	 *  Handler for changes in address space reference.
	 *
	 *      @param [in] addrSpaceName   The name of the referenced address space.
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

#endif // MASTER_MODE_EDITOR_H
