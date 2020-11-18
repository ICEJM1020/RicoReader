#include "RicoReader.h"

RicoReader::RicoReader(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
	drawInterface();

	readJson(NowIndex);
	readImage(NowIndex);

	connect(ui.LastButton, SIGNAL(clicked()), this, SLOT(goLast()));
	connect(ui.NextButton, SIGNAL(clicked()), this, SLOT(goNext()));
	connect(ui.GoButton, SIGNAL(clicked()), this, SLOT(goIndex()));
	connect(ui.RecordButton, SIGNAL(clicked()), this, SLOT(record()));
	connect(ui.ListWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(selectList(QListWidgetItem*)));
}

void RicoReader::goLast()
{
	NowIndex = NowIndex - 1;
	ui.ListWidget->clear();
	rects.clear();
	readJson(NowIndex);
	readImage(NowIndex);
	ui.NoInput->setText(QString::number(NowIndex));
	this->repaint();
}

void RicoReader::goNext()
{
	NowIndex = NowIndex + 1;
	ui.ListWidget->clear();
	rects.clear();
	readJson(NowIndex);
	readImage(NowIndex);
	ui.NoInput->setText(QString::number(NowIndex));
	this->update();
}

void RicoReader::goIndex()
{
	NowIndex = (ui.NoInput->toPlainText()).toInt();
	ui.ListWidget->clear();
	rects.clear();
	readJson(NowIndex);
	readImage(NowIndex);
	ui.NoInput->setText(QString::number(NowIndex));
	this->repaint();
}

void RicoReader::record()
{
	ofstream ofs;
	ofs.open("output.csv", std::ofstream::out | std::ofstream::app);
	std::string output = ui.ListWidget->selectedItems()[0]->text().split(QRegExp("[\\[\\]]"))[1].toStdString();
	ofs << NowIndex << "," << output << endl;
	ofs.close();
}

void RicoReader::selectList(QListWidgetItem* item)
{
	QStringList sections = item->text().split(QRegExp("[\\[\\],]"));
	float boundX = transPoint(sections[1].toInt(), FALSE) + 200;
	float boundY = transPoint(sections[2].toInt(), TRUE) + 30;
	float boundW = transPoint(sections[3].toInt(), FALSE) - transPoint(sections[1].toInt(), FALSE);
	float boundH = transPoint(sections[4].toInt(), TRUE) - transPoint(sections[2].toInt(), TRUE);
	QRect temp(boundX, boundY, boundW, boundH);
	rects.clear();
	rects.push_back(temp);
	this->repaint();
}

void RicoReader::readImage(int index)
{
	jpgAdd = baseLocation + QString::number(index) + ".jpg";
}

void RicoReader::readJson(int index)
{
	jsonAdd = baseLocation + QString::number(index) + ".json";
	ifstream ifs;
	ifs.open(jsonAdd.toStdString());
	//QFile file(jsonAdd);
	Json::Reader reader;
	Json::Value nowJson;

	if (ifs.is_open()) {
		if (!reader.parse(ifs, nowJson, false))
		{
			qDebug() << "reader parse error: " << strerror(errno) << endl;
		}
		else 
		{
			imgW = nowJson["activity"]["root"]["bounds"][2].asInt();
			imgH = nowJson["activity"]["root"]["bounds"][3].asInt();
			findChild(nowJson["activity"]["root"]);
		}
		
	}
	else 
	{
		QMessageBox::information(NULL, "No picture", "Sorry, picture not found", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
		qDebug() << "cannot open the json file" << endl;
	}

}

void RicoReader::findChild(Json::Value root)
{
	if (!root.isMember("children")) {
		std::string bounds = "[" + root["bounds"][0].asString() + "," + root["bounds"][1].asString() + "," + root["bounds"][2].asString() + "," + root["bounds"][3].asString() + "]";
		ui.ListWidget->addItem(new QListWidgetItem(QObject::tr(bounds.data())));

		float boundX = transPoint(root["bounds"][0].asInt(), FALSE) + 200;
		float boundY = transPoint(root["bounds"][1].asInt(), TRUE) + 30;
		float boundW = transPoint(root["bounds"][2].asInt(), FALSE) - transPoint(root["bounds"][0].asInt(), FALSE);
		float boundH = transPoint(root["bounds"][3].asInt(), TRUE) - transPoint(root["bounds"][1].asInt(), TRUE);
		QRect rect(boundX, boundY, boundW, boundH);
		rects.push_back(rect);
	}
	else {
		//qDebug() << root["children"].size() << endl;
		for (int i=0; i < root["children"].size(); i++) {
			findChild(root["children"][i]);
		}
		
	}
	this->repaint();
}

void RicoReader::paintEvent(QPaintEvent *e) {
	QPainter painter(this);

	QImage img = QImage(jpgAdd);
	img = img.scaled(labelW, labelH);
	QRect boundsImg(200, 30, labelW, labelH);
	QRect ImgSize(0,0,img.width() ,img.height());
	painter.drawImage(boundsImg, img, ImgSize);

	QPen pen;
	pen.setWidth(3);
	pen.setColor(Qt::red);
	pen.setStyle(Qt::SolidLine);

	painter.setPen(pen);

	for (int j = 0;j<rects.size();j++) {
		painter.drawRect(rects[j]);
	}

	painter.end();
}

float RicoReader::transPoint(float ori, bool wh) {
	if (!wh) {
		return (ori / imgW)*labelW;
	}
	else {
		return (ori / imgH)*labelH;
	}
}

void RicoReader::drawInterface()
{
	getScreen();
	this->setGeometry(0, 0, screen_Width, screen_Height);
	this->setFixedSize(this->width(), this->height());

	int app_h=this->height(), app_w=this->width();

	//ui.ImageLabel->setGeometry(200, 30, app_w*0.25, ((app_w*0.25)/9)*16);
	
	labelW = app_w * 0.25;
	labelH = ((app_w*0.25) / 9) * 16;

	ui.LastButton->setGeometry(100 + app_w*0.45, 15, app_w*0.1, app_h*0.1);
	ui.NextButton->setGeometry(150 + app_w*0.55, 15, app_w*0.1, app_h*0.1);
	ui.NoInput->setGeometry(200 + app_w * 0.65, 15, app_w*0.1, app_h*0.04);
	ui.GoButton->setGeometry(200 + app_w * 0.65, 30+ app_h * 0.04, app_w*0.1, app_h*0.04);
	ui.RecordButton->setGeometry(app_w*0.9-50, 15, app_w*0.1, app_h*0.1);

	ui.ListWidget->setGeometry(100 + app_w * 0.45, 30 + app_h * 0.1, app_w*0.45, app_h*0.8);
	//ui.JsonScroll->setGeometry(100 + app_w*0.45, 30+ app_h*0.1, app_w*0.45, app_h*0.8);

	ui.NoInput->setText(QString::number(NowIndex));

}

void RicoReader::getScreen()
{
	QDesktopWidget* thisDesktop = QApplication::desktop();
	//获取可用桌面大小  
	QRect deskRect = thisDesktop->screenGeometry(thisDesktop);
	//获取设备屏幕大小  
	QRect screenRect = thisDesktop->screenGeometry();

	int width = GetSystemMetrics(SM_CXFULLSCREEN);
	int height = GetSystemMetrics(SM_CYFULLSCREEN);
	/*screen_Width = screenRect.width();
	screen_Height = screenRect.height();*/
	screen_Width = width;
	screen_Height = height;
}

