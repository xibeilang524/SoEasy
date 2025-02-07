#if _MSC_VER >= 1600          ///1600为vs2010编译器版本 
#pragma execution_character_set("utf-8")        ///c++11特性  设置执行编码 
#endif 
#include "MLAssist.h"
#include <QDebug>
#include "../include/ITPublic.h"
#include <math.h>
#include <QtConcurrent> 
#include <QMessageBox>
#include "../include/qxtglobalshortcut5/QxtGlobalShortcut.h"
#include "FZParseScript.h"
#include "GameData.h"
MLAssist::MLAssist(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	m_KeyStart = 0;
	m_KeyStop = 1;
	m_gameHwnd = nullptr;
	m_gameProcessID = 0;
	m_autoBattle = false;
	m_screenHDC = GetDC(nullptr);
	ui.tableWidget->setRowCount(10);
	for (int i=0;i<10;++i)
	{
		for (size_t n = 0; n < 5; n++)
		{
			QTableWidgetItem* pItem = new QTableWidgetItem();
			ui.tableWidget->setItem(i, n, pItem);
		}		
	}
	ui.tableWidget->horizontalHeader()->setStyleSheet("font:bold;");
//	ui.tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui.tableWidget->verticalHeader()->setVisible(false);
//	ui.tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui.tableWidget->horizontalHeader()->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	ui.tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui.tableWidget->horizontalHeader()->setStretchLastSection(true);
//	ui.tableWidget->horizontalHeader()->setFixedHeight(30);
	ui.tableWidget->setColumnWidth(1,130);
//	ui.tableWidget->resizeColumnsToContents();//根据内容调整列宽 但每次都变 太麻烦 修改下
	memset(m_Infos, 0, sizeof(PERSONINFO) * 20);
	initPersonPos();
	connect(&m_getBattleTimer,SIGNAL(timeout()),this,SLOT(doUpdateBattleInfo()));
	connect(this, SIGNAL(signal_autoBattle()), this, SLOT(on_autoBattle()));
//	initComboBox();
}

MLAssist::~MLAssist()
{
	m_getBattleTimer.stop();
}

void MLAssist::initComboBox()
{
	for (int i = 0; i < 11; ++i)
	{
		QString szItem = QString("F%1").arg(i + 1);
		ui.comboBox_start->addItem(szItem, i);
		ui.comboBox_stop->addItem(szItem, i);
	}
	ui.comboBox_start->setCurrentIndex(m_KeyStart);
	ui.comboBox_stop->setCurrentIndex(m_KeyStop);
	ui.comboBox_start->removeItem(m_KeyStop);
	ui.comboBox_stop->removeItem(m_KeyStart);
	
	QxtGlobalShortcut *shortcut = new QxtGlobalShortcut(this);
	if (shortcut)
	{
		if (shortcut->setShortcut(QKeySequence(ui.comboBox_start->currentText()), (HWND)NULL) == FALSE)
		{
			QMessageBox::information(this, "提示", "注册启动热键失败", QMessageBox::Ok);
		}
		connect(shortcut, SIGNAL(activated(const QKeySequence&)), this, SLOT(dealHotKeyEvent(const QKeySequence&)));
	}
	QxtGlobalShortcut *shortcut2 = new QxtGlobalShortcut(this);
	if (shortcut2)
	{
		if (shortcut2->setShortcut(QKeySequence(ui.comboBox_stop->currentText()), (HWND)NULL) == FALSE)
		{
			QMessageBox::information(this, "提示", "注册停止热键失败", QMessageBox::Ok);
		}
		connect(shortcut2, SIGNAL(activated(const QKeySequence&)), this, SLOT(dealHotKeyEvent(const QKeySequence&)));
	}
}
void MLAssist::dealHotKeyEvent(const QKeySequence& key)
{
	if (key == QKeySequence(ui.comboBox_start->currentText()))
	{
		//开始同步
		ui.checkBox_autoBattle->setChecked(true);
		on_checkBox_autoBattle_clicked();
	}
	else if (key == QKeySequence(ui.comboBox_stop->currentText()))
	{
		//停止同步
		ui.checkBox_autoBattle->setChecked(false);
		on_checkBox_autoBattle_clicked();
	}
}

void MLAssist::initPersonPos()
{
	pointYes.x = 245;
	pointYes.y = 317;
	pointNo.x = 385;
	pointNo.y = 317;
	pointEscape.x = 600;
	pointEscape.y = 50;
	pointAttack.x = 411;
	pointAttack.y = 35;
	pointDefense.x = 429;
	pointDefense.y = 63;
	pointSkill.x = 517;
	pointSkill.y = 23;
	pointThings.x = 519;
	pointThings.y = 48;
	memset(m_personPos, 0, sizeof(POINT) * 20);
	m_personPos[0].x = 480;
	m_personPos[0].y = 340;
	m_personPos[1].x = 540;
	m_personPos[1].y = 306;
	m_personPos[2].x = 412;
	m_personPos[2].y = 362;
	m_personPos[3].x = 610;
	m_personPos[3].y = 270;
	m_personPos[4].x = 480;
	m_personPos[4].y = 340;

	m_personPos[5].x = 410;
	m_personPos[5].y = 280;
	m_personPos[6].x = 480;
	m_personPos[6].y = 250;
	m_personPos[7].x = 350;
	m_personPos[7].y = 326;
	m_personPos[8].x = 540;
	m_personPos[8].y = 180;
	m_personPos[9].x = 480;
	m_personPos[9].y = 340;
	m_personPos[10].x = 160;
	m_personPos[10].y = 150;
	m_personPos[11].x = 230;
	m_personPos[11].y = 120;
	m_personPos[12].x = 100;
	m_personPos[12].y = 190;
	m_personPos[13].x = 296;
	m_personPos[13].y = 83;	
	m_personPos[14].x = 44;
	m_personPos[14].y = 233;
	m_personPos[15].x = 227;
	m_personPos[15].y = 210;
	m_personPos[16].x = 291;
	m_personPos[16].y = 172;
	m_personPos[17].x = 164;
	m_personPos[17].y = 243;
	m_personPos[18].x = 351;
	m_personPos[18].y = 139;
	m_personPos[19].x = 93;
	m_personPos[19].y = 273;
	for (size_t i = 0; i < 10; i++)
	{
		m_skillPos[i].x = 470;
		m_skillPos[i].y = 155 + (i) * 24;
			/*SkillItem(I).X = 498
			SkillItem(I).Y = CInt(125 + (I - 1) * 22.5)*/
	}
}

