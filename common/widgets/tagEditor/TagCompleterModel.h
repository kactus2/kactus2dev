//-----------------------------------------------------------------------------
// File: TagCompleterModel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 28.08.2019
//
// Description:
// Model for tag completer.
//-----------------------------------------------------------------------------

#ifndef TAGCOMPLETERMODEL_H
#define TAGCOMPLETERMODEL_H

#include <IPXACTmodels/common/TagData.h>

#include <KactusAPI/include/TagManager.h>


#include <QAbstractListModel>
#include <QObject>

//-----------------------------------------------------------------------------
//! Model for tag completer.
//-----------------------------------------------------------------------------
class TagCompleterModel : public QAbstractListModel
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *    @param [in] tagData     The available tag data.
     *    @param [in] parent      Onwer of this model.
     */
    TagCompleterModel(QVector<TagData> tagData, QObject* parent = 0);

    /*!
     *  The destructor.
     */
    ~TagCompleterModel() = default;

    /*!
     *  Finds the number of rows in the model.
     *
     *    @param [in] parent   The parent index whose row count to find.
     *
     *    @return The number of rows in the model.
     */
    virtual int rowCount(QModelIndex const& parent = QModelIndex()) const;

    /*!
     *  Finds the data in the model for the given index.
     *
     *    @param [in] index   The index whose data to find.
     *    @param [in] role    The role describing which aspect of the data to find.
     *
     *    @return The data in the given index.
     */
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

public slots:

    /*!
     *  Handler for item selection.
     *
     *    @param [in] index   Index of the selected item.
     */
    void itemSelected(QModelIndex const& index);

    /*!
     *  Handler for item selection with item number.
     *
     *    @param [in] itemIndex   Index of the selected item.
     */
    void unfilteredItemSelected(int itemIndex);

signals:

    /*!
     *  Informs of the color of the selected item.
     *
     *    @param [in] itemColor   Color of the selected item.
     */
    void selectedColor(QColor const& itemColor);

private:
    // Disable copying.
    TagCompleterModel(TagCompleterModel const& rhs);
    TagCompleterModel& operator=(TagCompleterModel const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------
    
    //! List of available tags.
    QVector<TagData> tags_;
};

//-----------------------------------------------------------------------------

#endif // TAGCOMPLETERMODEL_H
