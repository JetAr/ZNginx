#pragma once



#include <QtGui/QtGui>
#include <QtCore/QtCore>
#include <QtGui/QMainWindow>

#include <Ogre.h>
//using namespace Ogre;


class OgreWidget;
class SceneViewWidget;
class LayerViewWidget;
class GeneralPropertiesViewWidget;
class CustomPropertiesViewWidget;
class EntityViewWidget;
class TemplateViewWidget;
class ColourPickerWidget;
class ObjectsViewWidget;
class ActionToolbar;
class OgitorPreferencesWidget;
class PreferencesManager;
class LineEditWithHistory;

class TerrainToolsWidget;



struct TabWidgetData
{
	Ogre::String  mCaption;
	void         *mHandle;
};

typedef Ogre::vector<TabWidgetData>::type TabWidgetDataList;



class MainWindow : public QMainWindow
{
 Q_OBJECT
public:
	//MainWindow(void);
	~MainWindow(void);
	MainWindow(QString args = "", QWidget *parent = 0);

	void setApplicationObject(QObject *obj);

	QObject                 *mApplicationObject;

	bool                     mAppActive;

	QTimer                  *mTimer;


	OgreWidget*  mOgreWidget;

	void createSceneRenderWindow();


	QTabWidget*  mEditorTab;

	void createCustomTabWindows();


	void initHiddenRenderWindow();
protected:
	void closeEvent(QCloseEvent *event);

private slots:
		void timerLoop();
		void newFile();
		void open();
		bool save();
		bool saveAs();
		void about();
		void insertCustomer(const QString &customer);
		void addParagraph(const QString &paragraph);

private:
	QString mstrTime;
	void createDockWindows();

private:
	void createActions();
	void createMenus();
	void createToolBars();
	void createStatusBar();

	QMenu *fileMenu;
	QMenu *editMenu;
	QMenu *viewMenu;

	QMenu *helpMenu;
	QToolBar *fileToolBar;
	QToolBar *editToolBar;
	QAction *newAct;
	QAction *openAct;
	QAction *saveAct;
	QAction *saveAsAct;
	QAction *exitAct;
	QAction *cutAct;
	QAction *copyAct;
	QAction *pasteAct;
	QAction *aboutAct;
	QAction *aboutQtAct;


	QListWidget *explorerList;
	QListWidget *groupsList;
	QListWidget *customerList;
	QListWidget *paragraphsList;

	QListWidget *logList;


};

extern MainWindow *mOgitorMainWindow;
