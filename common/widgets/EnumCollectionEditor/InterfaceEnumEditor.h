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
    QLabel* endLabel;
    QLineEdit* endEdit;
};

struct TargetInterfaceData {
    QString name;
    QString startValue; 
    QString endValue;
};

class InterfaceEnumEditor : public QFrame {
    Q_OBJECT

public:
    explicit InterfaceEnumEditor(QWidget* parent = nullptr);

    QStringList getSelectedInitiatorInterfaces() const;

    QList<TargetInterfaceData> getSelectedTargetInterfaces() const;

    void addItems(const QStringList& items, bool isTarget);

    void clearAll();

private:
    QVBoxLayout* mainLayout_;
    QScrollArea* scrollArea_;
    QWidget* scrollContainer_;
    QVBoxLayout* scrollLayout_;
    QList<InterfaceItem> interfaceItems_;
};

#endif
