//-----------------------------------------------------------------------------
// File: ModeEditorBase.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 7.4.2011
//
// Description:
// Base class for different interface mode editors of a bus interface.
//-----------------------------------------------------------------------------

#ifndef MODE_EDITOR_BASE_H
#define MODE_EDITOR_BASE_H

#include <IPXACTmodels/generaldeclarations.h>

#include <QGroupBox>
#include <QSharedPointer>

class BusInterface;
class Component;
class BusInterfaceInterface;

//-----------------------------------------------------------------------------
//! Base class for different interface mode editors of a bus interface.
//-----------------------------------------------------------------------------
class ModeEditorBase : public QGroupBox
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
	 *      @param [in] busInterface    Interface for accessing bus interfaces.
     *      @param [in] busName         Name of the edited bus interface.
	 *      @param [in] title           The title for the group box
	 *      @param [in] parent          The owner of this editor.
     */
    ModeEditorBase(BusInterfaceInterface* busInterface, std::string const& busName, QString const& title,
        QWidget* parent);

	/*!
     *  The destructor
     */
	virtual ~ModeEditorBase() = default;


    //! No copying. No assignment.
    ModeEditorBase(const ModeEditorBase& other) = delete;
    ModeEditorBase& operator=(const ModeEditorBase& other) = delete;

	/*!
     *  Check for the validity of the mode specific items.
     *
     *      @return True if item is valid.
     */
	virtual bool isValid() const = 0;

	/*!
     *  Restore the changes made in the editor back to ones in the model.
     */
	virtual void refresh() = 0;

	/*!
     *  Get the interface mode of the editor.
	 * 
	 *      @return General::InterfaceMode Specifies the interface mode.
     */
	virtual General::InterfaceMode getInterfaceMode() const = 0;

	/*!
     *  Save the interface mode-specific details to the bus interface.
     */
	virtual void saveModeSpecific() = 0;

    /*!
     *  Change the name of the edited bus interface.
     *
     *      @param [in] newName     New name of the bus interface.
     */
    void changeName(std::string const& newName);

signals:

	//! Emitted when contents of the model change
	void contentChanged();

	//! Prints an error message to the user.
	void errorMessage(QString const& msg) const;

	//! Prints a notification to user.
	void noticeMessage(QString const& msg) const;

protected:

	/*!
     *  Get the interface for accessing bus interfaces.
     */
    BusInterfaceInterface* getBusInterface() const;

    /*!
     *  Get the name of the edited bus interfaces.
     */
    std::string getBusName() const;

    //-----------------------------------------------------------------------------
    //! Data.
    //-----------------------------------------------------------------------------

    //! Interface for accessing bus interfaces.
    BusInterfaceInterface* busInterface_;

    //! Name of the edited bus interface.
    std::string busName_;
};

#endif // MODE_EDITOR_BASE_H
