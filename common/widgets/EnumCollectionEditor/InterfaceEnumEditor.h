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

struct EndpointInterfaceData {
    QString name;
    QString startValue; 
    QString range;
};

class InterfaceEnumEditor : public QFrame {
    Q_OBJECT

public:
    explicit InterfaceEnumEditor(QWidget* parent = nullptr);

    virtual ~InterfaceEnumEditor() = default;

    QStringList getSelectedStartingPointInterfaces() const;

    QList<EndpointInterfaceData> getSelectedEndpointInterfaces() const;

    void addItems(const QStringList& items, bool isTarget, const QString& instanceName);

    void setEndpointInterfaceValues(const QString& interfaceName, quint64 start, quint64 range);

    void clearEndpointInterfaceValues(const QString& interfaceName);

    void clearAll();

signals:
    void endpointInterfaceChecked(const QString& interfaceName, const QString& instanceName);
    void endpointInterfaceUnchecked(const QString& interfaceName, const QString& instanceName);
    void startingPointInterfaceChecked(const QString& interfaceName, const QString& instanceName);
    void startingPointInterfaceUnchecked(const QString& interfaceName, const QString& instanceName);

private:
    QVBoxLayout* mainLayout_;
    QScrollArea* scrollArea_;
    QWidget* scrollContainer_;
    QVBoxLayout* scrollLayout_;
    QList<InterfaceItem> interfaceItems_;
};

#endif
