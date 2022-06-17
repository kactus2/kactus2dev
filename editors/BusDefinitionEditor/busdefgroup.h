//-----------------------------------------------------------------------------
// File: busdefgroup.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 21.6.2011
//
// Description:
// BusDefGroup contains elements to set the properties of a Bus Definition.
//-----------------------------------------------------------------------------

#ifndef BUSDEFGROUP_H
#define BUSDEFGROUP_H

#include <common/widgets/listManager/listmanager.h>

#include <editors/BusDefinitionEditor/SystemGroupListEditor.h>

#include <IPXACTmodels/BusDefinition/BusDefinition.h>

#include <QGroupBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QSharedPointer>
#include <QPlainTextEdit>

class VLNVDisplayer;
class VLNVEditor;
class LibraryInterface;

//-----------------------------------------------------------------------------
//! BusDefGroup contains elements to set the properties of a Bus Definition.
//-----------------------------------------------------------------------------
class BusDefGroup : public QWidget
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
     *      @param [in] libraryHandler  Interface to the library.
	 *      @param [in] parent          The owner of this widget.
	 */
	BusDefGroup(LibraryInterface* libraryHandler, QWidget *parent);

    /*!
     *  The destructor.
     */
    virtual ~BusDefGroup() = default;

	/*!
     *  Set the bus definition that is being edited.
	 *
	 *      @param [in] busDef The bus definition
	 */
	void setBusDef(QSharedPointer<BusDefinition> busDef);

signals:

	/*!
     *  Emitted when user changes the state of one of the elements.
     */
	void contentChanged();

private slots:

	/*!
     *  Handler for state changes on direct connection check box.
     */
	void onDirectConnectionChanged(bool checked);

    /*!
     *  Handler for state changes on is broadcast check box.
     */
    void onIsBroadcastChanged(bool checked);

	/*!
     *  Handler for state changes on is addressable check box.
     */
	void onIsAddressableChanged(bool checked);

	/*!
     *  Handler for changes in max masters line edit.
     */
	void onMastersChanged();

	/*!
     *  Handler for changes in max slaves line edit.
     */
	void onSlavesChanged();

    /*!
     *  Handler for changes in system group names.
     */
    void onSystemNamesChanged();

    /*!
     *  Handler for changes in the description.
     */
    void onDescriptionChanged();

    /*!
     *  Handles the changes in bus definition extend.
     */
    void onExtendChanged();

private:
	//! No copying. No assignment.
	BusDefGroup(const BusDefGroup& other);
	BusDefGroup& operator=(const BusDefGroup& other);

    /*!
     *  Sets the widget layout.
     */
    void setupLayout();

    /*!
     *  Setup the extended bus definition.
     */
    void setupExtendedBus();

    /*!
     *  Get the extended bus definition for the selected bus definition.
     *
     *      @param [in] busDefinition   The selected bus definition.
     *
     *      @return The extended bus definition.
     */
    QSharedPointer<const BusDefinition> getExtendedBus(QSharedPointer<const BusDefinition> busDefinition) const;
    
    /*!
     *  Lock editors according to the selected extended bus definition.
     *
     *      @param [in] extendedBus     The selected extended bus definition.
     */
    void extendBusDefinition(QSharedPointer<const BusDefinition> extendedBus);

    /*!
     *  Remove extend system groups.
     */
    void removeSystemGroupsFromExtendedDefinition();

    /*!
     *  Remove the lock on editors caused by the previous extended bus definition.
     */
    void removeBusExtension();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------
    
    //! The library interface.
    LibraryInterface* library_;

    //! The bus definition to edit.
	QSharedPointer<BusDefinition> busDef_;

	//! Check box to set the direct connection option.
	QCheckBox directConnection_;

    //! Check box to set the broadcast option.
    QCheckBox isBroadcast_;

	//! Check box to set the is addressable option.
	QCheckBox isAddressable_;

	//! Editor to set the maximum number of masters.
	QLineEdit maxMastersEditor_;

	//! Editor to set the maximum number of slaves.
	QLineEdit maxSlavesEditor_;

    //! Editor for system group names.
    SystemGroupListEditor systemGroupEditor_;

    //! Editor for bus definition description.
    QPlainTextEdit descriptionEditor_;

    //! Display for the bus definition VLNV.
    VLNVDisplayer* vlnvDisplay_;

    //! Editor for the bus definition extension.
    VLNVEditor* extendEditor_;
};

#endif // BUSDEFGROUP_H
