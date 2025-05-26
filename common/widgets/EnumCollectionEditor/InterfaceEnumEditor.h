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

struct InterfaceItem {
    QCheckBox* checkBox;
    QLabel* startLabel;
    QLineEdit* startEdit;
    QLabel* rangeLabel;
    QLineEdit* rangeEdit;
};

struct TargetInterfaceData {
    QString name;
    QString startValue; 
    QString range;
};

class InterfaceEnumEditor : public QFrame {
    Q_OBJECT

public:
    explicit InterfaceEnumEditor(QWidget* parent = nullptr);

    virtual ~InterfaceEnumEditor() = default;

    QStringList getSelectedInitiatorInterfaces() const;

    QList<TargetInterfaceData> getSelectedTargetInterfaces() const;

    void addItems(const QStringList& items, bool isTarget, const QString& instanceName);

    void setTargetInterfaceValues(const QString& interfaceName, quint64 start, quint64 range);

    void clearTargetInterfaceValues(const QString& interfaceName);

    void clearAll();

signals:
    void targetInterfaceChecked(const QString& interfaceName, const QString& instanceName);
    void targetInterfaceUnchecked(const QString& interfaceName, const QString& instanceName);
    void initiatorInterfaceChecked(const QString& interfaceName, const QString& instanceName);
    void initiatorInterfaceUnchecked(const QString& interfaceName, const QString& instanceName);

private:
    QVBoxLayout* mainLayout_;
    QScrollArea* scrollArea_;
    QWidget* scrollContainer_;
    QVBoxLayout* scrollLayout_;
    QList<InterfaceItem> interfaceItems_;
};

#endif
