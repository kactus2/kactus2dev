/* 
 *  	Created on: 11.7.2011
 *      Author: Antti Kamppi
 * 		filename: firmnessgroup.h
 *		Project: Kactus 2
 */

#ifndef IMPLEMENTATIONGROUP_H
#define IMPLEMENTATIONGROUP_H

#include <common/utils.h>

#include <QGroupBox>
#include <QCheckBox>

/*! \brief Contains check boxes to set the firmness options for VLNVDialer.
 *
 */
class ImplementationGroup : public QGroupBox {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this widget.
	 *
	*/
	ImplementationGroup(QWidget *parent);
	
	//! \brief The destructor
	virtual ~ImplementationGroup();

signals:

	//! \brief Emitted when firmness options changes state.
	void optionsChanged(const Utils::ImplementationOptions& options);

private slots:

	//! \brief Handler for hwBox state changes.
	void onHWChanged(int state);

	//! \brief Handler for swBox state changes.
	void onSWChanged(int state);

	//! \brief Handler for systemBox state changes.
	void onSystemChanged(int state);

private:
	//! \brief No copying
	ImplementationGroup(const ImplementationGroup& other);

	//! \brief No assignment
	ImplementationGroup& operator=(const ImplementationGroup& other);

	//! \brief Check box to select hardware components in/out of search results.
	QCheckBox hwBox_;

	//! \brief Check box to select software component in/out of search results.
	QCheckBox swBox_;

	//! \brief Check box to select system components in/out of search results.
	QCheckBox systemBox_;

	//! \brief Contains the current search settings.
	Utils::ImplementationOptions options_;
};

#endif // IMPLEMENTATIONGROUP_H