void MLAssist::SetPersonInfo(PERSONINFO& info, char* name, int hp, int totalhp, int mp, int totalmp, int level)
{
	info.hp = hp;
	info.totalhp = totalhp;
	info.mp = mp;
	info.totalmp = totalmp;
	info.level = level;
	lstrcpyW(info.name,ANSITOUNICODE1(name));
	qDebug() << info.name;
	//MultiByteToWideChar(CP_ACP, 0, name, strlen(name), info.name, MAXBUFLEN);
}

void MLAssist::refreshBattleUI()
{
	for (int i = 0; i < 20; i++)
	{
		PERSONINFO info = m_Infos[i];
		QString szLabelName = QString("label_%1").arg(i);
		QLabel* pLabel = this->findChild<QLabel *>(szLabelName);
		if (pLabel)
		{
			WCHAR buf[MAXBUFLEN] = { 0 };
			if (wcslen(info.name) > 0)//
			{
				QString szShowText = QString("%1\n%2/%3\n%4").arg(QString::fromWCharArray(info.name)).arg(info.hp).arg( info.totalhp).arg(info.level);
	//			qDebug() << szShowText << info.name << info.hp << info.totalhp << info.level;
				pLabel->setText(szShowText);	
			}
			else
			{
				pLabel->setText("");
			}
		}
	}
}

void MLAssist::readBattleInfo()
{
	char* pText = YunLai::ReadMemoryStrFromProcessID(m_gameProcessID, "0x005590E0", 1000);//0x005590E4
	if (strlen(pText) < 1)
		return;
	LPWSTR wText = ANSITOUNICODE1(pText);
	QString szBattleInfo = QString::fromStdWString(wText);// YunLai::ReadMemoryStrFromProcessID(m_gameProcessID, "0x005590E0", 1000));
	QStringList szBattleInfoList = szBattleInfo.split("|");//11个为一组战斗信息
	int nPersonSize = szBattleInfoList.size() / 11;
	for(int i = 0;i< nPersonSize;++i)
	{
		QStringList szTemps = szBattleInfoList.mid(i*11,11);
	//	qDebug() << szTemps;
		int nPos = szTemps.at(2).toInt(nullptr, 16);
		QString szName = szTemps.at(3);
		lstrcpyW(m_Infos[nPos].name, szName.toStdWString().c_str());
		m_Infos[nPos].level = szTemps.at(5).toInt(nullptr,16);
		m_Infos[nPos].hp = szTemps.at(6).toInt(nullptr, 16);
		m_Infos[nPos].totalhp = szTemps.at(7).toInt(nullptr, 16);
		m_Infos[nPos].mp = szTemps.at(8).toInt(nullptr, 16);
		m_Infos[nPos].totalmp = szTemps.at(9).toInt(nullptr, 16);
	}
}

void MLAssist::setLoginUser(const QString& szText)
{
	QTableWidgetItem* pItem = ui.tableWidget->item(0, 0);
	pItem->setText(szText);
}

void MLAssist::setItemText(int row, int col, const QString& szText)
{
	QTableWidgetItem* pItem = ui.tableWidget->item(row, col);
	pItem->setText(szText);
}

void MLAssist::on_autoBattle()
{
	QtConcurrent::run(AttackThread, this);
}

void MLAssist::on_pushButton_move_clicked()
{
	int x = ui.lineEdit_x->text().toInt();
	int y = ui.lineEdit_y->text().toInt();
//	MoveToGamePoint(x, y,10,100);
	moveToPoint(x,y);
}

void MLAssist::on_pushButton_up_clicked()
{
	faceToDirection(MOVE_DIRECTION_UP);
	return;
	POINT points;
	long resX = 320;
	long resY = 240 - MOVE_STEP_DIAGONAL;
	points.x = resX;
	points.y = resY;
	//ClientToScreen(m_gameHwnd, &points);
		//YunLai::SetFocusToWnd(m_gameHwnd);//已经是焦点了 不需要
	LPARAM newl = MAKELPARAM(points.x, points.y);
	SendMessage(m_gameHwnd, WM_MOUSEMOVE, WM_MOUSEMOVE, newl);
	SendMessage(m_gameHwnd, WM_LBUTTONDOWN, WM_LBUTTONDOWN, newl);
	SendMessage(m_gameHwnd, WM_LBUTTONUP, WM_LBUTTONUP, newl);
}

void MLAssist::on_pushButton_down_clicked()
{
	faceToDirection(MOVE_DIRECTION_DOWN);
	return;
	POINT points;
	long resX = 320;
	long resY = 240 + MOVE_STEP_DIAGONAL;  //考虑边界 坐标是一个32宽像素的方框 
	points.x = resX;
	points.y = resY;
	//ClientToScreen(m_gameHwnd, &points);
	//YunLai::SetFocusToWnd(m_gameHwnd);//已经是焦点了 不需要
	LPARAM newl = MAKELPARAM(points.x, points.y);
	SendMessage(m_gameHwnd, WM_MOUSEMOVE, WM_MOUSEMOVE, newl);
	SendMessage(m_gameHwnd, WM_LBUTTONDOWN, WM_LBUTTONDOWN, newl);
	SendMessage(m_gameHwnd, WM_LBUTTONUP, WM_LBUTTONUP, newl);
}

void MLAssist::on_pushButton_left_clicked()
{
	faceToDirection(MOVE_DIRECTION_LEFT);
	return;
	POINT points;
	long resX = 320- MOVE_STEP_DIAGONAL;
	long resY = 240;
	points.x = resX;
	points.y = resY;
//	ClientToScreen(m_gameHwnd, &points);
//	YunLai::SetFocusToWnd(m_gameHwnd);//已经是焦点了 不需要
	LPARAM newl = MAKELPARAM(points.x, points.y);
	SendMessage(m_gameHwnd, WM_MOUSEMOVE, WM_MOUSEMOVE, newl);
	SendMessage(m_gameHwnd, WM_LBUTTONDOWN, WM_LBUTTONDOWN, newl);
	SendMessage(m_gameHwnd, WM_LBUTTONUP, WM_LBUTTONUP, newl);
}

