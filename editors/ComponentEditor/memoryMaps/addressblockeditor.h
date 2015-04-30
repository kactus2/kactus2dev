/* 
 *  	Created on: 24.8.2012
 *      Author: Antti Kamppi
 * 		filename: addressblockeditor.h
 *		Project: Kactus 2
 */

#ifndef ADDRESSBLOCKEDITOR_H
#define ADDRESSBLOCKEDITOR_H

#include <IPXACTmodels/addressblock.h>
#include <IPXACTmodels/component.h>

#include <QSortFilterProxyModel>
#include <QSharedPointer>
#include <QGroupBox>

class AddressBlockModel;
class AddressBlockProxy;
class EditableTableView;
class ExpressionFormatter;
class LibraryInterface;
class ParameterFinder;

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
	 *      @param [in] parent                  Pointer to the parent of this editor.
	 */
	AddressBlockEditor(QSharedPointer<AddressBlock> addressBlock,
		QSharedPointer<Component> component,
		LibraryInterface* handler,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
		QWidget* parent = 0);

	//! The destructor
	virtual ~AddressBlockEditor();

	/*! Check for the validity of the editor
	* 
	*       @return True if the editor is in valid state.
	*/
	virtual bool isValid() const;

	/*! Reload the information from the model to the editor.
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
	
	//! No copying
	AddressBlockEditor(const AddressBlockEditor& other);

	//! No assignment
	AddressBlockEditor& operator=(const AddressBlockEditor& other);

	//! The view to display the items.
	EditableTableView* view_;

	//! The proxy that does the sorting of items.
	AddressBlockProxy* proxy_;

	//! Pointer to the model that manages the details of address block.
	AddressBlockModel* model_;
};

#endif // ADDRESSBLOCKEDITOR_H
