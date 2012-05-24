//-----------------------------------------------------------------------------
// File: SystemChangeCommands.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 19.10.2011
//
// Description:
// Undo change commands for system designs.
//-----------------------------------------------------------------------------

#ifndef SYSTEMCHANGECOMMANDS_H
#define SYSTEMCHANGECOMMANDS_H

#include <LibraryManager/vlnv.h>

#include <QMap>
#include <QUndoCommand>

class ComponentItem;
class SWComponentItem;
class SWCompItem;
class SWConnectionEndpoint;
class VLNV;

//-----------------------------------------------------------------------------
//! SWComponentPacketizeCommand class.
//-----------------------------------------------------------------------------
class SWComponentPacketizeCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] component  The component item.
     *      @param [in] vlnv       The VLNV of the packetized component.
     *      @param [in] parent     The parent command.
     */
    SWComponentPacketizeCommand(ComponentItem* component, VLNV const& vlnv, QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~SWComponentPacketizeCommand();

    /*!
     *  Undoes the command.
     */
    virtual void undo();

    /*!
     *  Redoes the command.
     */
    virtual void redo();

private:
    // Disable copying.
    SWComponentPacketizeCommand(SWComponentPacketizeCommand const& rhs);
    SWComponentPacketizeCommand& operator=(SWComponentPacketizeCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The software component.
    ComponentItem* component_;

    //! The component VLNV.
    VLNV vlnv_;
};

//-----------------------------------------------------------------------------
//! TypeDefinitionChangeCommand class.
//-----------------------------------------------------------------------------
class TypeDefinitionChangeCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] endPoint          The SW connection end point.
     *      @param [in] oldBusType        The endpoint's old API/COM type.
     *      @param [in] parent            The parent command.
     */
    TypeDefinitionChangeCommand(SWConnectionEndpoint* endpoint, VLNV const& oldType,
                                QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~TypeDefinitionChangeCommand();

    /*!
     *  Undoes the command.
     */
    virtual void undo();

    /*!
     *  Redoes the command.
     */
    virtual void redo();

private:
    // Disable copying.
    TypeDefinitionChangeCommand(TypeDefinitionChangeCommand const& rhs);
    TypeDefinitionChangeCommand& operator=(TypeDefinitionChangeCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The diagram connection endpoint.
    SWConnectionEndpoint* endpoint_;

    //! The endpoint's old COM/API type.
    VLNV oldType_;

    //! The endpoint's new COM/API type.
    VLNV newType_;
};

//-----------------------------------------------------------------------------
//! PropertyValuesChangeCommand class.
//-----------------------------------------------------------------------------
class PropertyValuesChangeCommand : public QUndoCommand {

public:

	/*!
     *  Constructor.
     *
	 *      @param [in] component        Pointer to the component instance that is being edited.
	 *      @param [in] newConfElements  The new property values for the instance.
	 *      @param [in] parent           Pointer to the owner of this command.
	 *
	*/
	PropertyValuesChangeCommand(SWComponentItem* component,
		                        QMap<QString, QString> const& newPropertyValues, 
		                        QUndoCommand* parent = 0);

	/*!
     *  Destructor.
     */
	~PropertyValuesChangeCommand();

	/*!
     *  Undoes the command.
     */
    virtual void undo();

    /*!
     *  Redoes the command.
     */
    virtual void redo();

private:
	// Disable copying.
	PropertyValuesChangeCommand(PropertyValuesChangeCommand const& rhs);
	PropertyValuesChangeCommand& operator=(PropertyValuesChangeCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! Pointer to the component instance.
	SWComponentItem* component_;

	//! The old property values.
	QMap<QString, QString> oldPropertyValues_;

	//! The new property values..
	QMap<QString, QString> newPropertyValues_;
};

//-----------------------------------------------------------------------------
//! FileSetRefChangeCommand class.
//-----------------------------------------------------------------------------
class FileSetRefChangeCommand : public QUndoCommand
{
public:

	/*!
     *  Constructor.
     *
	 *      @param [in] component        Pointer to the component instance that is being edited.
	 *      @param [in] newConfElements  The new file set reference for the instance.
	 *      @param [in] parent           Pointer to the owner of this command.
	 *
	*/
	FileSetRefChangeCommand(SWCompItem* component, QString const& newFileSetRef, 
		                    QUndoCommand* parent = 0);

	/*!
     *  Destructor.
     */
	~FileSetRefChangeCommand();

	/*!
     *  Undoes the command.
     */
    virtual void undo();

    /*!
     *  Redoes the command.
     */
    virtual void redo();

private:
	// Disable copying.
	FileSetRefChangeCommand(FileSetRefChangeCommand const& rhs);
	FileSetRefChangeCommand& operator=(FileSetRefChangeCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! Pointer to the component instance.
	SWCompItem* component_;

	//! The old file set reference.
    QString oldFileSetRef_;

    //! The new file set reference.
    QString newFileSetRef_;
};

//-----------------------------------------------------------------------------

#endif // SYSTEMCHANGECOMMANDS_H