void MLAssist::on_pushButton_right_clicked()
{
	faceToDirection(MOVE_DIRECTION_RIGHT);
	return;
	POINT points;
	long resX = 320+ MOVE_STEP_DIAGONAL;
	long resY = 240;
	points.x = resX;
	points.y = resY;
	//ClientToScreen(m_gameHwnd, &points);
	//YunLai::SetFocusToWnd(m_gameHwnd);//已经是焦点了 不需要
	LPARAM newl = MAKELPARAM(points.x, points.y);
	SendMessage(m_gameHwnd, WM_MOUSEMOVE, WM_MOUSEMOVE, newl);
	SendMessage(m_gameHwnd, WM_LBUTTONDOWN, WM_LBUTTONDOWN, newl);
	SendMessage(m_gameHwnd, WM_LBUTTONUP, WM_LBUTTONUP, newl);
}

void MLAssist::on_pushButton_color_clicked()
{
	QString szPath = "E:\\游戏用\\外挂目录\\ml\\5zhuan\\封印专用\\抓蜜蜂.script";
	FZParseScript::getInstance().ParseGameScript(szPath);
	FZParseScript::getInstance().RunScript();
	return;
	COLORREF tempc = GetPixel(m_gameHDC, 340, 240);// 486, 130);

	COLORREF crTemp = GetPixel(m_gameHDC, 310, 212);// 356, 27);
	BYTE pixelR = GetRValue(crTemp);
	BYTE pixelG = GetGValue(crTemp);
	BYTE pixelB = GetBValue(crTemp);

	ui.label_color->setText(QString::number(crTemp));
	QPalette palette;
	palette.setColor(QPalette::Background, QColor(pixelR, pixelG, pixelB));
//	ui.label_color->setPalette(QString("background-color:%1").arg(QColor(tempc)));
}

void MLAssist::on_pushButton_clicked()
{
	ui.comboBox->clear();
	QMap<qint64, QString> processData;
	YunLai::GetAllProcess(processData);
	for (auto it = processData.begin(); it != processData.end(); ++it)
	{
		if (it.value().contains("qfmoli"))
		{			
			QString szLoginUser = YunLai::ReadMemoryStrFromProcessID(it.key(), "00E39EE4", 100);
			ui.comboBox->addItem(QString("%1 %2 %3").arg(it.key()).arg(it.value()).arg(szLoginUser), it.key());
		}
	}
}

void MLAssist::on_pushButton_hook_clicked()
{
	m_gameProcessID = ui.comboBox->currentData(Qt::UserRole).toInt();
	m_gameHwnd = YunLai::FindMainWindow(m_gameProcessID);
	m_gameHDC = GetDC(m_gameHwnd);
	GameData::getInstance().setGameProcess(m_gameProcessID);
	GameData::getInstance().setGameHwnd(m_gameHwnd);
	GameData::getInstance().setGameHDC(m_gameHDC);
	QString szLoginUser = YunLai::ReadMemoryStrFromProcessID(m_gameProcessID, "00E39EE4",100);
	QString szGameUserName = QString::fromWCharArray(ANSITOUNICODE1(YunLai::ReadMemoryStrFromProcessID(m_gameProcessID, "00E35878", 100)));
	QString szMapName = QString::fromWCharArray(ANSITOUNICODE1(YunLai::ReadMemoryStrFromProcessID(m_gameProcessID, "009181C0", 100)));

	
	int nEast = YunLai::ReadMemoryIntFromProcessID(m_gameProcessID, "0092BD28");
	int nSouth = YunLai::ReadMemoryIntFromProcessID(m_gameProcessID, "0092BD30");
	setItemText(0,0, szLoginUser);
	setItemText(1, 0, szGameUserName);
	setItemText(2, 0, szMapName);
	setItemText(3, 0, QString("东%1 南%2").arg(nEast).arg(nSouth));
	setItemText(4, 0, QString("连接有效"));
	int nHP = YunLai::ReadMemoryIntFromProcessID(m_gameProcessID, "00E2A418");
	int nTotHP = YunLai::ReadMemoryIntFromProcessID(m_gameProcessID, "00E2A41C");
	int nMP = YunLai::ReadMemoryIntFromProcessID(m_gameProcessID, "00E2A420");
	int nTotMP = YunLai::ReadMemoryIntFromProcessID(m_gameProcessID, "00E2A424");
	QString szHp = QString("HP:%1/%2").arg(nHP).arg(nTotHP);
	QString szMp = QString("MP:%1/%2").arg(nMP).arg(nTotMP);
	qDebug() << szHp << szMp;
	setItemText(0, 1, szHp);
	setItemText(1, 1, szMp);
	//	qDebug() << m_gameProcessID << m_gameHwnd << szEast;
	//获取升级经验 
	int nExperience = YunLai::ReadMemoryIntFromProcessID(m_gameProcessID, "00E2A438");
	int nNextExperience = YunLai::ReadMemoryIntFromProcessID(m_gameProcessID, "00E2A43C");
	setItemText(2, 1, QString("升:%1").arg(nNextExperience-nExperience));
	int nAttack = YunLai::ReadMemoryIntFromProcessID(m_gameProcessID, "00E2A460");
	setItemText(3, 1, QString("力:%1").arg(nAttack));
	int nDefence = YunLai::ReadMemoryIntFromProcessID(m_gameProcessID, "00E2A464");
	setItemText(4, 1, QString("防:%1").arg(nDefence));
	int nAgility = YunLai::ReadMemoryIntFromProcessID(m_gameProcessID, "00E2A468");
	setItemText(5, 1, QString("敏:%1").arg(nAgility));
	int nMind = YunLai::ReadMemoryIntFromProcessID(m_gameProcessID, "00E2A46C");
	setItemText(6, 1, QString("精:%1").arg(nMind));
	int nCharm = YunLai::ReadMemoryIntFromProcessID(m_gameProcessID, "00E2A474");
	setItemText(7, 1, QString("魅:%1").arg(nCharm));

	int nEarth = YunLai::ReadMemoryIntFromProcessID(m_gameProcessID, "00E2A4A4");
	int nWater = YunLai::ReadMemoryIntFromProcessID(m_gameProcessID, "00E2A4A8");
	int nFire = YunLai::ReadMemoryIntFromProcessID(m_gameProcessID, "00E2A4AC");
	int nWind = YunLai::ReadMemoryIntFromProcessID(m_gameProcessID, "00E2A4B0");
	QString szAttribute = "属:";
	if(nEarth>0)
		szAttribute += QString(" 地%1").arg(nEarth);
	if (nWater > 0)
		szAttribute += QString(" 水%1").arg(nWater);
	if (nFire > 0)
		szAttribute += QString(" 火%1").arg(nFire);
	if (nWind > 0)
		szAttribute += QString(" 风%1").arg(nWind);
	setItemText(8, 1, szAttribute);	
	int nGold = YunLai::ReadMemoryIntFromProcessID(m_gameProcessID, "00E2A4BC");
	setItemText(9, 1, QString("钱:%1").arg(nGold));

	char* skill = YunLai::ReadMemoryStrFromProcessID(m_gameProcessID, "00D84FEC",100);
	QString qSkill = QString::fromWCharArray(ANSITOUNICODE1(skill));
	setItemText(0, 2, qSkill);
	skill = YunLai::ReadMemoryStrFromProcessID(m_gameProcessID, "00D899E8", 100);
	qSkill = QString::fromWCharArray(ANSITOUNICODE1(skill));
	setItemText(1, 2, qSkill);	
	skill = YunLai::ReadMemoryStrFromProcessID(m_gameProcessID, "D8E3E4", 100);
	qSkill = QString::fromWCharArray(ANSITOUNICODE1(skill));
	setItemText(2, 2, qSkill);
	skill = YunLai::ReadMemoryStrFromProcessID(m_gameProcessID, "D92DE0", 100);
	qSkill = QString::fromWCharArray(ANSITOUNICODE1(skill));
	setItemText(3, 2, qSkill);
	skill = YunLai::ReadMemoryStrFromProcessID(m_gameProcessID, "D977DC", 100);
	qSkill = QString::fromWCharArray(ANSITOUNICODE1(skill));
	setItemText(4, 2, qSkill);
	skill = YunLai::ReadMemoryStrFromProcessID(m_gameProcessID, "D9C1D8", 100);
	qSkill = QString::fromWCharArray(ANSITOUNICODE1(skill));
	setItemText(5, 2, qSkill);
	skill = YunLai::ReadMemoryStrFromProcessID(m_gameProcessID, "DA0BD4", 100);
	qSkill = QString::fromWCharArray(ANSITOUNICODE1(skill));
	setItemText(6, 2, qSkill);
	skill = YunLai::ReadMemoryStrFromProcessID(m_gameProcessID, "DA55D0", 100);
	qSkill = QString::fromWCharArray(ANSITOUNICODE1(skill));
	setItemText(7, 2, qSkill);
	skill = YunLai::ReadMemoryStrFromProcessID(m_gameProcessID, "DA9FCC", 100);
	qSkill = QString::fromWCharArray(ANSITOUNICODE1(skill));
	setItemText(8, 2, qSkill);
	skill = YunLai::ReadMemoryStrFromProcessID(m_gameProcessID, "DAE9C8", 100);
	qSkill = QString::fromWCharArray(ANSITOUNICODE1(skill));
	setItemText(9, 2, qSkill);
	m_getBattleTimer.start(2000);	//2秒获取一次战斗信息

}

