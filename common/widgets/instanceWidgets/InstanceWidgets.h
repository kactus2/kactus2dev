#ifndef INSTANCEWIDGETS_H
#define INSTANCEWIDGETS_H

#include <QComboBox>
#include <QWheelEvent>

// Custom QComboBox to disable wheel events
class InstanceComboBox : public QComboBox
{
    Q_OBJECT
public:
    explicit InstanceComboBox(QWidget* parent = nullptr);

protected:
    void wheelEvent(QWheelEvent* event) override;
};

#endif // INSTANCEWIDGETS_H
