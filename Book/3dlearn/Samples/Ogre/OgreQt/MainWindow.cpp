#include "MainWindow.h"

#include "ogrewidget.hxx"

#include <QtGui/QtGui>
#include <QtCore/QtCore>



MainWindow *mOgitorMainWindow = 0;


//MainWindow::MainWindow(void)
//{
//	resize(800, 600);
//
//}


MainWindow::MainWindow(QString args, QWidget *parent)
: QMainWindow(parent)
, mOgreWidget(0)
//, mLastTime(0), mArgsFile(args), mHasFileArgs(false)
//, mUpdateLastSceneFile1(false), mUpdateLastSceneFile2(false), mPrefManager(0)
//, mSubWindowsVisible(true)
{
	mOgitorMainWindow = this;

	resize(800, 600);

	setWindowTitle(QString("Thanks for Ogitor&ogre provide us with such a perfect code") );

	mEditorTab = new QTabWidget(this);
	mEditorTab->setObjectName("editortab");


	createSceneRenderWindow();

	createCustomTabWindows();

	setCentralWidget(mEditorTab);

	mTimer = new QTimer(this);
	mTimer->setInterval(1000/30);
	connect(mTimer, SIGNAL(timeout()), this, SLOT(timerLoop()));
	mTimer->start();

	//initHiddenRenderWindow();


	createActions();
	createMenus();
	createToolBars();
	createStatusBar();
	createDockWindows();

}


MainWindow::~MainWindow(void)
{
	if (mTimer)
	{
		delete mTimer;
	}
}

void MainWindow::closeEvent(QCloseEvent *event)
//! [3] //! [4]
{
		event->accept();
}
//! [4]

//! [5]
void MainWindow::newFile()
//! [5] //! [6]
{
	QMessageBox::about(this, tr("title"), tr("newFile"));
}
//! [6]

//! [7]
void MainWindow::open()
//! [7] //! [8]
{

	QMessageBox::about(this, tr("title"), tr("open"));

}
//! [8]

//! [9]
bool MainWindow::save()
//! [9] //! [10]
{
	QMessageBox::about(this, tr("title"), tr("save"));

		return false;
}
//! [10]

//! [11]
bool MainWindow::saveAs()
//! [11] //! [12]
{
	QMessageBox::about(this, tr("title"), tr("saveAs"));

	return false;
}
//! [12]

//! [13]
void MainWindow::about()
//! [13] //! [14]
{
	QMessageBox::about(this, tr("About Application"),
		tr("The <b>Application</b> example demonstrates how to "
		"write modern GUI applications using Qt, with a menu bar, "
		"toolbars, and a status bar."));
}

void MainWindow::setApplicationObject(QObject *obj) 
{ 
	mApplicationObject = obj;
	mAppActive = true;
	obj->installEventFilter(this);
}

void MainWindow::timerLoop()
{

	//QDateTime time = QDateTime::currentDateTime();
	////获取系统现在的时间
	//mstrTime = time.toString("yyyy-MM-dd hh:mm:ss dddd");
	////设置系统时间显示格式
	////ui->label->setText(str);
	////在标签上显示时间

	mOgreWidget->update();

	//if(mHasFileArgs)
	//{
	//    if(mOgreWidget->mOgreInitialised)
}

//------------------------------------------------------------------------------
void MainWindow::createCustomTabWindows()
{
	//TabWidgetDataList tabwidgets = OgitorsRoot::getSingletonPtr()->GetTabWidgets();

	//for(unsigned int i = 0;i < tabwidgets.size();i++)
	//{
	//	QWidget *widget = static_cast<QWidget*>(tabwidgets[i].mHandle);
	//	if(widget)
	//	{
	//		try
	//		{
	//			widget->setObjectName(QString(tabwidgets[i].mCaption.c_str()));
	//			mEditorTab->addTab(widget, QString(tabwidgets[i].mCaption.c_str()));
	//		}
	//		catch(...)
	//		{
	//			QMessageBox::warning(QApplication::activeWindow(), "qtOgitor", QString(tr("Invalid TabWidget ID:%1")).arg(QString(tabwidgets[i].mCaption.c_str())), QMessageBox::Ok);
	//		}
	//	}
	//}
}

