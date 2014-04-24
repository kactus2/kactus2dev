/* 
 *  	Created on: 26.6.2012
 *      Author: Antti Kamppi
 * 		filename: interfaceselector.h
 *		Project: Kactus 2
 */

#ifndef INTERFACESELECTOR_H
#define INTERFACESELECTOR_H

#include <IPXACTmodels/generaldeclarations.h>
#include <IPXACTmodels/component.h>

#include <QComboBox>
#include <QSharedPointer>

/*! \brief A combo box to select a bus interface within a component.
 *
 */
class InterfaceSelector : public QComboBox {
	Q_OBJECT

public:

	/*! \brief The constructor.
	 *
	 * \param component Pointer to the component that's interfaces are being selected.
	 * \param parent Pointer to the owner of this combo box.
	 * \param mode The mode that defines the types of interfaces to show.
	 *
	*/
	InterfaceSelector(QSharedPointer<Component> component,
		QWidget *parent,
		General::InterfaceMode mode = General::INTERFACE_MODE_COUNT);

	//! \brief The destructor
	virtual	~InterfaceSelector();

	/*! \brief Set the interface mode filter.
	 *
	 * \param mode The mode that defines the types of interfaces to show.
	 *
	 * Note: If the mode is undefined (General::INTERFACE_MODE_COUNT) then all
	 * types of interfaces are shown.
	*/
	void setInterfaceMode(General::InterfaceMode mode);

public slots:

	//! \brief Refresh the contents of the combo box.
	void refresh();

	/*! \brief Set the specified interface as selected.
	 *
	 * \param interfaceName The name of the interface to select.
	 *
	*/
	void selectInterface(const QString& interfaceName);

signals:

	//! \brief Emitted when interface was selected by user.
	void interfaceSelected(const QString& interfaceName);

private slots:

	//! \brief Handler for selected index changes.
	void onIndexChange(int newIndex);

private:
	
	//! \brief No copying
	InterfaceSelector(const InterfaceSelector& other);

	//! \brief No assignment
	InterfaceSelector& operator=(const InterfaceSelector& other);

	//! \brief The filter that is used to limit the possible options.
	General::InterfaceMode mode_;

	//! \brief Pointer to the component that's interfaces are selected.
	QSharedPointer<Component> component_;
};

#endif // INTERFACESELECTOR_H
