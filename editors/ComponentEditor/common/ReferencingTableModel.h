//-----------------------------------------------------------------------------
// File: ReferencingTableModel.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 26.03.2015
//
// Description:
// Base class for table model properties for editable tables.
//-----------------------------------------------------------------------------

#ifndef REFERENCINGTABLEMODEL_H
#define REFERENCINGTABLEMODEL_H

#include <KactusAPI/include/ParameterFinder.h>

#include <QAbstractTableModel>

//-----------------------------------------------------------------------------
//! Base class for table model properties for editable tables.
//-----------------------------------------------------------------------------
class ReferencingTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:

    /*!
     *  [Description].
     *
     *    @param [in] parameterFinder     The parameter finder.
     *    @param [in] parent              The owner of this table.
     */
    ReferencingTableModel(QSharedPointer<ParameterFinder> parameterFinder, QObject *parent);

    /*!
     *  The destructor.
     */
    virtual ~ReferencingTableModel();

protected:

    /*!
     *  Remove all the references to all parameters from the item on the selected row.
     *
     *    @param [in] row     The row of the selected item.
     */
    void removeReferencesInItemOnRow(const int& row) const;

    /*!
     *  Gets the number of all the references made to a selected id on the selected row.
     *
     *    @param [in] row         The row of the selected item.
     *    @param [in] valueID     The id of the referenced parameter.
     *
     *    @return The amount of references made to the selected id on the selected row.
     */
    virtual int getAllReferencesToIdInItemOnRow(const int& row, QString const& valueID) const = 0;

    /*!
     *  Remove all the references from a single expression.
     *
     *    @param [in] expression  The expression, where to seek the references.
     */
    void removeReferencesFromSingleExpression(QString const& expression) const;

    /*!
     *  Get the unique name from the selected name.
     *
     *    @param [in] originalName    The selected name.
     *    @param [in] currentNames    A list of already used names.
     *
     *    @return A unique name created from the selected name.
     */
    QString getUniqueName(QString const& originalName, QStringList currentNames) const;

signals:

    /*!
     *  Decrease the amount of references to the selected parameter.
     *
     *    @param [in] valueId   The id of the referenced parameter.
     */
    void decreaseReferences(const QString& valueId) const;

    /*!
     *  Increase the amount of references to the selected parameter.
     *
     *    @param [in] valueId   The id of the referenced parameter.
     */
    void increaseReferences(QString const& parameterID);

private:

    //! Disable copying and assignment.
    ReferencingTableModel(const ReferencingTableModel& other);
    ReferencingTableModel& operator=(const ReferencingTableModel& other);

    //! The parameter finder.
    QSharedPointer<ParameterFinder> parameterFinder_;
};

#endif // REFERENCINGTABLEMODEL_H