void MainWindow::createSceneRenderWindow()
{
	QWidget *renderWindowWidget = new QWidget(mEditorTab);

	mOgreWidget = new OgreWidget(renderWindowWidget, true);

	QVBoxLayout *renderWindowLayout = new QVBoxLayout();
	renderWindowLayout->setSpacing(0);
	renderWindowLayout->setMargin(0);
	renderWindowLayout->addWidget(mOgreWidget, 1);

	renderWindowWidget->setLayout(renderWindowLayout);

	mEditorTab->addTab(renderWindowWidget, tr("Render"));
}

void MainWindow::initHiddenRenderWindow()
{
	Ogre::NameValuePairList hiddenParams;

	hiddenParams["externalWindowHandle"] = Ogre::StringConverter::toString((size_t) winId());

	hiddenParams["border"] = "none";
	Ogre::RenderWindow* pPrimary = Ogre::Root::getSingletonPtr()->createRenderWindow("Primary1",1,1,false,&hiddenParams);
	pPrimary->setVisible(false);
	pPrimary->setAutoUpdated(false);

	Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);

	// Load resources

	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}


void MainWindow::createActions()
//! [17] //! [18]
{
	newAct = new QAction(QIcon(":/images/new.png"), tr("&New"), this);
	newAct->setShortcuts(QKeySequence::New);
	newAct->setStatusTip(tr("Create a new file"));
	connect(newAct, SIGNAL(triggered()), this, SLOT(newFile()));

	//! [19]
	openAct = new QAction(QIcon(":/images/open.png"), tr("&Open..."), this);
	openAct->setShortcuts(QKeySequence::Open);
	openAct->setStatusTip(tr("Open an existing file"));
	connect(openAct, SIGNAL(triggered()), this, SLOT(open()));
	//! [18] //! [19]

	saveAct = new QAction(QIcon(":/images/save.png"), tr("&Save"), this);
	saveAct->setShortcuts(QKeySequence::Save);
	saveAct->setStatusTip(tr("Save the document to disk"));
	connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

	saveAsAct = new QAction(tr("Save &As..."), this);
	saveAsAct->setShortcuts(QKeySequence::SaveAs);
	saveAsAct->setStatusTip(tr("Save the document under a new name"));
	connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

	//! [20]
	exitAct = new QAction(tr("E&xit"), this);
	exitAct->setShortcut(tr("Ctrl+Q"));
	//! [20]
	exitAct->setStatusTip(tr("Exit the application"));
	connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

	//! [21]
	cutAct = new QAction(QIcon(":/images/cut.png"), tr("Cu&t"), this);
	//! [21]
	cutAct->setShortcuts(QKeySequence::Cut);
	cutAct->setStatusTip(tr("Cut the current selection's contents to the "
		"clipboard"));
	//connect(cutAct, SIGNAL(triggered()), textEdit, SLOT(cut()));

	copyAct = new QAction(QIcon(":/images/copy.png"), tr("&Copy"), this);
	copyAct->setShortcuts(QKeySequence::Copy);
	copyAct->setStatusTip(tr("Copy the current selection's contents to the "
		"clipboard"));

	pasteAct = new QAction(QIcon(":/images/paste.png"), tr("&Paste"), this);
	pasteAct->setShortcuts(QKeySequence::Paste);
	pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
		"selection"));

	aboutAct = new QAction(tr("&About"), this);
	aboutAct->setStatusTip(tr("Show the application's About box"));
	connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

	//! [22]
	aboutQtAct = new QAction(tr("About &Qt"), this);
	aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
	connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
	//! [22]

	//! [23]
	cutAct->setEnabled(false);
	//! [23] //! [24]
	copyAct->setEnabled(false);
}
//! [24]

//! [25] //! [26]
void MainWindow::createMenus()
//! [25] //! [27]
{
	fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(newAct);
	//! [28]
	fileMenu->addAction(openAct);
	//! [28]
	fileMenu->addAction(saveAct);
	//! [26]
	fileMenu->addAction(saveAsAct);
	fileMenu->addSeparator();
	fileMenu->addAction(exitAct);

	editMenu = menuBar()->addMenu(tr("&Edit"));
	editMenu->addAction(cutAct);
	editMenu->addAction(copyAct);
	editMenu->addAction(pasteAct);

	menuBar()->addSeparator();

	viewMenu = menuBar()->addMenu(tr("&View"));


	helpMenu = menuBar()->addMenu(tr("&Help"));
	helpMenu->addAction(aboutAct);
	helpMenu->addAction(aboutQtAct);
}
//! [27]

