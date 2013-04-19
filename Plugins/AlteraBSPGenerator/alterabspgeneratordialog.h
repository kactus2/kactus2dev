/* 
 *	Created on:	19.4.2013
 *	Author:		Antti Kamppi
 *	File name:	alterabspgeneratordialog.h
 *	Project:		Kactus 2
*/

#ifndef ALTERABSPGENERATORDIALOG_H
#define ALTERABSPGENERATORDIALOG_H

#include <common/Global.h>

#include <common/dialogs/commandLineGeneratorDialog/commandlinegeneratordialog.h>
#include <models/component.h>

#include <QSharedPointer>
#include <QDialog>

/*! \brief The dialog to generate the BSP for a CPU component.
 *
 */
class AlteraBSPGeneratorDialog : public CommandLineGeneratorDialog {
	Q_OBJECT

public:

	/*! \brief The constructor.
	 *
	 * Method: 		AlteraBSPGeneratorDialog
	 * Full name:	AlteraBSPGeneratorDialog::AlteraBSPGeneratorDialog
	 * Access:		private 
	 *
	 * \param component Pointer to the component to generate BSP for.
	 * \param parent Pointer to the owner of the dialog.
	 *
	*/
	AlteraBSPGeneratorDialog(QSharedPointer<Component> component, QWidget *parent);
	
	//! \brief The destructor.
	virtual ~AlteraBSPGeneratorDialog();

private:
	
	//! \brief No copying
	AlteraBSPGeneratorDialog(const AlteraBSPGeneratorDialog& other);

	//! \brief No assignment
	AlteraBSPGeneratorDialog& operator=(const AlteraBSPGeneratorDialog& other);

	//! \brief Pointer to the component to generate the BSP for.
	QSharedPointer<Component> component_;
};

#endif // ALTERABSPGENERATORDIALOG_H
