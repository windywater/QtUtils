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
		// �����ڼ�ʱ�У�����ִ�У����ڼ�ʱ�У���ǰ��ִ�У���ʱֹͣʱִ��
		if (!isDelaying())
		{
			m_func();
			m_func = [=]() {};	// ��ʱֹͣʱ��ִ�п�����
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
