/* 
 *  	Created on: 9.12.2011
 *      Author: Antti Kamppi
 * 		filename: channelinterfacemanagerdelegate.h
 *		Project: Kactus 2
 */

#ifndef CHANNELINTERFACEMANAGERDELEGATE_H
#define CHANNELINTERFACEMANAGERDELEGATE_H

#include <common/delegates/ComboDelegate/combodelegate.h>

#include <IPXACTmodels/component.h>

#include <QSharedPointer>

/*! \brief The delegate to provide editor to edit a list of channel interfaces.
 *
 */
class ChannelInterfaceManagerDelegate : public ComboDelegate {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this delegate
	 * \param component Pointer to the component that's interfaces are being selected.
	 *
	*/
	ChannelInterfaceManagerDelegate(QObject *parent, 
		QSharedPointer<Component> component);
	
	//! \brief The destructor
	virtual ~ChannelInterfaceManagerDelegate();

	/*! \brief Create a new editor for the given item
	 *
	 * \param parent Owner for the editor.
	 * \param option Contains options for the editor.
	 * \param index Model index identifying the item.
	 *
	 * \return Pointer to the editor to be used to edit the item.
	*/
	virtual QWidget* createEditor(QWidget* parent, 
		const QStyleOptionViewItem& option, 
		const QModelIndex& index) const;

private:
	//! \brief No copying
	ChannelInterfaceManagerDelegate(const ChannelInterfaceManagerDelegate& other);

	//! \brief No assignment
	ChannelInterfaceManagerDelegate& operator=(const ChannelInterfaceManagerDelegate& other);

	//! \brief Pointer to the component that's interfaces are being selected.
	QSharedPointer<Component> component_;
};

#endif // CHANNELINTERFACEMANAGERDELEGATE_H
