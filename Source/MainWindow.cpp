#include "MainWindow.h"

// Constructors
// =====================================================================
MainWindow::MainWindow() :	QWidget(NULL)
{
	// Instantiate the window
	this->setLayout(&layout);
	QFont font("Segoe UI");
	this->setFont(font);
	this->setWindowTitle("Video Compression and Stitch");
	font.setStyleHint(QFont::Monospace);
	this->setWindowIcon(QIcon(":/rsrcs/video.png"));

	// Add the UI elements
	QPushButton* selectVideosButton = new QPushButton("Select Videos", this);
	QPushButton* deleteSelectedVideos = new QPushButton("Delete Selected Videos", this);
	QPushButton* moveUpButton = new QPushButton("Move Video Up", this);
	QPushButton* moveDownButton = new QPushButton("Move Video Down", this);
	QPushButton* beginProcessingButton = new QPushButton("Begin Processing", this);
	QWidget* videoPositionWidget = new QWidget(this);
	QHBoxLayout* videoPositionLayout = new QHBoxLayout(this);
	QPushButton* selectOutputDestination = new QPushButton("Select Output Destination", this);
	outputDestinationLabel.setText("No output destination set...");
	QSpacerItem* vSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
	videoPositionWidget->setLayout(videoPositionLayout);
	videoPositionLayout->addWidget(moveUpButton);
	videoPositionLayout->addWidget(moveDownButton);
	progress.setTextVisible(false);
	CRFspinBox.setValue(31);
	CRFspinBox.setPrefix("CONSTANT RATE FACTOR: ");
	CRFspinBox.setRange(0, 51);

	// Set the selection model for the video list
	videosList.setSelectionMode(QAbstractItemView::SelectionMode::MultiSelection);

	// Set the window size
	QSize screenSize = qApp->primaryScreen()->size();
	this->setFixedWidth(screenSize.width() / 3.0);
	this->setFixedHeight(screenSize.height() * 0.9);

	// Add elements to the layout
	layout.addWidget(selectVideosButton);
	layout.addWidget(&videosList);
	layout.addWidget(videoPositionWidget);
	layout.addWidget(deleteSelectedVideos);
	layout.addWidget(selectOutputDestination);
	layout.addWidget(&outputDestinationLabel);
	layout.addWidget(&CRFspinBox);
	layout.addWidget(beginProcessingButton);
	//layout.addItem(vSpacer);
	layout.addWidget(&progress);

	// Set the thread timer duration 
	threadCheck.setInterval(1000);

	// Connect signals and slots
	connect(selectVideosButton, &QPushButton::clicked, this, &MainWindow::SelectVideos);
	connect(deleteSelectedVideos, &QPushButton::clicked, this, &MainWindow::DeleteSelectedVideos);
	connect(moveUpButton, &QPushButton::clicked, this, &MainWindow::MoveSelectedVideoUp);
	connect(moveDownButton, &QPushButton::clicked, this, &MainWindow::MoveSelectedVideoDown);
	connect(beginProcessingButton, &QPushButton::clicked, this, &MainWindow::BeginProcessing);
	connect(selectOutputDestination, &QPushButton::clicked, this, &MainWindow::ChangeOutputDestination);
	connect(&threadCheck, &QTimer::timeout, this, &MainWindow::joinThread);
}

// Destructors
// =====================================================================
MainWindow::~MainWindow()
{
}

// Event Handlers
// =====================================================================
void MainWindow::closeEvent(QCloseEvent* event)
{
	
}

// Private Slots
// =====================================================================
void MainWindow::SelectVideos()
{
	// Get the previous count of video files
	int previousVideoFileCount = videosList.count();

	// Get a list of video files
	QStringList files = QFileDialog::getOpenFileNames(this, "Select Video Files", QString(), tr("Video Files (*.mp4 *.avi *.mpg *.MOV)"));

	// Ensure that the videos haven't already been added
	QStringList existingVideos;
	for (int i = 0; i < videosList.count(); i++)
	{
		existingVideos.append(videosList.item(i)->data(Qt::DisplayRole).toString());
	}

	for (int i = 0; i < existingVideos.size(); i++)
	{
		files.removeAll(existingVideos.at(i));
	}

	// Add the video files to the list
	for (int i = 0; i < files.size(); i++)
	{
		videosList.addItem(files.at(i));
	}

	// If the previous count was 0 and the current count is > 0, set the outputname
	int currentCount = videosList.count();
	if (previousVideoFileCount == 0 && currentCount > 0)
	{
		std::string outputDesintation = videosList.item(0)->data(Qt::DisplayRole).toString().toStdString();
		char lastChar = '0';
		while (lastChar != '.')
		{
			lastChar = outputDesintation.back();
			outputDesintation.pop_back();
		}
		outputDesintation += "_combined.mp4";
		outputDestinationLabel.setText(QString::fromStdString(outputDesintation));
	}
}

