/* 
 *	Created on: 1.3.2013
 *	Author:		Antti Kamppi
 * 	File name:	parametercombobox.cpp
 * 	Project:	Kactus 2
*/

#include "parametercombobox.h"

//-----------------------------------------------------------------------------
// Function: parametercombobox::ParameterComboBox()
//-----------------------------------------------------------------------------
ParameterComboBox::ParameterComboBox(QSharedPointer<ParameterFinder> parameterFinder,
	QWidget *parent,
	bool editable /*= true*/ ):
QComboBox(parent),
parameterFinder_(parameterFinder)
{
	setEditable(editable);

	connect(this, SIGNAL(editTextChanged(const QString&)),
		this, SLOT(onEditChanged(const QString&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: parametercombobox::~ParameterComboBox()
//-----------------------------------------------------------------------------
ParameterComboBox::~ParameterComboBox()
{

}

//-----------------------------------------------------------------------------
// Function: parametercombobox::refresh()
//-----------------------------------------------------------------------------
void ParameterComboBox::refresh()
{	
    // the items are added without emitting signals
    disconnect(this, SIGNAL(editTextChanged(const QString&)),
        this, SLOT(onEditChanged(const QString&)));

    // remove previous items
    clear();

    QStringList allParameterIds = parameterFinder_->getAllParameterIds();

    foreach (QString valueId, allParameterIds)
    {
        QString itemName = parameterFinder_->nameForId(valueId);
        if (!itemName.isEmpty())
        {
            addItem(itemName, valueId);
        }
    }

	connect(this, SIGNAL(editTextChanged(const QString&)),
		this, SLOT(onEditChanged(const QString&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: parametercombobox::onEditChanged()
//-----------------------------------------------------------------------------
void ParameterComboBox::onEditChanged( const QString& text )
{
	emit textChanged(text);
}

//-----------------------------------------------------------------------------
// Function: parametercombobox::setRemapText()
//-----------------------------------------------------------------------------
void ParameterComboBox::setRemapText( const QString& text )
{
	// the text is set without emitting signals
	disconnect(this, SIGNAL(editTextChanged(const QString&)),
		this, SLOT(onEditChanged(const QString&)));

	setCurrentText(text);

	connect(this, SIGNAL(editTextChanged(const QString&)),
		this, SLOT(onEditChanged(const QString&)), Qt::UniqueConnection);
}
