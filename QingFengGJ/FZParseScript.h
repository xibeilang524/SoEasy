#pragma once

#include <QObject>
#include <QMap>
#include <QTimer>
class FZParseScript : public QObject
{
	Q_OBJECT
public:
	FZParseScript();
	~FZParseScript();
	enum SCRIPT_CTRL
	{
		SCRIPT_CTRL_STOP = 0,	//停止
		SCRIPT_CTRL_RUN = 1,	//执行
		SCRIPT_CTRL_PAUSE = 2,//暂停
	};
	enum SCRIPT_TIMER
	{
		SCRIPT_TIMER_RUN = 1,	//脚本定时器
		SCRIPT_TIMER_SYS = 2,	//系统指令定时器
	};

	QString getGameDescript() { return m_szScriptDesc; }
	QStringList GetGameScriptData() { return m_scriptDataList; }
	int GetGameScriptRunRow() { return m_scriptRunRow; }
	int GetGameScriptCtrlStatus() { return m_scriptCtrl; }
	static FZParseScript& getInstance();
	void ParseGameScript(const QString& szPath);
	void SetScriptRunRow(int nRow);
	bool RunScript();
	bool StopScript();
	bool PauseScript();
	void ParseSet(const QString& szData);
	void setTimerCmd(int nType,int nTimer);
	void setAutoActionCmd(int nType, int nVal);

	bool nop();
	bool nowhile(const QString& szData);
	bool moveto(const QString& szData);
	bool turn_about(const QString& szData);
	bool high(const QString& szData);
	bool work(const QString& szData);
	bool renew(const QString& szData);

	QString MidBrackets(const QString& szData);
	void ParseInputVal(const QString& szData);

	void ParseIFCmd(const QString& szData);
	static void RunScriptThread(FZParseScript* pThis);
signals:
	void updateScriptRow(int nRow);
	void refreshScriptUI();
private:
//	QMap<int, QString> m_script;
	QString m_szScriptDesc;	//脚本描述信息
	int m_scriptCtrl;		//脚本控制
	int m_scriptRunRow;		//脚本当前运行位置
	QStringList m_scriptDataList;
	QMap<int,QString > m_mark;//goto 标志名称对应的脚本行
	QTimer m_scriptRunTimer;		//脚本运行定时器 单位是毫秒，默认值为320，一般不需要进行改动。此值越小脚本运行的速度越快，但并不意味着效率会提高
	int m_tScriptTimerInterval;	//脚本运行间隔
	int m_tSysCmdTimerInterval;	//系统指令定时器间隔
	QMap<QString, int> m_userDefVariable;//用户自定义变量
};