void MLAssist::doUpdateBattleInfo()
{
	int nEast = YunLai::ReadMemoryIntFromProcessID(m_gameProcessID, "0092BD28");
	int nSouth = YunLai::ReadMemoryIntFromProcessID(m_gameProcessID, "0092BD30");
	setItemText(3, 0, QString("东%1 南%2").arg(nEast).arg(nSouth));
//	qDebug() << "更新战斗";
	memset(m_Infos, 0, sizeof(PERSONINFO) * 20);
	readBattleInfo();
	refreshBattleUI();
	return;
	POINT pi;
	::GetCursorPos(&pi);	
	DWORD srcColorVal = GetPixel(m_screenHDC, pi.x, pi.y);
	qDebug() <<"屏幕坐标 颜色"<< pi.x << pi.y << srcColorVal;
	::ScreenToClient(m_gameHwnd, &pi);
	DWORD colorVal = GetPixel(m_gameHDC, pi.x, pi.y);
	qDebug() << "游戏坐标 颜色" << pi.x << pi.y << colorVal << GetRValue(colorVal) << GetGValue(colorVal) << GetBValue(colorVal);
	/*DWORD capColor = YunLai::GetScreenColorCapture(pi.x,pi.y,m_gameHwnd);
	qDebug() << "截图取色 颜色" << pi.x << pi.y << capColor;*/

//	qDebug() << pi.x << pi.y << m_lastPoint.x << m_lastPoint.y << srcColorVal << colorVal << (m_lastPoint.x-pi.x) << (m_lastPoint.y-pi.y);
	m_lastPoint = pi;
}

