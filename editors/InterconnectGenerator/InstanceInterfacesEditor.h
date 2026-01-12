//-----------------------------------------------------------------------------
// File: InstanceInterfacesEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 12.08.2025
//
// Description:
// Editor for selecting instance and its interfaces to connect to interconnect
//-----------------------------------------------------------------------------

#ifndef INSTANCEINTERFACESEDITOR_H
#define INSTANCEINTERFACESEDITOR_H

#include <common/widgets/EnumCollectionEditor/InterfaceEnumEditor.h>
#include <IPXACTmodels/common/Document.h>
#include <editors/common/ExpressionSet.h>

#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>

class QLabel;
class QPushButton;
class QComboBox;
class InstanceComboBox;
class ParameterFinder;
class ComponentParameterModel;

class InstanceInterfacesEditor : public QFrame
{
    Q_OBJECT
public:

    /*!
     *	Constructor
     *  
     *    @param [in] expressionSet                 Struct containing items needed for expression parsing and formatting
     *    @param [in] parameterCompleterModel       Parameter completer model for expressions
     *    @param [in] docRevision                   IP-XACT standard revision in use
     *    @param [in] parent                        The parent widget
     */
    InstanceInterfacesEditor(ExpressionSet const& expressionSet, ComponentParameterModel* parameterCompleterModel, Document::Revision docRevision, QWidget* parent = nullptr);

    virtual ~InstanceInterfacesEditor() = default;

    //! No copying
    InstanceInterfacesEditor(InstanceInterfacesEditor const& other) = delete;
    InstanceInterfacesEditor& operator=(InstanceInterfacesEditor const& other) = delete;

    // Call this whenever an instance is added (= removed from available instances)
    // => updates combo box options
    
    
    /*!
     *	Update the instance options for instance combo box.
     *  Should called be whenever an instance is added (= removed from available instances).
     *  Reselects the same instance, if there was a selection before. Otherwise picks the first option.
     * 
     *    @param [in] availableInterfaces     The instances available to select.
     */
    void updateAvailableInstances(QSet<QString> const& availableInterfaces);

    /*!
     *	Gets the currently selected instance name.
     *	    
     *    @return The currently selected instance.
     */
    QString getSelectedInstance() const;

    /*!
     *	Adds provided instance interfaces (of same mode) to the list of interfaces to create.
     *  
     *    @param [in] items             The interfaces to add.
     *    @param [in] interfaceMode     The mode of the added interfaces.
     */
    void addInterfaceItems(const QList<InterfaceEnumEditor::InterfaceInput>& items, General::InterfaceMode interfaceMode);
    
    /*!
     *	Creates widgets for all of the added instance interfaces.
     *  
     *    @param [in] isChannel     Flag indicating if interconnect should use channel or bridge.
     */
    void createInterfaceItems(bool isChannel);

    /*!
     *	Get the selected interfaces.
     *  
     *    @return A list containing information about each selected interface.
     */
    QList<InterfaceEnumEditor::InterfaceData> getSelectedInterfaces() const;

    /*!
     *	Clear all interface items of this editor.
     */
    void clearInterfaceItems();

public slots:

    /*!
     *	Handler for when new instance is selected.
     */
    void onInstanceSelected();

signals:

    //! Signal for parent widget emitted when new instance is selected
    void instanceSelected(QString const& newSelection, QString const& oldSelection);

    //! Signal for parent to remove the selected instance
    void instanceRemoved();

private:

    //! Top level layout for the editor.
    QVBoxLayout topLayout_;

    //! Layout for the button row.
    QHBoxLayout buttonRowLayout_;

    //! Widgets for the button row
    QLabel* instanceNameLabel_;
    QPushButton* removeInstanceButton_;
    InstanceComboBox* instanceSelectorCombo_;

    //! Stores the previous instance selection for comparison when new instance is selected.
    QString previousInstanceSelection_;

    //! Editor for the instance interfaces.
    InterfaceEnumEditor* interfacesEditor_;

    //! Temporarily stores the interfaces to add to the interfaces editor.
    QHash<General::InterfaceMode, QList<InterfaceEnumEditor::InterfaceInput> > interfacesToAdd_;
};


#endif // INSTANCEINTERFACESEDITOR_H

