#include "DelayTask.h"

DelayTask::DelayTask(DelayMode mode, QObject* parent)
	: QObject(parent),
	m_mode(mode)
{
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

void DelayTask::setDelayTime(int msec)
{
	m_delayTime = msec;
	m_timer->setInterval(m_delayTime);
}

void DelayTask::doTask(std::function<void()> func)
{
	m_func = func;

	if (m_mode == ContinuousDelay)
		m_timer->start();
	else if (m_mode == TimeoutDelay)
	{
		if (!isDelaying())
			m_timer->start();
	}
	else if (m_mode == Prevent)
	{
		// 若不在计时中，立即执行；若在计时中，当前不执行，计时停止时执行
		if (!isDelaying())
		{
			m_func();
			m_func = [=]() {};	// 计时停止时，执行空任务
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
