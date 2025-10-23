//-----------------------------------------------------------------------------------------
// File: InterfaceEnumEditor.h
//-----------------------------------------------------------------------------------------
// Project: Kactus2
// Author: Sampo Suokuisma
// Date: 05.06.2025
//
// Description:
// Class for enumerating and selecting interfaces to connect to an interconnect.
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
     *  @param [in] interfaceMode The mode of the interface.
     *  @param [in] isChannel     True, if interconnect type is channel, otherwise false.
     */
    void addItems(const QList<InterfaceInput>& items, General::InterfaceMode interfaceMode, bool isChannel);

    /*!
     *	Add stretch to interfaces layout, if instance has no target-adjacent interfaces and used mode is channel.
     *  Aligns non-target-adjacent interfaces properly.
     */
    void addStretchIfNeeded();

    /*!
     *	Limit width of interface checkbox texts to fraction of parent widget width if needed.
     */
    void limitCheckboxTextWidths();

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

    //! Map of displayed interface names and related widgets
    QHash<QString, InterfaceItem> singleInterfaces_;

    //! Main layout for the editor.
    QVBoxLayout* mainLayout_;

    //! Layout for interfaces and related widgets
    QGridLayout* itemLayout_;

    /*
     *   Layout holding the gridlayout containing interfaces etc.
     *   Possible to add stretch to end of layout, if no target adjacent interfaces are added
     *   in order to align interface checkbox to the left.
     *   Need for stretch must be checked in dialog.
     */
    QHBoxLayout* optionalStretchLayout_;

    //! Keeps track of need to add stretch to interfaces based on added interfaces.
    bool needInterfacesStretch_ = true;

    //! Parameter finder for expression editors. Should use parameters of generated interconnect component.
    QSharedPointer<ParameterFinder> parameterFinder_;

    //! Parameter completer model for expression editors.
    ComponentParameterModel* parameterCompleterModel_;

    //! Formatter for interface start and range expressions.
    QSharedPointer<ExpressionFormatter> expressionFormatter_;

    //! Flag indicating the standard revision in use.
    bool isStd22_ = true;
};

#endif
