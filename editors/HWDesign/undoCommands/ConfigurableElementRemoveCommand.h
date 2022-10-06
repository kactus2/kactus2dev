//-----------------------------------------------------------------------------
// File: ConfigurableElementRemoveCommand.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 28.08.2017
//
// Description:
// Undo remove command for configurable element values.
//-----------------------------------------------------------------------------

#ifndef CONFIGURABLEELEMENTREMOVECOMMAND_H
#define CONFIGURABLEELEMENTREMOVECOMMAND_H

#include <QUndoCommand>
#include <QSharedPointer>
#include <QObject>

class ConfigurableElementValue;

//-----------------------------------------------------------------------------
//! Undo remove command for configurable element values.
//-----------------------------------------------------------------------------
class ConfigurableElementRemoveCommand : public QObject, public QUndoCommand
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] elementID               ID of the parameter referenced by the configurable element.
     *      @param [in] itemRow                 Row of the item.
     *      @param [in] parentName              Name of the parent item.
     *      @param [in] configurableElements    List of the configurable element values containing the element
     *                                          removed in this command.
     *      @param [in] parent                  The parent undo command.
     */
    ConfigurableElementRemoveCommand(QString const& elementID, int itemRow, QString const& parentName,
        QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > configurableElements,
        QUndoCommand* parent = 0);

    //! No copying. No assignment.
    ConfigurableElementRemoveCommand(const ConfigurableElementRemoveCommand& other) = delete;
    ConfigurableElementRemoveCommand& operator=(const ConfigurableElementRemoveCommand& other) = delete;

	/*!
     *  The destructor.
     */
    virtual ~ConfigurableElementRemoveCommand() = default;

	/*!
     *  Undoes the command.
     */
	virtual void undo();

	/*!
     *  Redoes the command.
     */
	virtual void redo();

signals:

    /*!
     *  Informs that the selected configurable element value has been added to the associated table.
     *
     *      @param [in] elementID       ID of the selected configurable element.
     *      @param [in] elementValue    Value of the selected configurable element.
     *      @param [in] elementRow      Row of the configurable element in the associated table.
     */
    void addConfigurableElement(QString const& elementID, QString const& elementValue, 
        int elementRow);

    /*!
     *  Informs that the selected configurable element value has been removed from the associated table.
     *
     *      @param [in] elementID   ID of the parameter referenced by the selected configurable element.
     *      @param [in] parentName  Name of the parent item containing the configurable element item.
     *      @param [in] elementRow  Row of the removed configurable element value.
     */
    void removeConfigurableElement(QString const& elementID, int elementRow);

    /*!
     *  Increase the amount of references in the new value.
     *
     *      @param [in] newValue    The new value for the configurable element.
     */
    void increaseReferencesInNewValue(QString const& newValue);

    /*!
     *  Decrease the amount of references in the old value.
     *
     *      @param [in] oldValue    The old value for the configurable element.
     */
    void decreaseReferencesInOldValue(QString const& oldValue);

private:

    /*!
     *  Locate the configurable element value with the referenced parameter ID.
     *
     *      @param [in] elementID   ID of the parameter referenced by the selected configurable element value.
     *
     *      @return The configurable element value containing a reference to the selected parameter ID.
     */
    QSharedPointer<ConfigurableElementValue> getRemovedConfigurableElement(QString const& elementID);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Name of the parent item.
    QString parentName_;

    //! Row of the configurable element in the associated table.
    int itemRow_;

    //! Index of the configurable element in the list of configurable element values.
    int rowInConfigurableElements_;

    //! List of the configurable element values containing the selected configurable element.
    QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > configurableElements_;

    //! The selected configurable element value.
    QSharedPointer<ConfigurableElementValue> removedElement_;
};

//-----------------------------------------------------------------------------

#endif // CONFIGURABLEELEMENTREMOVECOMMAND_H