void MLAssist::on_checkBox_autoBattle_clicked()
{
	m_autoBattle = ui.checkBox_autoBattle->isChecked();
	if (m_autoBattle)
	{
		emit signal_autoBattle();
		//MoveToGamePoint(226, 97,10,100);
	}
}
//'走到游戏相应坐标（游戏里面的绝对坐标）
void MLAssist::MoveToGamePoint(int gameX, int gameY, long stampTime/*=0*/, long nTryTime/*=0*/)
{	
	//当前坐标
	long dx, dy,resX,resY;
	POINT points;
	for (int i = 0; i < nTryTime; ++i)
	{
		int nCurX = YunLai::ReadMemoryIntFromProcessID(m_gameProcessID, "0092BD28");
		int nCurY = YunLai::ReadMemoryIntFromProcessID(m_gameProcessID, "0092BD30");
		dx = gameX - nCurX;	//东西
		dy = gameY - nCurY;	//南北 16像素即下一个坐标点
		int nMoveDirection = 0;
		if (dx > 0)//上走
		{
			if (dy == 0)//右上
				nMoveDirection = MOVE_DIRECTION_RIGHTUP;
			else if (dy > 0)
				nMoveDirection = MOVE_DIRECTION_RIGHT;
			else if (dy < 0)
				nMoveDirection = MOVE_DIRECTION_UP;
		}
		else if (dx == 0)
		{
			if (dy == 0)//原地不动
				return;
			else if (dy > 0)
				nMoveDirection = MOVE_DIRECTION_RIGHTDOWN;
			else if (dy < 0)
				nMoveDirection = MOVE_DIRECTION_LEFTUP;
		}
		else if (dx < 0)
		{
			if (dy == 0)//左下
				nMoveDirection = MOVE_DIRECTION_LEFTDOWN;
			else if (dy > 0)
				nMoveDirection = MOVE_DIRECTION_DOWN;
			else if (dy < 0)
				nMoveDirection = MOVE_DIRECTION_LEFT;
		}
		if (abs(dx) > 4)//最大8格// 先走一边  走到目标地后 再判断另一个
		{
			if (dx < 0)
				dx = -4;
			else
				dx = 4;
		}
		if (abs(dy) > 4)
		{
			if (dy < 0)
				dy = -4;
			else
				dy = 4;
		}
		switch (nMoveDirection)
		{
		case MOVE_DIRECTION_UP:				
		case MOVE_DIRECTION_DOWN:
		{
			resX = GAME_CENTER_X ;// 320 + 32 * dx + 33 * dy;		
			resY = GAME_CENTER_Y - dx * MOVE_STEP_UPDOWN;// 260 + 24 * (dy - dx);
			break;
		}
		case MOVE_DIRECTION_LEFT:
		case MOVE_DIRECTION_RIGHT:
		{
			resX = GAME_CENTER_X + dx * MOVE_STEP;// 320 + 32 * dx + 33 * dy;		
			resY = GAME_CENTER_Y;// 260 + 24 * (dy - dx);
			break;
		}
		case MOVE_DIRECTION_LEFTUP:
		case MOVE_DIRECTION_RIGHTDOWN:
		{
			resX = GAME_CENTER_X + dy * STEP_VAL;// 320 + 32 * dx + 33 * dy;		
			resY = GAME_CENTER_Y + dy * STEP_VAL;// 260 + 24 * (dy - dx);
			break;
		}
		case MOVE_DIRECTION_RIGHTUP:
		case MOVE_DIRECTION_LEFTDOWN:
		{
			resX = GAME_CENTER_X + dx * STEP_VAL;// 320 + 32 * dx + 33 * dy;		
			resY = GAME_CENTER_Y - dx * STEP_VAL;// 260 + 24 * (dy - dx);
			break;
		}		
		default:return;
		}
		//if (abs(dx) > 5)//最大8格// 先走一边  走到目标地后 再判断另一个
		//{
		//	if (dx < 0)
		//	{
		//		dx = -5;
		//		resX = GAME_CENTER_X + dx * STEP_VAL;// 320 + 32 * dx + 33 * dy;		
		//		resY = GAME_CENTER_Y + abs(dx * STEP_VAL);// 260 + 24 * (dy - dx);
		//	}
		//	else
		//	{
		//		dx = 5;
		//		resX = GAME_CENTER_X + dx * STEP_VAL;// 320 + 32 * dx + 33 * dy;		
		//		resY = GAME_CENTER_Y - dx * STEP_VAL;// 260 + 24 * (dy - dx);
		//	}
		//}
		//else
		//{
		//	if (dx < 0)
		//	{
		//		resX = GAME_CENTER_X + dx * STEP_VAL;// 320 + 32 * dx + 33 * dy;		
		//		resY = GAME_CENTER_Y + abs(dx * STEP_VAL);// 260 + 24 * (dy - dx);
		//	}
		//	else
		//	{
		//		resX = GAME_CENTER_X + dx * STEP_VAL;// 320 + 32 * dx + 33 * dy;		
		//		resY = GAME_CENTER_Y - dx * STEP_VAL;// 260 + 24 * (dy - dx);
		//	}
		//}
		
		//if (dx == 0)//东坐标对了后 计算另一个
		//{
		//	if (abs(dy) > 5)
		//	{
		//		if (dy < 0)
		//			dy = -5;
		//		else
		//			dy = 5;
		//	}
		//	resX = GAME_CENTER_X + dy * STEP_VAL;// 320 + 32 * dx + 33 * dy;		
		//	resY = GAME_CENTER_Y + dy * STEP_VAL;// 260 + 24 * (dy - dx);
		//}
		

		//resX = GAME_CENTER_X + dy * STEP_VAL;// 320 + 32 * dx + 33 * dy;		
		//resY = GAME_CENTER_Y + dy * STEP_VAL;// 260 + 24 * (dy - dx);
		points.x = resX;
		points.y = resY;
		//ClientToScreen(m_gameHwnd,&points);
		//YunLai::SetFocusToWnd(m_gameHwnd);//已经是焦点了 不需要
		LPARAM newl = MAKELPARAM(points.x, points.y);
		LPARAM dstGamePoint = MAKELPARAM(dx, dy);
		SendMessage(m_gameHwnd, WM_MOUSEMOVE, WM_MOUSEMOVE, newl);
		SendMessage(m_gameHwnd, WM_LBUTTONDOWN, WM_LBUTTONDOWN, newl);
		SendMessage(m_gameHwnd, WM_LBUTTONUP, WM_LBUTTONUP, newl);
//		YunLai::MouseClickedEvent(gameX, gameY);
		qDebug()<< nCurX<<nCurY <<gameX << gameY  <<dx<<dy<< resX << resY <<points.x << points.y ;
		//if (GetGameStatus()==1)//战斗中
		//{
		//	return;
		//}
		if (gameX == nCurX && gameY == nCurY)
			return;
		int uTime = (abs(dx) + abs(dy)) * 215;
		qDebug() << uTime;
		Sleep(1000);
//		Sleep((abs(gameX) + abs(gameY)) * 215);
	}		
//	Sleep(stampTime);
}

