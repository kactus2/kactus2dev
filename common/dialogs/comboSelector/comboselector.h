/* 
 *
 *  Created on: 4.2.2011
 *      Author: Antti Kamppi
 * 		filename: comboselector.h
 */

#ifndef COMBOSELECTOR_H
#define COMBOSELECTOR_H

#include <common/Global.h>

#include <QInputDialog>
#include <QString>

class Component;

/*! \brief ComboSelector is a dialog that can be used to select an option.
 *
 * This dialog displays a list of options that user can choose from in a 
 * combo box. User can also define an own option and select it.
 */
class KACTUS2_API ComboSelector : public QInputDialog {
	Q_OBJECT

public:

	
	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this dialog.
	 * \param selected Defines the currently selected value in the combobox.
	 * \param f WindowFlags that define how the dialog is displayed.
	 *
	*/
	ComboSelector(QWidget *parent = 0, 
		const QString selected = QString(), 
		Qt::WindowFlags f = Qt::Dialog);

	//! \brief The destructor
	virtual ~ComboSelector();

	/*! \brief Execute the dialog
	 *
	 * Executes the dialog modally and asks user to select one option in the
	 * combobox.
	 *
	 * \return QString containing the input that user selected.
	*/
	QString execDialog();

	
	/*! \brief Static function that can be used to select a group type in FileSet
	 *
	 * \param parent Pointer to the owner of the dialog.
	 * \param selected A voluntary preselected value in the dialog.
	 * \param label The text that is used to ask user input.
	 *
	 * \return QString containing the selected value. Null string if cancel was
	 * pressed.
	*/
	static QString selectGroupType(QWidget* parent = 0, 
		const QString selected = QString(),
		const QString label = 
		tr("Select group type or create your own group type"));

	/*! \brief Static function that can be used to select a file type for File.
	 *
	 * \param parent Pointer to the owner of the dialog
	 * \param selected A voluntary preselected value in the dialog
	 * \param label The text that is used to ask user input.
	 *
	 * \return QString Containing the selected value. Null string if cancel was
	 * pressed.
	*/
	static QString selectFileType(QWidget* parent = 0,
		const QString selected = QString(),
		const QString label =
		tr("Select a file type or set your own file type"));

	/*! \brief Static function to select a bus interface name from component.
	 *
	 * \param component Pointer to the component that's bus interface is wanted.
	 * \param parent Pointer to the owner of the combo selector.
	 * \param selected The value that is selected when combo box is opened.
	 * \param label The text to be displayed to user when input is asked.
	 *
	 * \return QString containing the name of the selected bus interface.
	*/
	static QString selectBusInterface(QSharedPointer<Component> component,
		QWidget* parent = 0,
		const QString selected = QString(),
		const QString label =
		tr("Select a bus interface to include in the channel"));

	/*! \brief Static function to select a file set from component.
	 *
	 * \param component Pointer to the component that's file set is wanted.
	 * \param parent Pointer to the owner of the combo selector.
	 * \param selected The value that is selected when combo box is opened.
	 * \param label The text to be displayed to user when input is asked.
	 *
	 * \return QString containing the name of the selected file set.
	*/
	static QString selectFileSet(QSharedPointer<Component> component,
		QWidget* parent = 0,
		const QString selected = QString(),
		const QString label =
		tr("Select a file set"));

	/*! \brief Static function to select a view from component.
	 *
	 * \param component Pointer to the component that's view is selected.
	 * \param parent Pointer to the owner of combo selector.
	 * \param selected The value that is selected when combo box is opened.
	 * \param label The text to be displayed to user when input is asked.
	 *
	 * \return QString containing the name of the selected view.
	*/
	static QString selectView(QSharedPointer<Component> component,
		QWidget* parent = 0,
		const QString selected = QString(),
		const QString label = tr("Select a view"));

private:

	//! No copying
	ComboSelector(const ComboSelector& other);

	//! No assignment
	ComboSelector& operator=(const ComboSelector& other);
	
};

#endif // GROUPSELECTOR_H
