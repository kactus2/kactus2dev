//-----------------------------------------------------------------------------
// File: MirroredSlaveModeEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 7.4.2011
//
// Description:
// Editor to edit mirrored slave details of a bus interface.
//-----------------------------------------------------------------------------

#ifndef MIRRORED_SLAVE_MODE_EDITOR_H
#define MIRRORED_SLAVE_MODE_EDITOR_H

#include "ModeEditorBase.h"

#include <KactusAPI/include/ParameterFinder.h>

#include <QSharedPointer>
#include <QLineEdit>

class BusInterface;
class Component;
class ExpressionEditor;
class ExpressionParser;
class MirroredSlaveInterface;
class BusInterfaceInterface;

//-----------------------------------------------------------------------------
//! Editor to edit mirrored slave details of a bus interface.
//-----------------------------------------------------------------------------
class MirroredSlaveModeEditor : public ModeEditorBase
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *    @param [in] busInterface        Interface for accessing bus interfaces.
     *    @param [in] busName             Name of the edited bus interface.
	 *    @param [in] parameterFinder     The parameter finder.
     *    @param [in] expressionParser    The expression parser.
	 *    @param [in] parent              The owner of this editor.
	 */
    MirroredSlaveModeEditor(BusInterfaceInterface* busInterface,
        std::string const& busName,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionParser> expressionParser,
        QWidget *parent);

	/*!
     *  The destructor.
     */
    virtual ~MirroredSlaveModeEditor() = default;

    //! No copying. No assignment.
    MirroredSlaveModeEditor(const MirroredSlaveModeEditor& other) = delete;
    MirroredSlaveModeEditor& operator=(const MirroredSlaveModeEditor& other) = delete;

	/*!
     *  Check for the validity of the edited item.
     *
     *    @return True if item is valid.
     */
	bool isValid() const final;

	/*!
     *  Restore the changes made in the editor back to ones in the model.
     *
     */
	void refresh() final;

	/*!
     *  Get the interface mode of the editor.
	 * 
	 *    @return General::InterfaceMode Specifies the interface mode.
     */
	General::InterfaceMode getInterfaceMode() const final;

	/*!
     *  Save the interface mode-specific details to the bus interface.
     */
	void saveModeSpecific() final;

    /*!
     *  Remove all the references from the expressions contained within this interface.
     */
    void removeReferencesFromExpressions();

private slots:

    /*!
     *  Handler for changes in remap address.
     */
    void onRemapChange();

    /*!
     *  Handler for changes in range.
     */
    void onRangeChange();

signals:

    /*!
     *  Increase the number of references in the selected parameter.
     *
     *    @param [in] id      The id of the selected parameter.
     */
    void increaseReferences(QString const& id);

    /*!
     *  Decrease the number of references in the selected parameter.
     *
     *    @param [in] id      The id of the selected parameter.
     */
    void decreaseReferences(QString const& id);

private:

    //-----------------------------------------------------------------------------
    //! Data.
    //-----------------------------------------------------------------------------

    //! Editor for the mirrored slave remap address.
    ExpressionEditor* remapEditor_;

    //! Editor for the mirrored slave range.
    ExpressionEditor* rangeEditor_;

    //! The expression parser for calculating the expression.
    QSharedPointer<ExpressionParser> expressionParser_;

    //! The parameter finder for searching the component parameters.
    QSharedPointer<ParameterFinder> parameterFinder_;
};

#endif // MIRRORED_SLAVE_MODE_EDITOR_H