//'走到游戏相应坐标（游戏里面的绝对坐标）
void MLAssist::MoveToGamePoint2(int gameX, int gameY, long stampTime/*=0*/, long nTryTime/*=0*/)
{
	//当前坐标
	long dx, dy, resX, resY;
	POINT points;
	for (int i = 0; i < nTryTime; ++i)
	{
		int nCurX = YunLai::ReadMemoryIntFromProcessID(m_gameProcessID, "0092BD28");
		int nCurY = YunLai::ReadMemoryIntFromProcessID(m_gameProcessID, "0092BD30");
		dx = gameX - nCurX;	//东西
		dy = gameY - nCurY;	//南北 16像素即下一个坐标点
		int nMoveDirection = 0;
		
		if (abs(dx) > MAX_STEP)//最大8格// 先走一边  走到目标地后 再判断另一个
		{
			if (dx < 0)
			{
				dx = -MAX_STEP;
				resX = GAME_CENTER_X + dx * STEP_VAL;// 320 + 32 * dx + 33 * dy;		
				resY = GAME_CENTER_Y + abs(dx * STEP_VAL);// 260 + 24 * (dy - dx);
				dy = 0;
			}
			else
			{
				dx = MAX_STEP;
				resX = GAME_CENTER_X + dx * STEP_VAL;// 320 + 32 * dx + 33 * dy;		
				resY = GAME_CENTER_Y - dx * STEP_VAL;// 260 + 24 * (dy - dx);
				dy = 0;
			}
		}
		else
		{
			if (dx < 0)
			{
				resX = GAME_CENTER_X + dx * STEP_VAL;// 320 + 32 * dx + 33 * dy;		
				resY = GAME_CENTER_Y + abs(dx * STEP_VAL);// 260 + 24 * (dy - dx);
				dy = 0;
			}
			else
			{
				resX = GAME_CENTER_X + dx * STEP_VAL;// 320 + 32 * dx + 33 * dy;		
				resY = GAME_CENTER_Y - dx * STEP_VAL;// 260 + 24 * (dy - dx);
				dy = 0;
			}
		}

		if (dx == 0)//东坐标对了后 计算另一个
		{
			if (abs(dy) > MAX_STEP)
			{
				if (dy < 0)
					dy = -MAX_STEP;
				else
					dy = MAX_STEP;
			}
			resX = GAME_CENTER_X + dy * STEP_VAL;// 320 + 32 * dx + 33 * dy;		
			resY = GAME_CENTER_Y + dy * STEP_VAL;// 260 + 24 * (dy - dx);
		}

		points.x = resX;
		points.y = resY;
		LPARAM newl = MAKELPARAM(points.x, points.y);
		LPARAM dstGamePoint = MAKELPARAM(dx, dy);
		SendMessage(m_gameHwnd, WM_MOUSEMOVE, WM_MOUSEMOVE, newl);
		SendMessage(m_gameHwnd, WM_LBUTTONDOWN, WM_LBUTTONDOWN, newl);
		SendMessage(m_gameHwnd, WM_LBUTTONUP, WM_LBUTTONUP, newl);
		qDebug() << nCurX << nCurY << gameX << gameY << dx << dy << resX << resY << points.x << points.y;
		//if (GetGameStatus()==1)//战斗中
		//{
		//	return;
		//}
		if (gameX == nCurX && gameY == nCurY)
			return;
		int uTime = (abs(dx) + abs(dy)) * 215;
		qDebug() << uTime;
		Sleep((abs(gameX) + abs(gameY)) * 215);
	}
	//	Sleep(stampTime);
}

void MLAssist::faceToDirection(int direction)
{
	POINT points;
	long resX = 320;
	long resY = 240;// -MOVE_STEP_DIAGONAL;
	switch (direction)
	{
	case MOVE_DIRECTION_UP:resY = 240 - MOVE_STEP_DIAGONAL; break;
	case MOVE_DIRECTION_DOWN:resY = 240 + MOVE_STEP_DIAGONAL; break;
	case MOVE_DIRECTION_LEFT:resX = 320 - MOVE_STEP_DIAGONAL; break;
	case MOVE_DIRECTION_RIGHT:resX = 320 + MOVE_STEP_DIAGONAL; break;
	case MOVE_DIRECTION_LEFTUP:
	{
		resX = 320 - MOVE_STEP_DIAGONAL;
		resY = 240 - MOVE_STEP_DIAGONAL; break;
	}
	case MOVE_DIRECTION_RIGHTUP:
	{
		resX = 320 + MOVE_STEP_DIAGONAL;
		resY = 240 - MOVE_STEP_DIAGONAL; break;
	}
	case MOVE_DIRECTION_LEFTDOWN:
	{
		resX = 320 - MOVE_STEP_DIAGONAL;
		resY = 240 + MOVE_STEP_DIAGONAL; break;
	}
	case MOVE_DIRECTION_RIGHTDOWN: 
	{
		resX = 320 + MOVE_STEP_DIAGONAL;
		resY = 240 + MOVE_STEP_DIAGONAL; break;
	}default:return;
	}
	points.x = resX;
	points.y = resY;
	//ClientToScreen(m_gameHwnd, &points);
		//YunLai::SetFocusToWnd(m_gameHwnd);//已经是焦点了 不需要
	LPARAM newl = MAKELPARAM(points.x, points.y);
	PostMessage(m_gameHwnd, WM_MOUSEMOVE, WM_MOUSEMOVE, newl);
	Sleep(GetDoubleClickTime());
	PostMessage(m_gameHwnd, WM_RBUTTONDOWN, WM_RBUTTONDOWN, newl);
	PostMessage(m_gameHwnd, WM_RBUTTONUP, WM_RBUTTONUP, newl);
	return;
	::ClientToScreen(m_gameHwnd, &points);
	::SetCursorPos(points.x,points.y);
	//YunLai::SetFocusToWnd(m_gameHwnd);//已经是焦点了 不需要
	YunLai::SimMouseClick(2,1);
}

int MLAssist::GetGameStatus()
{
	return YunLai::ReadMemoryIntFromProcessID(m_gameProcessID, "008E5444");
}
//获取人物鼠标位置  参数为敌人所在位置0-19
POINT MLAssist::GetGamePersonPos(int gamePos)
{
	return m_personPos[gamePos];
}

POINT MLAssist::GetGamePersonCoordinate()
{
	POINT pointPerson;
	pointPerson.x = YunLai::ReadMemoryIntFromProcessID(m_gameProcessID, "0092BD28");
	pointPerson.y = YunLai::ReadMemoryIntFromProcessID(m_gameProcessID, "0092BD30");
	return pointPerson;
}

void MLAssist::clickedYes()
{
	POINT yesPoint = pointYes;
	//ClientToScreen(m_gameHwnd, &yesPoint);
	//SetCursorPos(yesPoint.x, yesPoint.y);
	//YunLai::SimMouseClick();
	//return;
//	Sleep(1000);
	//ClientToScreen(m_gameHwnd, &points);
		//YunLai::SetFocusToWnd(m_gameHwnd);//已经是焦点了 不需要
	LPARAM newl = MAKELPARAM(pointYes.x, pointYes.y);
	PostMessage(m_gameHwnd, WM_MOUSEMOVE, WM_MOUSEMOVE, newl);	
	Sleep(GetDoubleClickTime());
	PostMessage(m_gameHwnd, WM_LBUTTONDOWN, WM_LBUTTONDOWN, newl);
	PostMessage(m_gameHwnd, WM_LBUTTONUP, WM_LBUTTONUP, newl);	
}

