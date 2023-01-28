#include "DelayTask.h"

DelayTask::DelayTask(QObject* parent)
    : QObject(parent)
{
    m_mode = ContinuousDelay;
	m_timer = new QTimer(this);
	m_timer->setSingleShot(true);
	setDelayTime(1000);

	connect(m_timer, &QTimer::timeout, this, [=] {
		m_func();
	});
}

DelayTask::~DelayTask()
{
}

void DelayTask::setDelayMode(DelayMode mode)
{
    m_mode = mode;
}

void DelayTask::setDelayTime(int msec)
{
	m_delayTime = msec;
	m_timer->setInterval(m_delayTime);
}

void DelayTask::doTask(const std::function<void()>& func)
{
	m_func = func;

	if (m_mode == ContinuousDelay)
    {
        if (!isDelaying())
            m_func();

        m_timer->start();
    }
	else if (m_mode == TimeoutDelay)
	{
        if (!isDelaying())
        {
            m_func();
            m_timer->start();
        }
	}
}

bool DelayTask::isDelaying()
{
	return m_timer->isActive();
}

void DelayTask::stop()
{
	m_timer->stop();
}
