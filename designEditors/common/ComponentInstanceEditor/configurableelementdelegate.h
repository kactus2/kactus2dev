/* 
 *  	Created on: 12.8.2011
 *      Author: Antti Kamppi
 * 		filename: configurableelementdelegate.h
 *		Project: Kactus 2
 */

#ifndef CONFIGURABLEELEMENTDELEGATE_H
#define CONFIGURABLEELEMENTDELEGATE_H

#include <editors/ComponentEditor/common/ExpressionDelegate.h>
#include <editors/ComponentEditor/common/ParameterFinder.h>

/*! \brief Delegate that provides editors to edit the configurable element values.
 *
 */
class ConfigurableElementDelegate : public ExpressionDelegate
 {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param component Pointer to the component that defines the component instance type.
	 * \param parent Pointer to the owner of this delegate.
	 *
	*/
	ConfigurableElementDelegate(QCompleter* parameterCompleter, QSharedPointer<ParameterFinder> parameterFinder,
        QObject *parent);
	
	//! \brief The destructor
	virtual ~ConfigurableElementDelegate();

protected:

    /*!
     *  Checks if the given column supports expressions in the editor.
     *
     *      @param [in] column      The column to check.
     *
     *      @return True, if the cells in the column accept expressions, otherwise false.
     */
    bool columnAcceptsExpression(int column) const;

private:
	//! \brief No copying
	ConfigurableElementDelegate(const ConfigurableElementDelegate& other);

	//! \brief No assignment
	ConfigurableElementDelegate& operator=(const ConfigurableElementDelegate& other);
};

#endif // CONFIGURABLEELEMENTDELEGATE_H