void MainWindow::DeleteSelectedVideos()
{
	QList<QListWidgetItem*> selectedItems = videosList.selectedItems();
	for (int i = 0; i < selectedItems.size(); i++)
	{
		int currentRow = videosList.row(selectedItems.at(i));
		QListWidgetItem* takenItem = videosList.takeItem(currentRow);
		delete takenItem; takenItem = NULL;
	}
}

void MainWindow::MoveSelectedVideoUp()
{
	QList<QListWidgetItem*> selectedItems = videosList.selectedItems();
	for (int i = 0; i < selectedItems.size(); i++)
	{
		// Get the current row and decrement if possible
		int currentRow = videosList.row(selectedItems.at(i));
		QListWidgetItem* takenItem = videosList.takeItem(currentRow);
		if (currentRow > 0)
		{ 
			currentRow--;
		}
		
		// Replace the item
		videosList.insertItem(currentRow, takenItem);
	}
}

void MainWindow::MoveSelectedVideoDown()
{
	QList<QListWidgetItem*> selectedItems = videosList.selectedItems();
	for (int i = 0; i < selectedItems.size(); i++)
	{
		// Get the current row and decrement if possible
		int currentRow = videosList.row(selectedItems.at(i));
		int nVideos = videosList.count();
		QListWidgetItem* takenItem = videosList.takeItem(currentRow);
		if (currentRow < nVideos - 1)
		{
			currentRow++;
		}

		// Replace the item
		videosList.insertItem(currentRow, takenItem);
	}
}

void MainWindow::ChangeOutputDestination()
{
	QString saveFile = QFileDialog::getSaveFileName(this, "Save Output Video As", QString(), "*.mp4");
	if (saveFile.count())
	{
		outputDestinationLabel.setText(saveFile);
	}
}

void MainWindow::BeginProcessing()
{
	// Do nothing if no videos exist or if the thread is currently running
	bool active = false;
	threadActiveM.lock_shared();
	active = threadActive;
	threadActiveM.unlock_shared();

	if (!videosList.count() || active)
	{
		return;
	}
	
	// Update the thread running flag
	threadActiveM.lock();
	threadActive = true;
	threadActiveM.unlock();

	// Find the path to the ffmpeg converter
	char buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);
	std::string::size_type pos = std::string(buffer).find_last_of("\\/");
	std::string path = std::string(buffer).substr(0, pos);
	std::string workingPath = path;
	path.append("\\ffmpeg.exe");

	// F-Out the list of videos
	QDateTime dateTime = QDateTime::currentDateTime();
	videoListFile = workingPath;
	videoListFile += "\\f_";
	videoListFile += dateTime.toString("yyyyMMddhhmmss").toStdString();
	videoListFile += ".txt";
	ofstream out;
	out.open(videoListFile);
	if (!out.is_open())
	{
		cout << "Output stream not open\n";
	}
	QStringList existingVideos;
	out << "# DELETE AFTER PROCESSING\n";
	for (int i = 0; i < videosList.count(); i++)
	{
		out << "file ";
		existingVideos.append(videosList.item(i)->data(Qt::DisplayRole).toString());
		out << existingVideos.back().toStdString();
		out << "\n";
	}
	out << "# DELETE AFTER PROCESSING\n";
	out.close();

	// Prepare the command line for FFMPEG
	std::string commandArgs = " -f concat -safe 0 -i \"";
	commandArgs.append(videoListFile);
	commandArgs.append("\" -c:v libx265 -crf ");
	commandArgs.append(std::to_string(CRFspinBox.value()));
	commandArgs.append(" -c:a copy \"");
	commandArgs.append(outputDestinationLabel.text().toStdString());
	commandArgs.append("\"");

	// Print the command args to the console
	cout << "BEGIN COMMAND ARGS\n";
	cout << commandArgs << "\n";
	cout << "END COMMAND ARGS\n";

	// set the size of the structures
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	// Create the FFMPEG process
	CreateProcess(path.c_str(),   // the path
		const_cast<char *>(commandArgs.c_str()),        // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		0,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&si,            // Pointer to STARTUPINFO structure
		&pi             // Pointer to PROCESS_INFORMATION structure (removed extra parentheses)
	);

	// Start the progress-updater thread
	threadCheck.start();
}

void MainWindow::joinThread()
{
	// Check to see if the processing thread has finished
	DWORD exitCode;
	GetExitCodeProcess(pi.hProcess, &exitCode);
	threadActiveM.lock_shared();
	bool active = threadActive;
	threadActiveM.unlock_shared();

	if (exitCode != STILL_ACTIVE && active)
	{
		// Thread finished
		threadActiveM.lock();
		threadActive = false;
		threadActiveM.unlock();

		// Close the process handles
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);

		// Delete the working text file
		if (!std::remove(videoListFile.c_str()))
		{
			cout << "Successfully cleaned up working file\n";
		}
	}

	// Join the thread if it's inactive
	if (!threadActive)
	{
		if (renderThread.joinable())
		{
			renderThread.join();
		}
		return;
	}
}
