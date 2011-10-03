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

#ifndef MUTEX_LOCKER_H
#define MUTEX_LOCKER_H

#include <QMutex>

namespace GCF
{
	// Unlocks a locked mutex in destructor
	class MutexUnlocker
	{
	public:
		// Ensure that mutex is locked
		MutexUnlocker(QMutex* mutex) {
			m_mutex = mutex;
		}

		// Unlock the mutex
		~MutexUnlocker() {
			if(m_mutex)
				m_mutex->unlock();
		}

	private:
		QMutex* m_mutex;
	};
}

#endif

