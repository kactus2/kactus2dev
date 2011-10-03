/****************************************************************************
**
** Copyright (C) VCreate Logic Private Limited, Bangalore
**
** Use of this file is limited according to the terms specified by
** VCreate Logic Private Limited, Bangalore.  Details of those terms
** are listed in licence.txt included as part of the distribution package
** of this file. This file may not be distributed without including the
** licence.txt file.
**
** Contact info@vcreatelogic.com if any conditions of this licensing are
** not clear to you.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "ScriptEditorConfigurationPage.h"
#include "ui_ScriptEditorConfigurationPage.h"

#include <QFontDialog>
#include "ScriptEditor.h"

struct GCF::Components::ScriptEditorConfigurationPageData
{
    Ui::ScriptEditorConfigurationPage ui;
    ScriptEditor* scriptEditor;
};

GCF::Components::ScriptEditorConfigurationPage::ScriptEditorConfigurationPage(QWidget* parent)
							    :QWidget(parent)
{
    d = new GCF::Components::ScriptEditorConfigurationPageData;
    d->ui.setupUi(this);
    d->scriptEditor = 0;
}

GCF::Components::ScriptEditorConfigurationPage::~ScriptEditorConfigurationPage()
{
    delete d;
}

void GCF::Components::ScriptEditorConfigurationPage::initFrom(GCF::Components::ScriptEditor* scriptEditor, bool autoSave)
{
    d->ui.tabSize->setValue( scriptEditor->tabSpace() );
    d->ui.fontComboBox->setCurrentFont( QFont(scriptEditor->font().family()) );
//    d->ui.fontComboBox->setCurrentFont( QFont("Helvetica [Cronyx]"));
    d->ui.fontSize->setValue( scriptEditor->fontSize() );

    if(autoSave)
        d->scriptEditor = scriptEditor;
    else
        d->scriptEditor = 0;
}

void GCF::Components::ScriptEditorConfigurationPage::saveInto(GCF::Components::ScriptEditor* scriptEditor)
{
    scriptEditor->setTabSpace( d->ui.tabSize->value() );
    QString str = d->ui.fontComboBox->currentFont().family();
    scriptEditor->setFont( d->ui.fontComboBox->currentFont() );
    int i = d->ui.fontSize->value();
    scriptEditor->setFontSize( d->ui.fontSize->value() );
}

void GCF::Components::ScriptEditorConfigurationPage::on_tabSize_valueChanged()
{
    if(d->scriptEditor)
        this->saveInto(d->scriptEditor);
}

void GCF::Components::ScriptEditorConfigurationPage::on_fontComboBox_currentIndexChanged()
{
    if(d->scriptEditor)
        this->saveInto(d->scriptEditor);
}

void GCF::Components::ScriptEditorConfigurationPage::on_fontSize_valueChanged()
{
    if(d->scriptEditor)
        this->saveInto(d->scriptEditor);
}

void GCF::Components::ScriptEditorConfigurationPage::showEvent(QShowEvent* se)
{
    Q_UNUSED(se);
    if(d->scriptEditor)
        this->initFrom(d->scriptEditor, true);
}

void GCF::Components::ScriptEditorConfigurationPage::hideEvent(QHideEvent* he)
{
    Q_UNUSED(he);
    if(d->scriptEditor)
        this->saveInto(d->scriptEditor);
}
