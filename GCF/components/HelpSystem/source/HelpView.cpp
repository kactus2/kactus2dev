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

#include "HelpView.h"
#include "HelpSystemComponent.h"
#include "TopicChooserDialog.h"

#include <QHelpEngine>
#include <QDesktopServices>
#include <QMessageBox>
#include <QMainWindow>

struct GCF::Components::HelpViewData
{

};

GCF::Components::HelpView::HelpView(QWidget* parent):QTextBrowser(parent)
{
	d = new GCF::Components::HelpViewData;
    connect(this,SIGNAL(sourceChanged(const QUrl&)),this,SLOT(setFocus()));
}


GCF::Components::HelpView::~HelpView()
{
	delete d;
}

void GCF::Components::HelpView::setSource(const QUrl &url)
{
    bool help = url.toString() == QLatin1String("help");
    QHelpEngine* engine = GCF::Components::HelpSystemComponent::instance().helpEngine();

    if (url.isValid() && !help)
    {
        if(url.scheme() == QLatin1String("http")
                           || url.scheme() == QLatin1String("ftp")
                           || url.scheme() == QLatin1String("mailto")
                           || url.path().endsWith(QLatin1String("pdf")))
        {
            bool launched = QDesktopServices::openUrl(url);
            if (!launched)
                QMessageBox::information(this, "Error", "Unable to launch web browser");
        }
        else
        {
            QUrl u = engine->findFile(url);
            if(u.toString().isEmpty())
            {
                QMessageBox::information(this,"Error","Documentation not found");
            }
            if (u.isValid())
            {
                QTextBrowser::setSource(u);

                // TODO
                // Change the window title to something shorter, yet appropriate
                setWindowTitle( this->documentTitle() );
            }
        }
    }
}

void GCF::Components::HelpView::setFontName(const QString& fontName)
{
    QFont font = this->font();
    if( font.family() == fontName )
        return;

    font.setFamily( fontName );
    setFont( font );
}

QString GCF::Components::HelpView::fontName() const
{
    return this->font().family();
}

void GCF::Components::HelpView::setFontSize(int size)
{
    QFont font = this->font();
    if( font.pointSize() == size )
        return;

    font.setPointSize( size );
    setFont( font );
}

int GCF::Components::HelpView::fontSize() const
{
    return this->font().pointSize();
}

QVariant GCF::Components::HelpView::loadResource(int type, const QUrl &name)
{
	QByteArray ba;
    QHelpEngine* engine = GCF::Components::HelpSystemComponent::instance().helpEngine();
    if (type < 4 && engine)
    {
        QUrl url(name);
        if (name.isRelative())
			url = source().resolved(url);
        ba = engine->fileData(url);
    }

    return ba;
}

void GCF::Components::HelpView::showHelp(const QUrl& url)
{
	//qDebug(" URL Is : %s", qPrintable(url.toString()));
	QHelpEngine* engine = GCF::Components::HelpSystemComponent::instance().helpEngine();
    if (!engine)
	    return;

    setSource(url);
    scrollToAnchor(url.fragment());
}

void GCF::Components::HelpView::showHelp( const QMap<QString, QUrl> & links, const QString & keyword)
{
    GCF::Components::TopicChooserDialog topicChooserDlg(this);
    connect(&topicChooserDlg, SIGNAL(topicSelected(QUrl)),this,SLOT(showHelp(QUrl)));
    topicChooserDlg.showTopics(links,keyword);
    topicChooserDlg.exec();
}

void GCF::Components::HelpView::zoomIn()
{
    QTextBrowser::zoomIn();
}

void GCF::Components::HelpView::zoomOut()
{
    QTextBrowser::zoomOut();
}