void MLAssist::clickedNo()
{
	LPARAM newl = MAKELPARAM(pointNo.x, pointNo.y);
	PostMessage(m_gameHwnd, WM_MOUSEMOVE, WM_MOUSEMOVE, newl);
	PostMessage(m_gameHwnd, WM_LBUTTONDOWN, WM_LBUTTONDOWN, newl);
	PostMessage(m_gameHwnd, WM_LBUTTONUP, WM_LBUTTONUP, newl);
}

void MLAssist::clickedSkillBtn()
{
	LPARAM newl = MAKELPARAM(pointSkill.x, pointSkill.y);
	PostMessage(m_gameHwnd, WM_MOUSEMOVE, WM_MOUSEMOVE, newl);
	PostMessage(m_gameHwnd, WM_LBUTTONDOWN, WM_LBUTTONDOWN, newl);
	PostMessage(m_gameHwnd, WM_LBUTTONUP, WM_LBUTTONUP, newl);
}

bool MLAssist::WaitSkillDlg()
{
	//int isBattle = GetGameStatus();
	//COLORREF tempc = GetPixel(m_gameHDC, 311, 214);// 486, 130); //读秒判断
	//if(tempc == /*5939943*/15780518 && isBattle==1)
	//{
	//	return true;
	//}
	int isBattle = GetGameStatus();
	COLORREF tempc = GetPixel(m_gameHDC, 193, 141);// 486, 130);
	if (tempc == /*5939943*/15780518 && isBattle == 1)
	{
		return true;
	}
	return false;
}

bool MLAssist::WaitAndWakeSkillDlg()
{
	int iTryNum = 0;
	while (iTryNum < 5 && WaitSkillDlg() == false)//点击技能 弹出技能对话框
	{
		clickedSkillBtn();
		++iTryNum;
		Sleep(200);
	}
	return WaitSkillDlg();
}

void MLAssist::nowhile(QString szMap, int status/*=1*/)
{
}

bool MLAssist::WaitActDlg()
{
	//int isBattle = GetGameStatus();
	//COLORREF tempc = GetPixel(m_gameHDC,310,212 );// 356, 27);
	//if (tempc == /*9749495*/10930928 && isBattle == 1)
	//{
	//	return true;
	//}
	//return false;
	int isBattle = GetGameStatus();
	COLORREF tempc = GetPixel(m_gameHDC, 585, 8);// 356, 27);
	if (tempc == /*9749495*/15595514 && isBattle == 1)
	{
		return true;
	}
	return false;
}

bool MLAssist::WaitDialog()
{
	int isBattle = GetGameStatus();
	COLORREF tempc = GetPixel(m_gameHDC, 157, 313);// 486, 130);
	//qDebug() << "221,323"<< tempc;
	if (tempc == /*5939943*/65536 && isBattle == 0)
	//COLORREF tempc = GetPixel(m_gameHDC, 465, 12);// 486, 130);
	//qDebug() << "221,323" << tempc;
	//if (tempc == /*5939943*/14671859 && isBattle == 0)
	{
		return true;
	}
	return false;
}
//等待人物宠物操作
bool MLAssist::WaitSelectOpe()
{
	int isBattle = GetGameStatus();
	COLORREF tempc = GetPixel(m_gameHDC, 600, 10);// 486, 130);
	if (tempc == /*5939943*/7243681 && isBattle == 1)
	{
		return true;
	}
	return false;
}
//等待遇敌
bool MLAssist::WaitAutoEny()
{
	int isBattle = GetGameStatus();
	COLORREF tempc = GetPixel(m_gameHDC, 632, 12);// 486, 130);
	if (tempc == /*5939943*/7243681 && isBattle == 0)
	{
		return true;
	}
	return false;
}

void MLAssist::useFirstSkill()
{
	//选取技能
	POINT points;
	points.x = 115;
	points.y = 175;
	//ClientToScreen(m_gameHwnd, &points);
	//SetCursorPos(points.x,points.y);
	//YunLai::SimMouseClick(1,2);
	//return;
		//YunLai::SetFocusToWnd(m_gameHwnd);//已经是焦点了 不需要
	LPARAM newl = MAKELPARAM(points.x, points.y);
	PostMessage(m_gameHwnd, WM_MOUSEMOVE, WM_MOUSEMOVE, newl);
	/*SendMessage(m_gameHwnd, WM_LBUTTONDOWN, WM_LBUTTONDOWN, newl);
	SendMessage(m_gameHwnd, WM_LBUTTONUP, WM_LBUTTONUP, newl);
	SendMessage(m_gameHwnd, WM_LBUTTONDOWN, WM_LBUTTONDOWN, newl);
	SendMessage(m_gameHwnd, WM_LBUTTONUP, WM_LBUTTONUP, newl);*/
	/*YunLai::MouseClickedEvent();
	YunLai::MouseClickedEvent();*/
	Sleep(GetDoubleClickTime());
	YunLai::SetFocusToWnd(m_gameHwnd);
	PostMessage(m_gameHwnd, WM_LBUTTONDOWN, WM_LBUTTONDOWN, newl);
	PostMessage(m_gameHwnd, WM_LBUTTONUP, WM_LBUTTONUP, newl);
	Sleep(1000);
	PostMessage(m_gameHwnd, WM_LBUTTONDOWN, WM_LBUTTONDOWN, newl);
	PostMessage(m_gameHwnd, WM_LBUTTONUP, WM_LBUTTONUP, newl);
	//两次 1
}

