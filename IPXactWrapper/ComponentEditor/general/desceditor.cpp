//-----------------------------------------------------------------------------
// File: desceditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 7.2.2011
//
// Description:
// Description editor for the component editor.
//-----------------------------------------------------------------------------

#include "desceditor.h"

#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: DescEditor()
//-----------------------------------------------------------------------------
DescEditor::DescEditor(QWidget* parent /* = 0 */) : QGroupBox(parent), m_descEdit(0)
{
    setTitle(tr("Description"));

    // Add the text edit to the group box.
    QVBoxLayout* layout = new QVBoxLayout(this);

    m_descEdit = new QPlainTextEdit();
    layout->addWidget(m_descEdit);

    // Connect the signals informing of changes in items.
    connect(m_descEdit, SIGNAL(textChanged()),
            this, SIGNAL(contentChanged()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ~DescEditor()
//-----------------------------------------------------------------------------
DescEditor::~DescEditor()
{
}

//-----------------------------------------------------------------------------
// Function: setDescription()
//-----------------------------------------------------------------------------
void DescEditor::setDescription(QString const& desc)
{
    m_descEdit->setPlainText(desc);
}

//-----------------------------------------------------------------------------
// Function: getDescription()
//-----------------------------------------------------------------------------
QString DescEditor::getDescription() const
{
    return m_descEdit->toPlainText();
}
