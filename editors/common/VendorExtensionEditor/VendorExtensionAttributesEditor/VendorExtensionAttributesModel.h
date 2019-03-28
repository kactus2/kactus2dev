//-----------------------------------------------------------------------------
// File: VendorExtensionAttributesModel.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 20.03.2019
//
// Description:
// Model for editing vendor extension attributes.
//-----------------------------------------------------------------------------

#ifndef VENDOREXTENSIONATTRIBUTESMODEL_H
#define VENDOREXTENSIONATTRIBUTESMODEL_H

#include <QAbstractTableModel>

class GenericVendorExtension;

//-----------------------------------------------------------------------------
// class ParameterArrayModel.
//-----------------------------------------------------------------------------
class VendorExtensionAttributesModel : public QAbstractTableModel
{
	Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] genericExtension    Generic extension containing the edited attributes.
     *      @param [in] parent              The parent of this model.
     */
    VendorExtensionAttributesModel(GenericVendorExtension* genericExtension, QObject* parent = 0);

    /*!
     *  The destructor.
     */
    ~VendorExtensionAttributesModel();

    /*!
     *  Returns the amount of columns in the model.
     *
     *      @param [in] parent  If given, the column count of the children of the given parent is returned.
     *
     *      @return The amount of columns.
     */
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    /*!
     *  Returns the amount of rows in the model.
     *
     *      @param [in] parent  If given, the row count of the children of the given parent is returned.
     *
     *      @return The amount of rows.
     */
    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    /*!
     *  Retrieves the header data for the given column (row headers are not supported in this model).
     *
     *      @param [in] section         The column from which the index is retrieved.
     *      @param [in] orientation     Whether the header is horizontal or vertical.
     *      @param [in] role            The role for which the header is retrieved.
     *
     *      @return The header data.
     */
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    /*!
     *  Retrieves the data for the appropriate role at the given index.
     *
     *      @param [in] index   The index of data to be returned.
     *      @param [in] role    The role for which data is returned.
     *
     *      @return The requested data.
     */
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    /*!
     *  Sets the data at the given index.
     *
     *      @param [in] index   The index at which the data is set.
     *      @param [in] value   The new value to being set.
     *      @param [in] role    The role for which the data is set.
     *
     *      @return True, if data was set successfully, otherwise false.
     */
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    /*!
     *  Returns the flags for an object at the given index.
     *
     *      @param [in] index   The index of the object queried.
     *
     *      @return The relevant flags.
     */
    Qt::ItemFlags flags(QModelIndex const& index) const;

    /*!
     *  Get the edited attributes.
     *
     *      @return The edited attributes.
     */
    QVector<QPair<QString, QString> > getAttributes() const;

    // Disable copying. // Disable assignment.
    VendorExtensionAttributesModel(VendorExtensionAttributesModel const& rhs) = delete;
    VendorExtensionAttributesModel& operator=(VendorExtensionAttributesModel const& rhs) = default;

signals:
    //! Emitted when the model content has changed.
    void contentChanged();

public slots:

    /*!
     *  Called when a new enumeration should be added to the model.
     *
     *      @param [in] index   The index to add the enumeration to.
     */
    void onAddItem(QModelIndex const& index);

    /*!
     *  Called when an enumeration should be removed from the model.
     *
     *      @param [in] index   The index to remove the enumeration from.
     */
    void onRemoveItem(QModelIndex const& index);

private:


    /*!
     *  Check if the index is valid.
     *
     *      @param [in] index   The index being checked.
     *
     *      @return True if the index is not valid, otherwise false.
     */
    bool indexIsNotValid(QModelIndex const& index) const;

    /*!
     *  Gets the value of the array in the given index.
     *
     *      @param [in] index   The index of the value.
     *
     *      @return The data in the given index.
     */
    QVariant valueForIndex(QModelIndex const& index) const;

    /*!
     *  Get a unique type name for a new attribute.
     *
     *      @return A unique type name.
     */
    QString getUniqueNameForNewAttribute() const;

    /*!
     *  Check if the selected type name exists in the current attributes.
     *
     *      @param [in] attributeName   The selected type name.
     *
     *      @return True, if the name exists in the selected attributes, false otherwise.
     */
    bool nameExistsInAttributes(QString const& attributeName) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! List of attribute type / value pairs.
    QVector<QPair<QString, QString> > attributes_;
};

#endif // VENDOREXTENSIONATTRIBUTESMODEL_H
