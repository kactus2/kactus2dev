/*
 *  Created on: 21.6.2011
 *      Author: Antti Kamppi
 * 		filename: signalsgroup.h
 */

#ifndef SIGNALSGROUP_H
#define SIGNALSGROUP_H

#include <QGroupBox>
#include <QPushButton>

/*! \brief Signals group contains two buttons for user to add new signals
 *
 */
class SignalsGroup : public QGroupBox {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this widget
	 *
	*/
	SignalsGroup(QWidget *parent);
	
	//! \brief The destructor
	virtual ~SignalsGroup();

signals:

	//! \brief Emitted when user presses "Add new signal"
	void addNewSignal();

	//! \brief Emitted when user presses "Add new signal options"
	void addNewSignalOptions();

private:
	//! \brief No copying
	SignalsGroup(const SignalsGroup& other);

	//! \brief No assignment
	SignalsGroup& operator=(const SignalsGroup& other);	

	//! \brief Button to add new signals
	QPushButton newSignalButton_;

	//! \brief Button to add new signal with options
	QPushButton newSignalOptionsButton_;
};

#endif // SIGNALSGROUP_H
