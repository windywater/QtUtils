#pragma once
#include <QObject>
#include <QTimer>
#include <functional>

/*
* ��ʱ���񣬷�Ϊ ContinuousDelay��TimeoutDelay����ģʽ�����û����� 1s ��ʱΪ����
* ContinuousDelay ģʽ�� ���û�һ�μ��� 1s �����һ�� change �¼�����ͣ�û������У�������� change �¼���
* ֻ�����һ���û�ͣ���� 1s �󣬲Ž�������һ�� change �¼���
* TimeoutDelay ģʽ�� ��ͣ�û������У�����ÿ 1s һ�β���һ�� change �¼���
*/
class DelayTask : public QObject
{
	Q_OBJECT
public:
	enum DelayMode
	{
		ContinuousDelay,
		TimeoutDelay,
	};

    DelayTask(QObject* parent = Q_NULLPTR);
	~DelayTask();

    void setDelayMode(DelayMode mode);
	void setDelayTime(int msec);
    void doTask(const std::function<void()>& func);
	bool isDelaying();
	void stop();
private:
	DelayMode m_mode;
	int m_delayTime;
	std::function<void()> m_func;
	QTimer* m_timer;
};
