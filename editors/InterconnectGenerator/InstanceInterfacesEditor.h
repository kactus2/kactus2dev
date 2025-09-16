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

#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>

class QLabel;
class QPushButton;
class QComboBox;
class InterfaceEnumEditor;
class InstanceComboBox;


class InstanceInterfacesEditor : public QFrame
{
    Q_OBJECT
public:

    explicit InstanceInterfacesEditor(Document::Revision docRevision, QWidget* parent = nullptr);

    virtual ~InstanceInterfacesEditor() = default;

    // Call this whenever an instance is added (= removed from available instances)
    // => updates combo box options
    void updateAvailableInstances(QSet<QString> const& availableInterfaces);

    QString getSelectedInstance() const;

    void addInterfaceItems(const QStringList& items, General::InterfaceMode interfaceMode);
    void createInterfaceItems(const QString& instanceName, bool isChannel);

    QList<InterfaceEnumEditor::InterfaceData> getSelectedInterfaces() const;

    void clearInterfaceItems();

public slots:

    void onInstanceSelected();

signals:

    void instanceSelected(QString const& newSelection, QString const& oldSelection);

    void instanceRemoved();

protected:
private:
    QVBoxLayout topLayout_;
    QHBoxLayout buttonRowLayout_;

    QLabel* instanceNameLabel_;
    QPushButton* removeInstanceButton_;
    InstanceComboBox* instanceSelectorCombo_;

    QString previousInstanceSelection_;

    InterfaceEnumEditor* interfacesEditor_;

    QHash<General::InterfaceMode, QStringList> interfacesToAdd_;
};


#endif // INSTANCEINTERFACESEDITOR_H

