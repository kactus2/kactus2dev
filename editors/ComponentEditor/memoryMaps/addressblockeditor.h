//-----------------------------------------------------------------------------
// File: addressblockeditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 24.08.2012
//
// Description:
// Editor for editing the details of registers in an address block.
//-----------------------------------------------------------------------------

#ifndef ADDRESSBLOCKEDITOR_H
#define ADDRESSBLOCKEDITOR_H

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/AddressBlock.h>

#include <QSharedPointer>
#include <QGroupBox>

class AddressBlockModel;
class EditableTableView;
class ExpressionFormatter;
class LibraryInterface;
class ParameterFinder;

class RegisterValidator;
class ColumnFreezableTable;

//-----------------------------------------------------------------------------
//! Editor for editing the details of registers in an address block.
//-----------------------------------------------------------------------------
class AddressBlockEditor : public QGroupBox
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] addressBlock            Pointer to the address block being edited.
	 *      @param [in] component               Pointer to the component being edited.
	 *      @param [in] handler                 Pointer to the instance managing the library.
	 *      @param [in] parameterFinder         Pointer to the parameter finder.
	 *      @param [in] expressionFormatter     Pointer to the expression formatter.
     *      @param [in] registerValidator       Validator used for registers.
	 *      @param [in] parent                  Pointer to the parent of this editor.
	 */
	AddressBlockEditor(QSharedPointer<AddressBlock> addressBlock,
		QSharedPointer<Component> component,
		LibraryInterface* handler,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        QSharedPointer<RegisterValidator> registerValidator,
		QWidget* parent = 0);

	//! The destructor.
	virtual ~AddressBlockEditor();

	/*!
     *  Reload the information from the model to the editor.
	 */
	virtual void refresh();

signals:

    /*!
     *  Change the value for address unit bits in the model.
     *
     *      @param [in] newAddressUnitBits  The new address unit bits value.
     */
    void addressUnitBitsChanged(int newAddressUnitBits);

    /*!
     *  Informs that the contents of the editor have changed.
     */
    void contentChanged();

    /*!
     *  Informs of a need to redraw the visualizer.
     */
    void graphicsChanged();

    /*!
     *  Sends an error message forward.
     *
     *      @param [in] msg     The error message.
     */
    void errorMessage(const QString& msg) const;

    /*!
     *  Sends a notification message forward.
     *
     *      @param [in] msg     The notification message.
     */
    void noticeMessage(const QString& msg) const;

    /*!
     *  Increase the amount of references made to the given parameter.
     *
     *      @param [in] id  The id of the given parameter.
     */
    void increaseReferences(QString id) const;

    /*!
     *  Decrease the amount of references made to the given parameter.
     *
     *      @param [in] id  The id the given parameter.
     */
    void decreaseReferences(QString id) const;

    /*!
     *  Informs that a new item has been created.
     *
     *      @param [in] index   The index of the new item.
     */
    void childAdded(int index);

    /*!
     *  Informs that an item has been removed.
     *
     *      @param [in] index   The index of the removed item.
     */
    void childRemoved(int index);

private:
	
	//! No copying.
	AddressBlockEditor(const AddressBlockEditor& other);

	//! No assignment.
	AddressBlockEditor& operator=(const AddressBlockEditor& other);

	//! The view to display the items.
    ColumnFreezableTable* view_;

	//! Pointer to the model that manages the details of address block.
	AddressBlockModel* model_;
};

#endif // ADDRESSBLOCKEDITOR_H
