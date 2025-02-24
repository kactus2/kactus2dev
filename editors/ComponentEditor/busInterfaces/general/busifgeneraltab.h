//-----------------------------------------------------------------------------
// File: busifgeneraltab.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 05.04.2011
//
// Description:
// Container for editor on the general tab of a bus interface editor.
//-----------------------------------------------------------------------------

#ifndef BUSIFGENERALTAB_H
#define BUSIFGENERALTAB_H

#include "busifgeneraldetails.h"
#include "interfacemodestack.h"

#include <editors/common/ExpressionSet.h>

#include <common/widgets/nameGroupEditor/namegroupeditor.h>
#include <common/widgets/vlnvEditor/vlnveditor.h>
#include <common/widgets/ParameterGroupBox/parametergroupbox.h>

#include <KactusAPI/include/ParameterFinder.h>
#include <KactusAPI/include/ExpressionFormatter.h>
#include <KactusAPI/include/ExpressionParser.h>

#include <QWidget>
#include <QSharedPointer>

class BusInterface;
class Component;
class LibraryInterface;
class AbstractionTypesEditor;
class BusInterfaceInterface;

//-----------------------------------------------------------------------------
//! Container for editor on the general tab of a bus interface editor.
//-----------------------------------------------------------------------------
class BusIfGeneralTab : public QWidget
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *    @param [in] libHandler              Pointer to the instance that manages the library.
	 *    @param [in] busif                   Pointer to the bus interface being edited.
	 *    @param [in] component               Pointer to the component being edited.
	 *    @param [in] parameterFinder         Pointer to the parameter finder.
	 *    @param [in] expressionFormatter     Pointer to the expression formatter.
     *    @param [in] expressionParser        Pointer to the expression parser.
     *    @param [in] busInterface            Interface for accessing bus interfaces.
     *    @param [in] busName                 Name of the edited bus interface.
	 *    @param [in] parent                  Pointer to the owner of this editor.
	 *    @param [in] parentWnd               Pointer to the parent window.
	 */
    BusIfGeneralTab(LibraryInterface* libHandler,
        QSharedPointer<BusInterface> busif,
        QSharedPointer<Component> component,
        ExpressionSet expressions,
        BusInterfaceInterface* busInterface,
        std::string const& busName,
        QWidget* parent,
        QWidget* parentWnd);

	/*!
     *  The destructor.
     */
	virtual ~BusIfGeneralTab() = default;

	/*!
     *  Restore the changes made in the editor back to ones in the model.
	 */
	virtual void refresh();

	/*!
     *  Get the currently set bus type.
	 *
	 *    @return VLNV that is set for the current bus type.
	 */
	VLNV getBusType() const;

    /*!
     *  Locks/unlocks the VLNV selection for bus definition and abstraction definition.
     *
     *    @param [in] locked   The lock to set.
     */
    void setBusTypesLock( bool locked );

signals:

	/*!
     *  Emitted when contents of the model change
     */
	void contentChanged();

	/*!
     *  Prints an error message to the user.
     */
	void errorMessage(const QString& msg) const;

	/*!
     *  Prints a notification to user.
     */
	void noticeMessage(const QString& msg) const;

	/*!
     *  Emitted when a help page should be changed in the context help window.
     */
	void helpUrlRequested(QString const& url);

    /*!
     *  Increase the amount of references to the parameter corresponding to the id.
     *
     *    @param [in] id      The id of the parameter being searched for.
     */
    void increaseReferences(QString id);

    /*!
     *  Decrease the amount of references to the parameter corresponding to the id.
     *
     *    @param [in] id      The id of the parameter being searched for.
     */
    void decreaseReferences(QString id);

    /*!
     *  Open the reference tree of the parameter corresponding to the ID.
     *
     *    @param [in] ID              The ID of the selected parameter.
     *    @param [in] parameterName   Name of the selected parameter.
     */
    void openReferenceTree(QString const& id, QString const& parameterName) const;

    /*!
     *  Recalculate references made to the selected parameters.
     *
     *    @param [in] parameterList       The selected parameters.
     *    @param [in] parameterInterface  Interface for accessing parameters.
     */
    void recalculateReferencesToParameters(QVector<QString> const& parameterList,
        AbstractParameterInterface* parameterInterface);

    /*!
     *  Inform of the name change in the edited bus interface.
     *
     *    @param [in] newName     New name of the bus interface.
     */
    void nameChanged(std::string const& newName);

public slots:

    /*!
     *  Handle the change in name of the edited bus interface.
     */
    void onNameChanged();

protected:

	/*!
     *  Handler for widget's show event
     */
	virtual void showEvent(QShowEvent* event);

private slots:

	/*!
     *  Handler for changes in the bus type.
     */
	void onBusTypeChanged();

	/*!
     *  Handler for changes in interface mode.
     */
	void onModeChanged(General::InterfaceMode mode);

	/*!
     *  Set the bus type for the bus interface.
	 *
	 *    @param [in] busDefVLNV The vlnv identifying the bus type.
	 */
	void onSetBusType(const VLNV& busDefVLNV);

	/*!
     *  Set the abstraction type for the bus interface.
	 *
	 *    @param [in] absDefVLNV The vlnv identifying the abstraction definition.
	 */
	void onSetAbsType(const VLNV& absDefVLNV);

private:

	//! No copying
	BusIfGeneralTab(const BusIfGeneralTab& other);

	//! No assignment
	BusIfGeneralTab& operator=(const BusIfGeneralTab& other);

    //! Sets the widget layout.
    void setupLayout();

    //-----------------------------------------------------------------------------
    //! Data.
    //-----------------------------------------------------------------------------

    //! Interface for accessing bus interfaces.
    BusInterfaceInterface* busInterface_;

	//! Contains the name, display name and description of bus interface.
	NameGroupEditor nameEditor_;

	//! Contains the vlnv of the bus type for this bus interface.
	VLNVEditor busType_;

	//! Contains the vlnv of the abstraction types for this interface.
    AbstractionTypesEditor* abstractionEditor_;

	//! Contains the mode-specific editors.
	InterfaceModeStack modeStack_;

	//! Contains the details of this bus interface.
	BusIfGeneralDetails details_;

	//! Contains the parameters of bus interface.
	ParameterGroupBox parameters_;

	//! Pointer to the instance that manages the library.
	LibraryInterface* libHandler_;

    //! The used IP-XACT revision.
    Document::Revision docRevision_;
};

#endif // BUSIFGENERALTAB_H
