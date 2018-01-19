//-----------------------------------------------------------------------------
// File: PortWireTypeEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 24.11.2017
//
// Description:
// Editor for the port wire type definitions.
//-----------------------------------------------------------------------------

#ifndef PORTWIRETYPEEDITOR_H
#define PORTWIRETYPEEDITOR_H

#include <QScrollArea>

class Port;
class Component;

class PortTypeValidator;

class PortWireTypeModel;
class PortWireTypeDelegate;
class PortWireTypeView;

//-----------------------------------------------------------------------------
//! Editor for the port wire type definitions.
//-----------------------------------------------------------------------------
class PortWireTypeEditor : public QScrollArea
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
     *      @param [in] component       Component containing the edited port.
     *      @param [in] typeValidator   Validator for the port type definitions.
	 *      @param [in] parent          The owner of the editor.
	 */
    PortWireTypeEditor(QSharedPointer<Component> component, QSharedPointer<PortTypeValidator> typeValidator,
        QWidget* parent);
	
	/*!
     *  The destructor.
     */
    virtual ~PortWireTypeEditor();

    /*!
     *  Set a new component.
     *
     *      @param [in] newComponent    The selected component.
     */
    void setComponent(QSharedPointer<Component> newComponent);

    /*!
     *  Set a new port for the port wire type model.
     *
     *      @param [in] newPort     The selected port.
     */
    void setPortForModel(QSharedPointer<Port> newPort);

signals:

    /*!
     *  Signal for informing changes in content.
     */
    void contentChanged();

private:
	//! No copying. No assignment.
	PortWireTypeEditor(const PortWireTypeEditor& other);
	PortWireTypeEditor& operator=(const PortWireTypeEditor& other);

    /*!
     *  Connect the signals.
     */
    void connectSignals();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! View for the port type.
    PortWireTypeView* typeDefinitionView_;

    //! Model for the port type.
    PortWireTypeModel* typeDefinitionModel_;

    //! Delegate for the port type.
    PortWireTypeDelegate* typeDefinitionDelegate_;
};

#endif // PORTWIRETYPEEDITOR_H
