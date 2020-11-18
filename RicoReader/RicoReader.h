#pragma once
//QT
#include <QtWidgets/QMainWindow>
#include <QDesktopWidget>
#include <qfile.h>
#include <qdebug.h>
#include <qpainter.h>
#include <qmessagebox.h>
#include "ui_RicoReader.h"

//windows
#include <Windows.h>

//cpp
#include <fstream>
//json
#include <json/json.h>
//own


using namespace std;

class RicoReader : public QMainWindow
{
    Q_OBJECT

public:
	//variable
	int NowIndex = 0;
	QString baseLocation = "E:/DataSet/Rico/combined/";
	QString jpgAdd;
	QString jsonAdd;
	std::vector<QRect> rects;
	//function
    RicoReader(QWidget *parent = Q_NULLPTR);
	void drawInterface();
	void readImage(int index);
	void readJson(int index);
public slots:
	void goLast();
	void goNext();
	void goIndex();
	void record();
	void selectList(QListWidgetItem* item);

private:
    //variable
	Ui::RicoReaderClass ui;
	int screen_Width;
	int screen_Height;
	float imgW, imgH;
	float labelW, labelH;

	//function
	void getScreen();
	void findChild(Json::Value root);
	void paintEvent(QPaintEvent * e);
	float transPoint(float ori, bool wh);
};
