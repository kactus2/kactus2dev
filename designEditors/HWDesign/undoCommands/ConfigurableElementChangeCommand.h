//-----------------------------------------------------------------------------
// File: ConfigurableElementChangeCommand.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 22.08.2017
//
// Description:
// Undo change commands for configurable element value editor.
//-----------------------------------------------------------------------------

#ifndef CONFIGURABLEELEMENTCHANGECOMMAND_H
#define CONFIGURABLEELEMENTCHANGECOMMAND_H

#include <QUndoCommand>
#include <QSharedPointer>
#include <QObject>

class ConfigurableElementValue;

//-----------------------------------------------------------------------------
//! Undo change commands for configurable element value editor.
//-----------------------------------------------------------------------------
class ConfigurableElementChangeCommand: public QObject, public QUndoCommand
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] elementID               Parameter ID referenced by the changed configurable element.
     *      @param [in] oldValue                Old value of the changed configurable element.
     *      @param [in] newValue                New value of the changed configurable element.
     *      @param [in] defaultValue            Default value of the parameter.
     *      @param [in] configurableElements    List of the associated configurable element values.
     *      @param [in] parent                  The parent undo command.
     */
    ConfigurableElementChangeCommand(QString const& elementID, QString const& oldValue, QString const& newValue,
        QString const& defaultValue,
        QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > configurableElements,
        QUndoCommand* parent = 0);

	/*!
     *  The destructor.
     */
	virtual ~ConfigurableElementChangeCommand();

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
     *  Informs of data change in the configurable element referencing the selected ID.
     *
     *      @param [in] parameterID     ID of the referenced parameter.
     *      @param [in] newDataValue    New value of the configurable element.
     */
    void dataChangedInID(QString const& parameterID, QString const& newDataValue);

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

    //! No copying. No assignment.
    ConfigurableElementChangeCommand(const ConfigurableElementChangeCommand& other);
    ConfigurableElementChangeCommand& operator=(const ConfigurableElementChangeCommand& other);

    /*!
     *  Find the configurable element from the list of associated configurable elements.
     *
     *      @param [in] elementID   ID of the parameter referenced by the configurable element.
     *
     *      @return The configurable element value referencing the selected parameter. A new configurable element
     *              value is constructed if one does not exist.
     */
    QSharedPointer<ConfigurableElementValue> setupConfigurableElement(QString const& elementID);

    /*!
     *  Change the value in the configurable element.
     *
     *      @param [in] newElementValue     New value for the configurable element.
     */
    void changeConfigurableElement(QString const& newElementValue);

    //! The value before the change command.
    QString oldValue_;

    //! The changed value.
    QString newValue_;

    //! The default value.
    QString defaultValue_;

    //! The configurable element.
    QSharedPointer<ConfigurableElementValue> element_;

    //! List of the configurable elements containing the selected element.
    QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > configurableElements_;
};

//-----------------------------------------------------------------------------

#endif // CONFIGURABLEELEMENTCHANGECOMMAND_H
