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

#ifndef CONFIGURATION_DIALOG_COMPONENT_H
#define CONFIGURATION_DIALOG_COMPONENT_H

#include "AbstractComponent.h"

namespace GCF
{
	struct ConfigurationDialogComponentData;
	class GCF_EXPORT_COMPONENT ConfigurationDialogComponent : public AbstractComponent
	{
		Q_OBJECT
		GCF_DECLARE_COMPONENT(ConfigurationDialogComponent)

	public:
		ConfigurationDialogComponent();
		~ConfigurationDialogComponent();

	protected:
		QWidget* fetchWidget(const QString& completeName) const;
		void addChildWidget(QWidget* parent, QWidget* child, const QString& hint);
		void initializeComponent();
		void finalizeComponent();
		void creationAnnounced();

	private:
		ConfigurationDialogComponentData* d;
	};
}

#endif
