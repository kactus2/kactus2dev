//-----------------------------------------------------------------------------
// File: busdefgroup.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 21.6.2011
//
// Description:
// BusDefGroup contains elements to set the properties of a Bus Definition.
//-----------------------------------------------------------------------------

#ifndef BUSDEFGROUP_H
#define BUSDEFGROUP_H

#include <common/widgets/listManager/listmanager.h>

#include <editors/BusDefinitionEditor/SystemGroupListEditor.h>

#include <editors/common/DocumentNameGroupEditor.h>


#include <IPXACTmodels/BusDefinition/BusDefinition.h>

#include <QGroupBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QSharedPointer>
#include <QPlainTextEdit>

class VLNVDisplayer;
class VLNVEditor;
class LibraryInterface;
class ParameterGroupBox;
class ExpressionFormatter;
class ParameterFinder;
class AbstractParameterInterface;

//-----------------------------------------------------------------------------
//! BusDefGroup contains elements to set the properties of a Bus Definition.
//-----------------------------------------------------------------------------
class BusDefGroup : public QWidget
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *    @param [in] libraryHandler          Interface to the library.
     *    @param [in] expressionFormatter     The expression formatter to use.
     *    @param [in] parameterFinder         The parameter finder to use.
     *    @param [in] parent                  The owner of this widget.
     */
    BusDefGroup(LibraryInterface* libraryHandler, QSharedPointer<ExpressionFormatter> expressionFormatter,
        QSharedPointer<ParameterFinder> parameterFinder, QWidget *parent);

    /*!
     *  The destructor.
     */
    virtual ~BusDefGroup() = default;

    /*!
     *  Set the bus definition that is being edited.
     *
     *    @param [in] busDef The bus definition
     */
    void setBusDef(QSharedPointer<BusDefinition> busDef);

signals:

    /*!
     *  Emitted when user changes the state of one of the elements.
     */
    void contentChanged();

    /*!
     *  Increase the amount of references to the parameter corresponding to the id.
     *
     *    @param [in] id      The id of the parameter being searched for.
     */
    void increaseReferences(QString id);

    /*!
     *  Decrease the amount of references to the parameter corresponding to the id.
     *
     *    @param [in] id      The id of the parameter being searched for.
     */
    void decreaseReferences(QString id);

    /*!
     *  Recalculate references made to the selected parameters.
     *
     *    @param [in] parameterList       The selected parameters.
     *    @param [in] parameterInterface  Interface for accessing parameters.
     */
    void recalculateReferencesToParameters(QStringList const& parameterList,
        AbstractParameterInterface* parameterInterface);

    /*!
     *  Open the reference tree of the parameter with the id.
     *
     *    @param [in] id              The id of the parameter.
     *    @param [in] parameterName   Name of the selected parameter.
     */
    void openReferenceTree(QString const& id, QString const& parameterName);

private slots:

    /*!
     *  Handler for state changes on direct connection check box.
     */
    void onDirectConnectionChanged(bool checked);

    /*!
     *  Handler for state changes on is broadcast check box.
     */
    void onIsBroadcastChanged(bool checked);

    /*!
     *  Handler for state changes on is addressable check box.
     */
    void onIsAddressableChanged(bool checked);

    /*!
     *  Handler for changes in max masters line edit.
     */
    void onInitiatorsChanged();

    /*!
     *  Handler for changes in max slaves line edit.
     */
    void onTargetsChanged();

    /*!
     *  Handler for changes in system group names.
     */
    void onSystemNamesChanged();

    /*!
     *  Handles the changes in bus definition extend.
     */
    void onExtendChanged();

private:
    //! No copying. No assignment.
    BusDefGroup(const BusDefGroup& other);
    BusDefGroup& operator=(const BusDefGroup& other);

    /*!
     *  Sets the widget layout.
     */
    void setupLayout();

    /*!
     *  Setup the extended bus definition.
     */
    void setupExtendedBus();

    /*!
     *  Get the extended bus definition for the selected bus definition.
     *
     *    @param [in] busDefinition   The selected bus definition.
     *
     *    @return The extended bus definition.
     */
    QSharedPointer<const BusDefinition> getExtendedBus(QSharedPointer<const BusDefinition> busDefinition) const;
    
    /*!
     *  Lock editors according to the selected extended bus definition.
     *
     *    @param [in] extendedBus     The selected extended bus definition.
     */
    void extendBusDefinition(QSharedPointer<const BusDefinition> extendedBus);

    /*!
     *  Remove extend system groups.
     */
    void removeSystemGroupsFromExtendedDefinition();

    /*!
     *  Remove the lock on editors caused by the previous extended bus definition.
     */
    void removeBusExtension();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------
    
    //! The library interface.
    LibraryInterface* library_;

    //! The bus definition to edit.
    QSharedPointer<BusDefinition> busDef_;

    //! Check box to set the direct connection option.
    QCheckBox directConnection_;

    //! Check box to set the broadcast option.
    QCheckBox isBroadcast_;

    //! Check box to set the is addressable option.
    QCheckBox isAddressable_;

    //! Editor to set the maximum number of initiators.
    QLineEdit maxInitiatorsEditor_;

    //! Editor to set the maximum number of targets.
    QLineEdit maxTargetsEditor_;

    //! Editor for system group names.
    SystemGroupListEditor systemGroupEditor_;

    //! Editor for bus definition document name group
    DocumentNameGroupEditor documentNameGroupEditor_;

    //! Editor for the bus definition extension.
    VLNVEditor* extendEditor_;

    //! The bus definition parameters editor.
    ParameterGroupBox* parameterEditor_;
};

#endif // BUSDEFGROUP_H
