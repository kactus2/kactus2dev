/* 
 *  Created on: 21.6.2011
 *      Author: Antti Kamppi
 * 		filename: busdefgroup.h
 */

#ifndef BUSDEFGROUP_H
#define BUSDEFGROUP_H

#include <IPXACTmodels/busdefinition.h>

#include <QGroupBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QSharedPointer>
#include <QPlainTextEdit>

/*! \brief BusDefGroup contains elements to set the settings for the bus definition.
 *
 */
class BusDefGroup : public QGroupBox {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this widget
	 *
	*/
	BusDefGroup(QWidget *parent);

	/*! \brief Set the bus definition that is being edited
	 *
	 * \param busDef Pointer to the bus definition
	 *
	*/
	void setBusDef(QSharedPointer<BusDefinition> busDef);
	
	//! \brief The destructor
	virtual ~BusDefGroup();

signals:

	//! \brief Emitted when user changes the state of one of the elements.
	void contentChanged();

private slots:

	//! \brief Handler for state changes on direct connection check box
	void onDirectConnectionChanged(int state);

	//! \brief Handler for state changes on is addressable check box
	void onIsAddressableChanged(int state);

	//! \brief Handler for changes in max masters line edit.
	void onMastersChanged();

	//! \brief Handler for changes in max slaves line edit.
	void onSlavesChanged();

    void onDescriptionChanged();

private:
	//! \brief No copying
	BusDefGroup(const BusDefGroup& other);

	//! \brief No assignment
	BusDefGroup& operator=(const BusDefGroup& other);

    //! Sets the widget layout.
    void setupLayout();
    
	//! \brief Pointer to the bus definition to edit.
	QSharedPointer<BusDefinition> busDef_;

	//! \brief Check box to set the direct connection option
	QCheckBox directConnection_;

	//! \brief Check box to set the is addressable option
	QCheckBox isAddressable_;

	//! \brief Spin box to set the maximum number of masters
	QLineEdit maxMasters_;

	//! \brief Spin box to set the maximum number of slaves
	QLineEdit maxSlaves_;

    //! Editor for bus definition description.
    QPlainTextEdit descriptionEditor_;
};

#endif // BUSDEFGROUP_H
