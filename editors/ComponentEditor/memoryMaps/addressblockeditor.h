//-----------------------------------------------------------------------------
// File: addressblockeditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
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
class RegisterInterface;
class RegisterBase;

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
     *      @param [in] registers           Registers for the interface.
     *      @param [in] registerInterface   Interface for registers.
	 *      @param [in] component           The component being edited.
	 *      @param [in] handler             The instance managing the library.
	 *      @param [in] parameterFinder     The parameter finder.
	 *      @param [in] parent              The parent of this editor.
	 */
    AddressBlockEditor(QSharedPointer<QList<QSharedPointer<RegisterBase> > > registers,
        RegisterInterface* registerInterface, QSharedPointer<Component> component,
        LibraryInterface* handler, QSharedPointer<ParameterFinder> parameterFinder, QWidget* parent = 0);

	//! The destructor.
	virtual ~AddressBlockEditor() = default;

    //! No copying.
    AddressBlockEditor(const AddressBlockEditor& other) = delete;

    //! No assignment.
    AddressBlockEditor& operator=(const AddressBlockEditor& other) = delete;

	/*!
     *  Reload the information from the model to the editor.
	 */
	virtual void refresh();

signals:

    /*
     *  Informs of register name change.
     *
     *      @param [in] oldName     The old name.
     *      @param [in] newName     The new name.
     */
    void registerNameChanged(QString const& oldName, QString const& newName);

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
    void graphicsChanged(int);

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

    void childAddressingChanged(int index);

private:
	

	//! The view to display the items.
    EditableTableView* view_;

	//! The model that manages the details of address block.
	AddressBlockModel* model_;

    //! Interface for accessing registers.
    RegisterInterface* interface_;

    //! Pointer to the registers for the interface.
    QSharedPointer<QList<QSharedPointer<RegisterBase> > > registers_;
};

#endif // ADDRESSBLOCKEDITOR_H
