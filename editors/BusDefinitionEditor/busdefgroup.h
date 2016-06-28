//-----------------------------------------------------------------------------
// File: busdefgroup.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 21.6.2011
//
// Description:
// BusDefGroup contains elements to set the properties of a Bus Definition.
//-----------------------------------------------------------------------------

#ifndef BUSDEFGROUP_H
#define BUSDEFGROUP_H

#include <common/widgets/listManager/listmanager.h>

#include <IPXACTmodels/BusDefinition/BusDefinition.h>

#include <QGroupBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QSharedPointer>
#include <QPlainTextEdit>

//-----------------------------------------------------------------------------
//! BusDefGroup contains elements to set the properties of a Bus Definition.
//-----------------------------------------------------------------------------
class BusDefGroup : public QGroupBox
{
	Q_OBJECT

public:

	/*!
     *  The constructor
	 *
	 *      @param [in] parent The owner of this widget
	 */
	BusDefGroup(QWidget *parent);

	/*!
     *  Set the bus definition that is being edited
	 *
	 *      @param [in] busDef The bus definition
	 */
	void setBusDef(QSharedPointer<BusDefinition> busDef);
	
	//! The destructor.
	virtual ~BusDefGroup();

signals:

	//! Emitted when user changes the state of one of the elements.
	void contentChanged();

private slots:

	//! Handler for state changes on direct connection check box.
	void onDirectConnectionChanged(bool checked);

    //! Handler for state changes on is broadcast check box.
    void onIsBroadcastChanged(bool checked);

	//! Handler for state changes on is addressable check box.
	void onIsAddressableChanged(bool checked);

	//! Handler for changes in max masters line edit.
	void onMastersChanged();

	//! Handler for changes in max slaves line edit.
	void onSlavesChanged();

    //! Handler for changes in system group names.
    void onSystemNamesChanged();

    //! Handler for changes in the description.
    void onDescriptionChanged();

private:
	//! No copying
	BusDefGroup(const BusDefGroup& other);

	//! No assignment
	BusDefGroup& operator=(const BusDefGroup& other);

    //! Sets the widget layout.
    void setupLayout();
    
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
    ListManager systemgroupEditor_;

    //! Editor for bus definition description.
    QPlainTextEdit descriptionEditor_;
};

#endif // BUSDEFGROUP_H