void MLAssist::attackAnyEnemy()
{
	POINT enemyPos = GetGamePersonPos(10);
	for (int i = 10; i < 20; i++)
	{
		PERSONINFO info = m_Infos[i];	
		WCHAR buf[MAXBUFLEN] = { 0 };
		if (wcslen(info.name) > 0)//
		{
			enemyPos = GetGamePersonPos(i);
			break;
		}	
	}
	/*ClientToScreen(m_gameHwnd, &enemyPos);
	SetCursorPos(enemyPos.x, enemyPos.y);
	YunLai::SimMouseClick(1,2);
	return;*/

	//选取技能
	//ClientToScreen(m_gameHwnd, &points);
		//YunLai::SetFocusToWnd(m_gameHwnd);//已经是焦点了 不需要
	YunLai::SetFocusToWnd(m_gameHwnd);
	LPARAM newl = MAKELPARAM(enemyPos.x, enemyPos.y);
	PostMessage(m_gameHwnd, WM_MOUSEMOVE, WM_MOUSEMOVE, newl);
	/*YunLai::MouseClickedEvent();
	YunLai::MouseClickedEvent();*/
	Sleep(500);
	PostMessage(m_gameHwnd, WM_LBUTTONDOWN, WM_LBUTTONDOWN, newl);
	PostMessage(m_gameHwnd, WM_LBUTTONUP, WM_LBUTTONUP, newl);
	Sleep(500);
	PostMessage(m_gameHwnd, WM_LBUTTONDOWN, WM_LBUTTONDOWN, newl);
	PostMessage(m_gameHwnd, WM_LBUTTONUP, WM_LBUTTONUP, newl);
	//SendMessage(m_gameHwnd, WM_LBUTTONDOWN, WM_LBUTTONDOWN, newl);
	//SendMessage(m_gameHwnd, WM_LBUTTONUP, WM_LBUTTONUP, newl);
	////宠物的
	//SendMessage(m_gameHwnd, WM_LBUTTONDOWN, WM_LBUTTONDOWN, newl);
	//SendMessage(m_gameHwnd, WM_LBUTTONUP, WM_LBUTTONUP, newl);
}

int MLAssist::getPersionHP()
{
	int nHP = YunLai::ReadMemoryIntFromProcessID(m_gameProcessID, "00E2A418");
	return nHP;
}

int MLAssist::getPersionMP()
{
	int nMP = YunLai::ReadMemoryIntFromProcessID(m_gameProcessID, "00E2A420");
	return nMP;
}

bool MLAssist::WaitToMove(int x, int y)
{
	int nTime = 100;//100秒
	moveToPoint(x, y);
	for (int i=0;i<nTime;++i)
	{
		POINT curPoint = GetGamePersonCoordinate();
		if (curPoint.x == x && curPoint.y == y)
			return true;
		Sleep(1000);
	}
	return false;
}

void MLAssist::AttackThread(MLAssist* pThis)
{
	while (pThis->m_autoBattle)
	{
		//判断是否战斗中 如果不是
		if (pThis->GetGameStatus() == 0)
		{
//			pThis->on_pushButton_up_clicked();
//			pThis->moveToPointEast();
			if (pThis->getPersionHP() < 1000)
			{
				break;
			}
			if (pThis->getPersionMP() < 1000)
			{
				if (pThis->WaitToMove(221, 84) == true)
				{
					pThis->moveToPoint(221, 83);//这个不检测  检测地图变化
					//先不检测  等待切换 然后直接去加血
					Sleep(1000);
					pThis->WaitToMove(12,37);
					pThis->WaitToMove(8, 33);
					pThis->moveToPoint(8, 31);
					Sleep(2000);
					pThis->faceToDirection(0);
				}
				
				break;
			}
			pThis->faceToDirection(MOVE_DIRECTION_DOWN);
			if (pThis->WaitDialog())
			{
				Sleep(2000);
				pThis->clickedYes();
			}
			else
			{
				Sleep(1000);
				continue;//继续 判断
			}
		}
		if (pThis->GetGameStatus() == 0)//战斗状态 是立即变得 应该等待
		{
			Sleep(2000);
			continue;
		}
		//判断是否读秒中
		if (pThis->WaitActDlg())
		{
			Sleep(100);			
			if (pThis->WaitAndWakeSkillDlg() == false)
			{
				Sleep(200);
				continue;
			}
			
			pThis->useFirstSkill();
			Sleep(200);
			pThis->attackAnyEnemy();
		}
		Sleep(2000);
	}
}

void MLAssist::moveToPoint(int x, int y)
{
	YunLai::WriteMemoryIntToProcess(m_gameProcessID, "008E543C", x);
	YunLai::WriteMemoryIntToProcess(m_gameProcessID, "008E5440", y);
	HANDLE hTarget = OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_gameProcessID);
	DWORD tId;
	CreateRemoteThread(hTarget, NULL, 0, (LPTHREAD_START_ROUTINE)(0x0044D000), NULL, 0, &tId);
	return;
	LPVOID CallBase = VirtualAllocEx(hTarget, NULL, 0x1000, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (CallBase == NULL)//如果内存申请失败
	{
		//关闭进程句柄
		CloseHandle(hTarget);
		return;
	}
	bool bRet= WriteProcessMemory(hTarget, CallBase, (LPCVOID)RemoteMoveCall, 0x1000, NULL);
	if (bRet==false)
	{
		//释放内存空间
		VirtualFreeEx(hTarget, CallBase, 0x1000, MEM_DECOMMIT);
		CloseHandle(hTarget);
		return ;
	}
	//动态装载指定的DLL文件
	HANDLE hthread = CreateRemoteThread(hTarget, NULL, 0, LPTHREAD_START_ROUTINE(CallBase), NULL, 0, NULL);
	if (hthread == NULL)//创建加载DLL的线程失败
	{
		//释放内存空间
		VirtualFreeEx(hTarget, CallBase, 0x1000, MEM_DECOMMIT);
		CloseHandle(hTarget);
		return;
	}
	WaitForSingleObject(hthread, INFINITE);
	//释放资源
	VirtualFreeEx(hTarget, CallBase, 0x1000, MEM_DECOMMIT);
	CloseHandle(hthread);
	CloseHandle(hTarget);
}

void MLAssist::moveToPointEast()
{
	int nEast = YunLai::ReadMemoryIntFromProcessID(m_gameProcessID, "0092BD28");
	int nSouth = YunLai::ReadMemoryIntFromProcessID(m_gameProcessID, "0092BD30");
	if(nEast == m_recordPoint.x)
		moveToPoint(nEast+1, nSouth);
	else
		moveToPoint(m_recordPoint.x, nSouth);
}

void MLAssist::AutoEny()
{
	int nEast = YunLai::ReadMemoryIntFromProcessID(m_gameProcessID, "0092BD28");
	int nSouth = YunLai::ReadMemoryIntFromProcessID(m_gameProcessID, "0092BD30");
	if(nEast == m_recordPoint.x || m_lastEnyPoint.y != m_recordPoint.y)
		moveToPoint(m_recordPoint.x, m_recordPoint.y);
}


void MLAssist::RemoteMoveCall()
{
	_asm
	{
		/*push 0x1
		push 0x3
		mov ecx, 0x00ce5758*/
		mov eax, 0x0044D000
		call eax		
	}
}

