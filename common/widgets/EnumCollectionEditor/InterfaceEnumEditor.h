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

#include <IPXACTmodels/generaldeclarations.h>
#include <IPXACTmodels/common/Document.h>
#include <editors/common/ExpressionSet.h>

#include <QFrame>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QCheckBox>
#include <QLineEdit>
#include <QLabel>
#include <QList>
#include <QString>
#include <QPair>

class ExpressionEditor;
class ExpressionFormatter;
class ParameterFinder;
class ComponentParameterModel;

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
    explicit InterfaceEnumEditor(ExpressionSet expressionSet, ComponentParameterModel* parameterCompleterModel, Document::Revision docRevision, QWidget* parent = nullptr);

    /*!
     *  Destructor.
     */
    virtual ~InterfaceEnumEditor() = default;

    //! Contains data about interface selected in editor
    struct InterfaceData
    {
        QString name;
        QString startValue;
        QString range;
    };

    //! Contains data about an interface to be added as option in editor
    struct InterfaceInput
    {
        QString interfaceName;

        //! Flag indicating if the interface is able to 
        //! 1) connect to the bridge initiator(s), if interconnect is using bridge or
        //! 2) connect to the interconnect tomponent mirrored target, if interconnect is using channel
        bool isTargetAdjacent;
    };

    /*!
     *  Get the details of selected interfaces.
     *
     *  @return List of interface data including names and address information.
     */
    QList<InterfaceData> getSelectedInterfaces() const;

    /*!
     *  Add a list of interface names to the editor.
     *
     *  @param [in] items         List of interface names to add.
     *  @param [in] isTarget      True if the interfaces are for endpoints; false for starting points.
     *  @param [in] instanceName  Name of the associated instance.
     */
    void addItems(const QStringList& items, bool isTarget, const QString& instanceName);

    void addItems(const QList<InterfaceInput>& items, General::InterfaceMode interfaceMode, bool isChannel);

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

    void createLayoutCondenser();

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

private slots:

    //! Handles finishing editing of current word when focus is moved from calling editor
    void onExpressionValueEdited();

private:

    /*!
     *  Struct representing the UI elements associated with a single interface.
     */
    struct InterfaceItem
    {
        QCheckBox* checkBox;       /*!< Checkbox to enable or disable the interface. */
        QLabel* startLabel;        /*!< Label for the start address. */
        ExpressionEditor* startEdit;      /*!< Input field for the start address. */
        QLabel* rangeLabel;        /*!< Label for the address range. */
        ExpressionEditor* rangeEdit;      /*!< Input field for the address range. */
    };

    QHash<QString, InterfaceItem> singleInterfaces_;

    //! Main layout for the editor.
    QVBoxLayout* mainLayout_;

    //! Scroll area to accommodate many interface items.
    QScrollArea* scrollArea_;

    //! Container widget inside the scroll area.
    QWidget* scrollContainer_;

    //! Layout for items within the scroll container.
    QVBoxLayout* scrollLayout_;

    QGridLayout* itemLayout_;

    QWidget* itemLayoutCondenser_;

    //! Parameter finder for expression editors. Should use parameters of generated interconnect component.
    QSharedPointer<ParameterFinder> parameterFinder_;

    //! Parameter completer model for expression editors.
    ComponentParameterModel* parameterCompleterModel_;

    QSharedPointer<ExpressionFormatter> expressionFormatter_;

    //! List of currently displayed interface widgets.
    QList<InterfaceItem> interfaceItems_;

    bool isStd22_ = true;
};

#endif
