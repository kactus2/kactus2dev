/* $Id$
 *
 *  Created on: 21.1.2011
 *      Author: Antti Kamppi
 */

#ifndef ATTRIBUTEBOXPLUGIN_H_
#define ATTRIBUTEBOXPLUGIN_H_

#include <QObject>
#include <QDesignerCustomWidgetInterface>

class AttributeBoxPlugin: public QObject,
                            public QDesignerCustomWidgetInterface {

	Q_OBJECT
	Q_INTERFACES(QDesignerCustomWidgetInterface)

public:

	AttributeBoxPlugin(QObject* parent = 0);

	virtual ~AttributeBoxPlugin();

	QString name() const;

	QString includeFile() const;

	QString group() const;

	QIcon icon() const;

	QString toolTip() const;

	QString whatsThis() const;

	bool isContainer() const;

	QWidget* createWidget(QWidget* parent);

        bool isInitialized() const;

        void initialize(QDesignerFormEditorInterface *core);

private:

        bool initialized;
};

#endif /* ATTRIBUTEBOXPLUGIN_H_ */
