//-----------------------------------------------------------------------------
// File: registereditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 24.08.2012
//
// Description:
// Editor for editing the details of fields in a register.
//-----------------------------------------------------------------------------

#ifndef REGISTEREDITOR_H
#define REGISTEREDITOR_H

#include <IPXACTmodels/Component/Component.h>

#include <QSharedPointer>
#include <QGroupBox>

class RegisterTableModel;
class LibraryInterface;
class ParameterFinder;
class EditableTableView;
class ExpressionFormatter;
class FieldValidator;
class FieldInterface;
class Field;

//-----------------------------------------------------------------------------
//! Editor for editing the details of fields in a register.
//-----------------------------------------------------------------------------
class RegisterEditor : public QGroupBox
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
     *    @param [in] fields              Pointer to the available fields.
	 *    @param [in] component           Pointer to the component being edited.
	 *    @param [in] handler             Pointer to the instance that manages the library.
	 *    @param [in] parameterFinder     Pointer to the parameter finder.
     *    @param [in] fieldInterface      Interface for fields.
	 *    @param [in] parent              Pointer to the parent of this editor.
	 */
    RegisterEditor(QSharedPointer<QList<QSharedPointer<Field> > > fields,
        QSharedPointer<Component> component,
        LibraryInterface* handler,
        QSharedPointer<ParameterFinder> parameterFinder,
        FieldInterface* fieldInterface,
        QWidget* parent = 0);

	/*!
     *  The destructor.
     */
	virtual ~RegisterEditor() = default;

	/*!
     *  Reload the information from the model to the editor.
	 */
	virtual void refresh();

signals:
    
    /*
     *  Informs of field name change.
     *
     *    @param [in] oldName     The old name.
     *    @param [in] newName     The new name.
     */
    void fieldNameChanged(QString const& oldName, QString const& newName);

    /*!
     *  Informs of changes to the component editor tree.
     */
    void contentChanged();

    /*!
     *  Informs of a need to redraw the visualization.
     */
    void graphicsChanged(int index);

    void addressingChanged(int);

    /*!
     *  Sends an error message forward.
     *
     *    @param [in] message     The error message.
     */
    void errorMessage(QString const& message) const;

    /*!
     *  Sends a notification forward.
     *
     *    @param [in] message     The notification.
     */
    void noticeMessage(QString const& message) const;

    /*!
     *  Increase the amount of references made to the given parameter.
     *
     *    @param [in] id  The id of the given parameter.
     */
    void increaseReferences(QString const& id) const;

    /*!
     *  Decrease the amount of references made to the given parameter.
     *
     *    @param [in] id  The id of the given parameter.
     */
    void decreaseReferences(QString const& id) const;

    /*!
     *  Informs that a new item has been created.
     *
     *    @param [in] index   The index of the new item.
     */
    void childAdded(int index);

    /*!
     *  Informs that an item has been removed.
     *
     *    @param [in] index   The index of the removed item.
     */
    void childRemoved(int index);

private:

	//! No copying.
	RegisterEditor(const RegisterEditor& other);

	//! No assignment.
	RegisterEditor& operator=(const RegisterEditor& other);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! Pointer to the view that displays the items.
    EditableTableView* view_;

	//! Pointer to the model that manages the details of items.
	RegisterTableModel* model_;

    //! Interface for accessing fields.
    FieldInterface* interface_;

    //! Pointer to the available fields.
    QSharedPointer<QList<QSharedPointer<Field> > > fields_;
};

#endif // REGISTEREDITOR_H