//! [29] //! [30]
void MainWindow::createToolBars()
{
	fileToolBar = addToolBar(tr("File"));
	fileToolBar->addAction(newAct);
	//! [29] //! [31]
	fileToolBar->addAction(openAct);
	//! [31]
	fileToolBar->addAction(saveAct);

	editToolBar = addToolBar(tr("Edit"));
	editToolBar->addAction(cutAct);
	editToolBar->addAction(copyAct);
	editToolBar->addAction(pasteAct);
}
//! [30]

//! [32]
void MainWindow::createStatusBar()
//! [32] //! [33]
{
	statusBar()->showMessage(tr("测试"));
}


void MainWindow::createDockWindows()
{
	QDockWidget *dock = new QDockWidget(tr("Explorer"), this);
	dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	explorerList = new QListWidget(dock);
	explorerList->addItems(QStringList()
		<< "001"
		<< "002");
	dock->setWidget(explorerList);
	addDockWidget(Qt::LeftDockWidgetArea, dock);
	viewMenu->addAction(dock->toggleViewAction());


	//groupsList
	dock = new QDockWidget(tr("Groups"), this);
	dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	groupsList = new QListWidget(dock);
	groupsList->addItems(QStringList()
		<< "001"
		<< "002");
	dock->setWidget(groupsList);
	addDockWidget(Qt::RightDockWidgetArea, dock);
	viewMenu->addAction(dock->toggleViewAction());



	//dock = new QDockWidget(tr("Customers"), this);
	//dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	//customerList = new QListWidget(dock);
	//customerList->addItems(QStringList()
	//	<< "John Doe, Harmony Enterprises, 12 Lakeside, Ambleton"
	//	<< "Jane Doe, Memorabilia, 23 Watersedge, Beaton"
	//	<< "Tammy Shea, Tiblanka, 38 Sea Views, Carlton"
	//	<< "Tim Sheen, Caraba Gifts, 48 Ocean Way, Deal"
	//	<< "Sol Harvey, Chicos Coffee, 53 New Springs, Eccleston"
	//	<< "Sally Hobart, Tiroli Tea, 67 Long River, Fedula");
	//dock->setWidget(customerList);
	//addDockWidget(Qt::RightDockWidgetArea, dock);
	//viewMenu->addAction(dock->toggleViewAction());

	//dock = new QDockWidget(tr("Paragraphs"), this);
	//paragraphsList = new QListWidget(dock);
	//paragraphsList->addItems(QStringList()
	//	<< "Thank you for your payment which we have received today."
	//	<< "Your order has been dispatched and should be with you "
	//	"within 28 days."
	//	<< "We have dispatched those items that were in stock. The "
	//	"rest of your order will be dispatched once all the "
	//	"remaining items have arrived at our warehouse. No "
	//	"additional shipping charges will be made."
	//	<< "You made a small overpayment (less than $5) which we "
	//	"will keep on account for you, or return at your request."
	//	<< "You made a small underpayment (less than $1), but we have "
	//	"sent your order anyway. We'll add this underpayment to "
	//	"your next bill."
	//	<< "Unfortunately you did not send enough money. Please remit "
	//	"an additional $. Your order will be dispatched as soon as "
	//	"the complete amount has been received."
	//	<< "You made an overpayment (more than $5). Do you wish to "
	//	"buy more items, or should we return the excess to you?");
	//dock->setWidget(paragraphsList);
	//addDockWidget(Qt::RightDockWidgetArea, dock);
	//viewMenu->addAction(dock->toggleViewAction());



	////logList
	////dock = new QDockWidget(tr("Logs"), this);
	////dock->setAllowedAreas(Qt::BottomDockWidgetArea);
	////groupsList = new QListWidget(dock);
	////groupsList->addItems(QStringList()
	////	<< "001"
	////	<< "002");
	////dock->setWidget(groupsList);
	////addDockWidget(Qt::BottomDockWidgetArea, dock);
	////viewMenu->addAction(dock->toggleViewAction());




	//connect(customerList, SIGNAL(currentTextChanged(const QString &)),
	//	this, SLOT(insertCustomer(const QString &)));
	//connect(paragraphsList, SIGNAL(currentTextChanged(const QString &)),
	//	this, SLOT(addParagraph(const QString &)));
}

void MainWindow::insertCustomer(const QString &customer)
{

	int i =  0;


}

void MainWindow::addParagraph(const QString &paragraph)
{
	int i =  0;

}
