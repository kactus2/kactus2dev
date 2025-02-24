//-----------------------------------------------------------------------------
// File: AbstractionPortsDelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 06.06.2018
//
// Description:
// Master delegate for abstraction definition wire and transactional ports.
//-----------------------------------------------------------------------------

#include "AbstractionPortsDelegate.h"
#include "QualifierEditor.h"

#include <IPXACTmodels/BusDefinition/BusDefinition.h>
#include <IPXACTmodels/common/Qualifier.h>

#include <IPXACTmodels/utilities/BusDefinitionUtils.h>

#include <editors/AbstractionDefinitionEditor/LogicalPortColumns.h>


#include <KactusAPI/include/LibraryInterface.h>
#include <KactusAPI/include/PortAbstractionInterface.h>

#include <QComboBox>
#include <QStringList>
#include <QLineEdit>

//-----------------------------------------------------------------------------
// Function: AbstractionPortsDelegate::AbstractionPortsDelegate()
//-----------------------------------------------------------------------------
AbstractionPortsDelegate::AbstractionPortsDelegate(QAbstractItemModel* parametersModel,
    QSharedPointer<ParameterFinder> parameterFinder, LibraryInterface* libraryAcces, 
    Document::Revision stdRevision, QObject* parent) :
ExpressionDelegate(parametersModel, parameterFinder, parent),
stdRevision_(stdRevision),
libraryAccess_(libraryAcces),
busDefinition_(0)
{
    setModeOptions();
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* AbstractionPortsDelegate::createEditor(QWidget* parent, QStyleOptionViewItem const& option,
    const QModelIndex& index ) const
{
    if (index.column() == LogicalPortColumns::MODE)
    {
        QComboBox* box = new QComboBox(parent);

        box->addItems(modeOptions_);

        connect(box, SIGNAL(destroyed()), this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
        return box;
    }
    else if (index.column() == LogicalPortColumns::PRESENCE)
    {
        QComboBox* box = new QComboBox(parent);

        QStringList list = { "required", "optional", "illegal" };
        box->addItems(list);

        connect(box, SIGNAL(destroyed()), this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
        return box;
    }
    else if (index.column() == LogicalPortColumns::SYSTEM_GROUP)
    {
        if (busDefinition_)
        {
            QComboBox* box = new QComboBox(parent);
            box->addItem("");
            box->addItems(BusDefinitionUtils::getSystemGroups(busDefinition_, libraryAccess_));

            connect(box, SIGNAL(destroyed()), this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
            return box;
        }
        else
        {
            return 0;
        }
    }
    else if (index.column() == LogicalPortColumns::MATCH)
    {
        QComboBox* box = new QComboBox(parent);
        box->addItems({"true", "false"});

        connect(box, SIGNAL(destroyed()), this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
        return box;
    }
    else if (index.column() == LogicalPortColumns::QUALIFIER)
    {
        QualifierEditor* qualifierEditor = new QualifierEditor(stdRevision_, parent);
        qualifierEditor->setupEditor(getQualifierList());
        connect(qualifierEditor, SIGNAL(finishEditing()), this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
        connect(qualifierEditor, SIGNAL(cancelEditing()), this, SLOT(onEditingCanceled()), Qt::UniqueConnection);
        return qualifierEditor;
    }
    else
    {
        return ExpressionDelegate::createEditor(parent, option, index);
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsDelegate::setEditorData()
//-----------------------------------------------------------------------------
void AbstractionPortsDelegate::setEditorData(QWidget* editor, QModelIndex const& index ) const
{
    if (editorIsLineEditor(index.column()))
    {
        QLineEdit* line = qobject_cast<QLineEdit*>(editor);
        Q_ASSERT_X(line, "BusPortsDelegate::setEditorData", "Type conversion failed for QLineEdit");

        QString text = index.data(Qt::DisplayRole).toString();
        line->setText(text);

        if (index.column() == LogicalPortColumns::NAME && text == QLatin1String("unnamed"))
        {
            line->clear();
        }
    }
    else if (editorIsComboBox(index.column()))
    {
        QComboBox* box = qobject_cast<QComboBox*>(editor);
        Q_ASSERT_X(box, "BusPortsDelegate::setEditorData", "Type conversion failed for combo box");

        QString text = index.data(Qt::DisplayRole).toString();
        int textIndex = box->findText(text);
        if (textIndex >= 0)
        {
            box->setCurrentIndex(textIndex);
        }
        else
        {
            box->setCurrentText(text);
        }
    }
    else if (index.column() == LogicalPortColumns::QUALIFIER)
    {
        QualifierEditor* qualifierEditor = qobject_cast<QualifierEditor*>(editor);
        Q_ASSERT_X(qualifierEditor, "AbstractionPortsDelegate::setEditorData",
            "Type conversion failed for QualifierEditor");

        auto setQualifiers = index.data(Qt::EditRole).value<QualifierData>();
        
        // Hide all attributes before showing only set attributes. Done here to make editor the right width.
        qualifierEditor->hideAllAttributes();
        qualifierEditor->setupEditorData(setQualifiers.activeQualifiers_, setQualifiers.attributes_);
    }
    else
    {
        ExpressionDelegate::setEditorData(editor, index);
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsDelegate::editorIsLineEditor()
//-----------------------------------------------------------------------------
bool AbstractionPortsDelegate::editorIsLineEditor(int indexColumn) const
{
    return indexColumn == LogicalPortColumns::NAME ||
        indexColumn == LogicalPortColumns::PAYLOADNAME || indexColumn == LogicalPortColumns::PAYLOADEXTENSION;
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsDelegate::editorIsComboBox()
//-----------------------------------------------------------------------------
bool AbstractionPortsDelegate::editorIsComboBox(int indexColumn) const
{
    return indexColumn == LogicalPortColumns::MODE || indexColumn == LogicalPortColumns::PRESENCE ||
        indexColumn == LogicalPortColumns::SYSTEM_GROUP || indexColumn == LogicalPortColumns::PROTOCOLTYPE ||
        indexColumn == LogicalPortColumns::PAYLOADTYPE || indexColumn == LogicalPortColumns::MATCH;
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsDelegate::setModelData()
//-----------------------------------------------------------------------------
void AbstractionPortsDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index)
    const
{
    if (editorIsLineEditor(index.column()))
    {
        QLineEdit* lineEditor = qobject_cast<QLineEdit*>(editor);
        Q_ASSERT_X(lineEditor, "BusPortsDelegate::setModelData", "Type conversion failed for normal QLineEdit");

        model->setData(index, lineEditor->text(), Qt::EditRole);
    }
    else if (editorIsComboBox(index.column()))
    {
        QComboBox* selector = qobject_cast<QComboBox*>(editor);
        Q_ASSERT_X(selector, "BusPortsDelegate::setModelData", "Type conversion failed for combo box");

        model->setData(index, selector->currentText(), Qt::EditRole);
    }
    else if (index.column() == LogicalPortColumns::QUALIFIER)
    {
        QualifierEditor* qualifierEditor = qobject_cast<QualifierEditor*>(editor);
        Q_ASSERT_X(qualifierEditor, "AbstractionPortsDelegate::setModelData", "Type conversion failed for qualifier editor");

        model->setData(index, QVariant::fromValue(qualifierEditor->getQualifierData()), Qt::EditRole);
    }
    else
    {
        ExpressionDelegate::setModelData(editor, model, index);
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsDelegate::commitAndCloseEditor()
//-----------------------------------------------------------------------------
void AbstractionPortsDelegate::commitAndCloseEditor()
{
	QWidget* editor = qobject_cast<QWidget*>(sender());

	if (editor)
    {
		emit commitData(editor);
		emit closeEditor(editor);
	}
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsDelegate::onEditingCanceled()
//-----------------------------------------------------------------------------
void AbstractionPortsDelegate::onEditingCanceled()
{
    QWidget* edit = qobject_cast<QWidget*>(sender());
    if (edit)
    {
        emit closeEditor(edit);
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsDelegate::setBusDef()
//-----------------------------------------------------------------------------
void AbstractionPortsDelegate::setBusDef(QSharedPointer<const BusDefinition> busDefinition)
{
    busDefinition_ = busDefinition;
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsDelegate::updateEditorGeometry()
//-----------------------------------------------------------------------------
void AbstractionPortsDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    if (index.column() != LogicalPortColumns::QUALIFIER)
    {
        return ExpressionDelegate::updateEditorGeometry(editor, option, index);
    }

    int enumerationCount = getQualifierList().count();
    int editorMinimumHeight = 25 * (enumerationCount + 2);

    int editorWidth = editor->sizeHint().width();
    if (editorWidth < 150)
    {
        editorWidth = 150;
    }
    else if (editorWidth > 380)
    {
        editorWidth = 380;
    }

    editor->setFixedWidth(editorWidth);

    const int PARENT_HEIGHT = editor->parentWidget()->height();
    const int AVAILABLE_HEIGHT_BELOW = PARENT_HEIGHT - option.rect.top();

    if (AVAILABLE_HEIGHT_BELOW > editorMinimumHeight)
    {
        editor->move(option.rect.topLeft());
    }
    else
    {
        int editorNewY = PARENT_HEIGHT - editorMinimumHeight;
        if (editorNewY <= 0)
        {
            editorNewY = 0;
        }

        editor->move(option.rect.left(), editorNewY);
    }

    if (editorMinimumHeight > PARENT_HEIGHT)
    {
        editor->setFixedHeight(PARENT_HEIGHT);
    }
    else
    {
        editor->setFixedHeight(editorMinimumHeight);
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsDelegate::getQualifierList()
//-----------------------------------------------------------------------------
QStringList AbstractionPortsDelegate::getQualifierList() const
{
    QStringList list = { "address", "data", };
    if (stdRevision_ != Document::Revision::Std22)
    {
        return list;
    }

    list << QStringLiteral("clock")
        << QStringLiteral("reset")
        << QStringLiteral("valid")
        << QStringLiteral("interrupt")
        << QStringLiteral("clock enable")
        << QStringLiteral("power enable")
        << QStringLiteral("opcode")
        << QStringLiteral("protection")
        << QStringLiteral("flow control")
        << QStringLiteral("user")
        << QStringLiteral("request")
        << QStringLiteral("response");

    return list;
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsDelegate::setModeOptions()
//-----------------------------------------------------------------------------
void AbstractionPortsDelegate::setModeOptions()
{
    if (stdRevision_ == Document::Revision::Std22)
    {
        modeOptions_ = { "initiator", "target", "system" };
    }
    else
    {
        modeOptions_ = { "master", "slave", "system" };
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsDelegate::descriptionColumn()
//-----------------------------------------------------------------------------
int AbstractionPortsDelegate::descriptionColumn() const
{
    return LogicalPortColumns::DESCRIPTION;
}
