#include "InstanceWidgets.h"

// Implementation of InstanceComboBox
InstanceComboBox::InstanceComboBox(QWidget* parent)
    : QComboBox(parent)
{
}

void InstanceComboBox::wheelEvent(QWheelEvent* event)
{
    event->ignore();
}
