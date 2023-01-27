#pragma once
#include <QObject>
#include <QTimer>
#include <functional>

/*
* 延时覆盖任务，分为 ContinuousDelay、TimeoutDelay、Prevent 三种模式。以敲击键盘 1s 延时为例：
* ContinuousDelay 模式： 若敲击一次键盘 1s 后产生一次 change 事件，则不停敲击键盘中，不会产生 change 事件，
* 只有最后一次敲击停下来 1s 后，才仅仅产生一次 change 事件。
* TimeoutDelay 模式： 不停敲击键盘中，仍以每 1s 一次产生一次 change 事件。
* 以上两种模式中，后面的任务都会覆盖前面的任务，适用于频繁动作下的重复任务。
* Prevent 模式：计时器活动时，任务立即执行，否则暂不执行，计时停止时执行。
*/
class DelayTask : public QObject
{
	Q_OBJECT
public:
	enum DelayMode
	{
		ContinuousDelay,
		TimeoutDelay,
		Prevent
	};

	DelayTask(DelayMode mode, QObject* parent = Q_NULLPTR);
	~DelayTask();

	void setDelayTime(int msec);
	void doTask(std::function<void()> func);
	bool isDelaying();
	void stop();
private:
	DelayMode m_mode;
	int m_delayTime;
	std::function<void()> m_func;
	QTimer* m_timer;
};