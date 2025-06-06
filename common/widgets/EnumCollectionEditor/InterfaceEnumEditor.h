//-----------------------------------------------------------------------------------------
// File: InterfaceEnumEditor.h
//-----------------------------------------------------------------------------------------
// Project: Kactus2
// Author: Sampo Suokuisma
// Date: 05.06.2025
//
// Description:
// Class for enumerating and selecting interface points for starting or ending connections.
//-----------------------------------------------------------------------------------------

#ifndef INTERFACEENUMEDITOR_H
#define INTERFACEENUMEDITOR_H

#include <QFrame>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QCheckBox>
#include <QLineEdit>
#include <QLabel>
#include <QList>
#include <QString>
#include <QPair>

/*!
 *  Struct representing the UI elements associated with a single interface.
 */
struct InterfaceItem {
    QCheckBox* checkBox;       /*!< Checkbox to enable or disable the interface. */
    QLabel* startLabel;        /*!< Label for the start address. */
    QLineEdit* startEdit;      /*!< Input field for the start address. */
    QLabel* rangeLabel;        /*!< Label for the address range. */
    QLineEdit* rangeEdit;      /*!< Input field for the address range. */
};

/*!
 *  Struct representing selected data for a target endpoint interface.
 */
struct EndpointInterfaceData {
    QString name;              /*!< Name of the interface. */
    QString startValue;        /*!< Start address value (as a string). */
    QString range;             /*!< Range value (as a string). */
};

/*!
 *  Class for enumerating and selecting interface points for starting or ending connections.
 */
class InterfaceEnumEditor : public QFrame {
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *  @param [in] parent  Parent widget.
     */
    explicit InterfaceEnumEditor(QWidget* parent = nullptr);

    /*!
     *  Destructor.
     */
    virtual ~InterfaceEnumEditor() = default;

    /*!
     *  Get the names of selected starting point interfaces.
     *
     *  @return List of interface names that have been selected as starting points.
     */
    QStringList getSelectedStartingPointInterfaces() const;

    /*!
     *  Get the details of selected endpoint interfaces.
     *
     *  @return List of endpoint interface data including names and address information.
     */
    QList<EndpointInterfaceData> getSelectedEndpointInterfaces() const;

    /*!
     *  Add a list of interface names to the editor.
     *
     *  @param [in] items         List of interface names to add.
     *  @param [in] isTarget      True if the interfaces are for endpoints; false for starting points.
     *  @param [in] instanceName  Name of the associated instance.
     */
    void addItems(const QStringList& items, bool isTarget, const QString& instanceName);

    /*!
     *  Set address values for a selected endpoint interface.
     *
     *  @param [in] interfaceName  Name of the interface.
     *  @param [in] start          Start address value.
     *  @param [in] range          Range value.
     */
    void setEndpointInterfaceValues(const QString& interfaceName, quint64 start, quint64 range);

    /*!
     *  Clear the address values for a specific endpoint interface.
     *
     *  @param [in] interfaceName  Name of the interface to clear.
     */
    void clearEndpointInterfaceValues(const QString& interfaceName);

    /*!
     *  Remove all interface entries from the editor.
     */
    void clearAll();

signals:
    /*!
     *  Emitted when an endpoint interface is checked.
     *
     *  @param interfaceName  Name of the interface.
     *  @param instanceName   Name of the associated instance.
     */
    void endpointInterfaceChecked(const QString& interfaceName, const QString& instanceName);

    /*!
     *  Emitted when an endpoint interface is unchecked.
     *
     *  @param interfaceName  Name of the interface.
     *  @param instanceName   Name of the associated instance.
     */
    void endpointInterfaceUnchecked(const QString& interfaceName, const QString& instanceName);

    /*!
     *  Emitted when a starting point interface is checked.
     *
     *  @param interfaceName  Name of the interface.
     *  @param instanceName   Name of the associated instance.
     */
    void startingPointInterfaceChecked(const QString& interfaceName, const QString& instanceName);

    /*!
     *  Emitted when a starting point interface is unchecked.
     *
     *  @param interfaceName  Name of the interface.
     *  @param instanceName   Name of the associated instance.
     */
    void startingPointInterfaceUnchecked(const QString& interfaceName, const QString& instanceName);

private:
    //! Main layout for the editor.
    QVBoxLayout* mainLayout_;

    //! Scroll area to accommodate many interface items.
    QScrollArea* scrollArea_;

    //! Container widget inside the scroll area.
    QWidget* scrollContainer_;

    //! Layout for items within the scroll container.
    QVBoxLayout* scrollLayout_;

    //! List of currently displayed interface widgets.
    QList<InterfaceItem> interfaceItems_;
};

#endif
