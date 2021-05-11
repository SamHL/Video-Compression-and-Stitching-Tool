#pragma once

// Include Directories
// ========================================================
// QT
#include <QtCore>
#include <QtWidgets>
#include <QMainWindow>
#include <QSizePolicy>
#include <qdialog.h>
#include <qtimer.h>
#include <qgridlayout.h>

// STL
#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <shared_mutex>
#include <string>

// WINDOWS
#include <Windows.h>

// Namespaces
// ========================================================
using std::cout;
using std::ofstream;
using std::ifstream;
using std::vector;
using std::shared_mutex;
using std::shared_lock;
using std::get;
using std::tuple;

// Class Definition 
// ========================================================
class MainWindow : public QWidget
{
	Q_OBJECT

public:
	// Constructor
	MainWindow();

	// Destructor
	~MainWindow();

	// Getters

	// Setters

protected:
	// Event Handlers
	void closeEvent(QCloseEvent* event) override;

private:
	// Private UI Variables
	QGridLayout layout;
	QProgressBar progress;
	QListWidget videosList;
	QLabel outputDestinationLabel;
	QSpinBox CRFspinBox;

	// Render Video Out
	double totalFrameCount = 0;  shared_mutex totalFrameCountM;
	double currentFrame = 0; shared_mutex currentFrameM;
	bool threadActive = false; shared_mutex threadActiveM;
	std::thread renderThread;
	void joinThread();
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	QTimer threadCheck;
	std::string videoListFile;

private slots:
	// Private Slots
	void SelectVideos();
	void DeleteSelectedVideos();
	void MoveSelectedVideoUp();
	void MoveSelectedVideoDown();
	void ChangeOutputDestination();
	void BeginProcessing();
};