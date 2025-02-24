//-----------------------------------------------------------------------------
// File: ChoicesModel.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 04.11.2014
//
// Description:
// Model for editing choices in a component.
//-----------------------------------------------------------------------------

#ifndef CHOICESMODEL_H
#define CHOICESMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include <QVariant>
#include <QModelIndex>
#include <QSharedPointer>

class Choice;
class ChoiceValidator;

//-----------------------------------------------------------------------------
//! Model for editing choices in a component.
//-----------------------------------------------------------------------------
class ChoicesModel : public QAbstractListModel
{
	Q_OBJECT

public:
    
    /*!
     *  Constructor.
     *
     *    @param [in] choices     List of the available choices.
     *    @param [in] validator   Validator for choices.
     *    @param [in] parent      The parent object.
     */
    ChoicesModel(QSharedPointer<QList<QSharedPointer<Choice> > > choices,
        QSharedPointer<ChoiceValidator> validator,
        QObject* parent = 0);
    
    /*!
     *  Destructor.
     */
    virtual ~ChoicesModel() = default;

    /*!
     *  Validate the choices.
     * 
     *    @return True, if the choices are valid, false otherwise.
     */
    bool validate() const;

    /*!
     *  Returns the choice at the given index.
     *
     *    @param [in] index   The selected index.
     */
    QSharedPointer<Choice> getChoice(QModelIndex const& index) const;
    
    /*!
     *  Returns the number of rows in this model.
     *
     *    @param [in] parent ModelIndex of the item that's rowCount is requested.
     */
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
    
    /*!
     *  Returns the data stored for the specified item.
     *
     *    @param [in] index  ModelIndex of the wanted item.
     *    @param [in] role   Specifies what kind of data is requested.
     */
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    
    /*!
     *  Returns the data for the header of the list.
     *
     *    @param [in] section      Specifies the column for which the header is requested.
     *    @param [in] orientation  The orientation of the header, only Qt::Horizontal is supported.
     *    @param [in] role         Specifies what kind of header data is requested.
     */
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    
    /*!
     *  Saves the data to the model for specified item
     *
     *    @param [in] index  The model index of the item that's data is to be saved.
     *    @param [in] value  The data that is to be saved.
     *    @param [in] role   The role specifies what kind of data should be saved.
     *
     *    @return True if saving happened successfully.
     */
    virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);
    
    /*!
     *  Returns the item flags that defines the possible operations for the item.
     *
     *    @param [in] index Model index that identifies the item.
     *
     *    @return Qt::ItemFlags specify the possible operations for the item.
     */
    virtual Qt::ItemFlags flags(const QModelIndex& index) const;
    
    /*!
     *  Replace an item text in the list.
     *
     *    @param [in] index    ModelIndex of the item that is to be replaced.
     *    @param [in] newText  The new text for the given item.
     */
    virtual void replace(QModelIndex& index, const QString newText);

public slots:
    
    /*!
     *  Removes the specified item from the model.
     *
     *    @param [in] index The model index of the item to remove.
     */
    virtual void remove(const QModelIndex& index);
    
    /*!
     *  A new item should be added to given index.
     *
     *    @param [in] index The position where new item should be added at.
     */
    virtual void addItem(const QModelIndex& index);
    
    /*!
     *  Move item to another position.
     *
     *    @param [in] originalPos  Identifies the item that should be moved.
     *    @param [in] newPos       The new position the item should be moved to.
     */
    virtual void moveItem(const QModelIndex& originalPos, const QModelIndex& newPos);

signals:

    /*!
     *  Emitted when contents of the model change.
     */
	void contentChanged();
    
    /*!
     *	Emitted when the choice list empties or is no longer empty
     *  
     *    @param [in] isEmpty     Indicates if list was emptied or not.
     */
    void choicesEmpty(bool isEmpty);

private:
    
    // Disable copying.
    ChoicesModel(ChoicesModel const& rhs);
    ChoicesModel& operator=(ChoicesModel const& rhs);
    
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------
    
    //! The list of available choices.
    QSharedPointer<QList<QSharedPointer<Choice> > > choices_;
    
    //! Validator for choices.
    QSharedPointer<ChoiceValidator> validator_;
};

#endif // CHOICESMODEL_H
