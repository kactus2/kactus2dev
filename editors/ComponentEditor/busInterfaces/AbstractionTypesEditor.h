//-----------------------------------------------------------------------------
// File: AbstractionTypesEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 15.01.2018
//
// Description:
// Editor for the bus interface abstraction definitions.
//-----------------------------------------------------------------------------

#ifndef ABSTRACTIONTYPESEDITOR_H
#define ABSTRACTIONTYPESEDITOR_H

#include <QWidget>
#include <QGroupBox>

class AbstractionTypesModel;
class AbstractionTypesDelegate;
class EditableTableView;
class BusInterface;
class Component;
class ConfigurableVLNVReference;
class LibraryInterface;
class VLNV;
class AbstractionTypeInterface;

//-----------------------------------------------------------------------------
//! Editor for the bus interface abstraction definitions.
//-----------------------------------------------------------------------------
class AbstractionTypesEditor : public QGroupBox
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
     *      @param [in] component                   Component containing the edited port.
     *      @param [in] library                     Interface for the VLNV library.
     *      @param [in] abstractionTypeInterface    Interface for accessing abstraction types.
     *      @param [in] parentWindow                The main window.
	 *      @param [in] parent                      The owner of the editor.
	 */
    AbstractionTypesEditor(QSharedPointer<Component> component,
        LibraryInterface* library,
        AbstractionTypeInterface* abstractionTypeInterface,
        QWidget* parentWindow,
        QWidget* parent);

	/*!
     *  The destructor.
     */
    virtual ~AbstractionTypesEditor();

    /*!
     *  Set a new component.
     *
     *      @param [in] newComponent    The selected component.
     */
    void setComponent(QSharedPointer<Component> newComponent);

    /*!
     *  Set a new bus interface for the abstraction types model.
     */
    void setBusForModel();

    /*!
     *  Add a new abstraction definition referencing the selected VLNV to the bus interface.
     *
     *      @param [in] newAbstraction  The selected VLNV.
     */
    void addNewAbstraction(VLNV const& newAbstraction);

signals:

    /*!
     *  Signal for informing changes in content.
     */
    void contentChanged();

private:
	//! No copying. No assignment.
    AbstractionTypesEditor(const AbstractionTypesEditor& other);
    AbstractionTypesEditor& operator=(const AbstractionTypesEditor& other);

    /*!
     *  Connect the signals.
     */
    void connectSignals();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Interface for accessing abstraction types.
    AbstractionTypeInterface* abstractionTypeInterface_;

    //! The abstraction types view.
    EditableTableView* abstractionView_;

    //! The model containing data of the abstraction types.
    AbstractionTypesModel* abstractionModel_;

    //! Delegate for handling the abstraction types data.
    AbstractionTypesDelegate* abstractionDelegate_;

    //! Interface for the VLNV library.
    LibraryInterface* library_;
};

#endif // ABSTRACTIONTYPESEDITOR_H
