#include "MainWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QProcess>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QRegularExpression>
#include <QFileDialog>
#include <QCheckBox>
#include <QStyle>
#include <QGuiApplication>
#include <QScreen> // Für QGuiApplication::primaryScreen()
#include <QMenuBar>
#include <QMessageBox>
#include <QDebug>
#include <csignal>  // Für SIGINT
#include <unistd.h> // Für kill()
#include <QMessageBox> // Für QMessageBox

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      appSelector(new QComboBox(this)), // Initialisierung von appSelector
      cameraSelector(new QComboBox(this)),
      resolutionSelector(new QComboBox(this)),
      framerateSelector(new QComboBox(this)),
      previewSelector(new QComboBox(this)),
      cameraInfo(new QTextEdit(this)),
      parameterLayout(new QFormLayout),
      parameterWidget(new QWidget(this)),
      outputLog(new QTextEdit(this)),
      startStopButton(new QPushButton("Start", this)),
      // helpButton(new QPushButton("Help", this)), // Entfernt
      outputFileName(new QLineEdit(this)),
      browseButton(new QPushButton("Browse", this)),
      timeoutSelector(new QComboBox(this)),
      timelapseInput(new QLineEdit(this)),
      segmentationCheckbox(new QCheckBox("Enable Segmentation (_%04d)", this)),
      postProcessFileSelector(new QComboBox(this)),
      postProcessFileBrowseButton(new QPushButton("Browse", this)),
      customPreviewInput(new QLineEdit(this)),
      BoxInput(new QLineEdit(this)) { // Initialisierung von BoxInput

    // Fenster zentrieren
    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, size(), QGuiApplication::primaryScreen()->availableGeometry()));

    // Setze vordefinierte Timeout-Werte
    timeoutSelector->setEditable(true);
    timeoutSelector->addItems({"0", "5000", "10000", "15000", "20000"});
    timeoutSelector->setCurrentText("0");

    // Set up the app selector
    appSelector->addItems({"rpicam-vid", "rpicam-jpeg", "rpicam-still", "rpicam-raw", "rpicam-hello", "rpicam-focus", "rpicam-focus008"});

    // Timelapse-Feld initialisieren
    timelapseInput->setPlaceholderText("Timelapse Interval (ms)");
    timelapseInput->setVisible(false); // Standardmäßig ausgeblendet

    // Set up the preview selector
    previewSelector->addItems({"", "--qt-preview", "-n"}); // Leere Option hinzufügen
    previewSelector->setCurrentText("--qt-preview"); // Standardwert setzen

    // Initialisiere das Dropdown-Menü für Post-Process-Dateien
    postProcessFileSelector->setEditable(true); // Benutzerdefinierte Eingaben erlauben
    postProcessFileSelector->addItems({
        "annotate_cv.json",
        "drc.json",
        "face_detect_cv.json",
        "hailo_classifier.json",
        "hailo_pose_inf_fl.json",
        "hailo_scrfd.json",
        "hailo_yolov5_personface.json",
        "hailo_yolov5_segmentation.json",
        "hailo_yolov6_inference.json",
        "hailo_yolov8_inference.json",
        "hailo_yolov8_pose.json",
        "hailo_yolox_inference.json",
        "hdr.json",
        "imx500_mobilenet_ssd.json",
        "imx500_posenet.json",
        "motion_detect.json",
        "negate.json",
        "object_classify_tf.json",
        "object_detect_tf.json",
        "pose_estimation_tf.json",
        "segmentation_tf.json",
        "sobel_cv.json",
        "yolov5_personface.json",
        "yolov5seg.json"
    });
    postProcessFileSelector->setCurrentText(""); // Standardwert leer setzen

    // Layout setup
    auto *mainLayout = new QVBoxLayout;
    auto *inputLayout = new QHBoxLayout;

    inputLayout->addWidget(new QLabel("App:"));
    inputLayout->addWidget(appSelector);

    inputLayout->addWidget(new QLabel("Camera:"));
    inputLayout->addWidget(cameraSelector);

    inputLayout->addWidget(new QLabel("Resolution:"));
    inputLayout->addWidget(resolutionSelector);

    inputLayout->addWidget(new QLabel("Framerate:"));
    inputLayout->addWidget(framerateSelector);

    inputLayout->addWidget(new QLabel("Preview:"));
    inputLayout->addWidget(previewSelector);

    // Box-Eingabefeld initialisieren
    BoxInput->setPlaceholderText("Box (e.g., 100,200,800,600)");
    BoxInput->setText("100,200,800,600"); // Standardwert
    inputLayout->addWidget(new QLabel("Box:"));
    inputLayout->addWidget(BoxInput);

    // Output-Dateiname und Button
    auto *outputLayout = new QHBoxLayout;
    outputLayout->addWidget(new QLabel("Output File:"));
    outputLayout->addWidget(outputFileName);
    outputLayout->addWidget(browseButton);
    outputLayout->addWidget(segmentationCheckbox); // Checkbox hinzufügen

    // Timeout-Dropdown
    auto *timeoutLayout = new QHBoxLayout;
    timeoutLayout->addWidget(new QLabel("Timeout (ms):"));
    timeoutLayout->addWidget(timeoutSelector);

    // Timelapse-Feld
    auto *timelapseLayout = new QHBoxLayout;
    timelapseLayout->addWidget(new QLabel("Timelapse:"));
    timelapseLayout->addWidget(timelapseInput);

    // Layout für das Post-Process-Dropdown
    auto *postProcessLayout = new QHBoxLayout;
    postProcessLayout->addWidget(new QLabel("Post-Process File:"));
    postProcessLayout->addWidget(postProcessFileSelector);
    postProcessLayout->addWidget(postProcessFileBrowseButton);

    mainLayout->addLayout(inputLayout);
    mainLayout->addLayout(outputLayout);
    mainLayout->addLayout(timeoutLayout);
    mainLayout->addLayout(timelapseLayout); // Timelapse-Feld hinzufügen
    mainLayout->addLayout(postProcessLayout); // Post-Process-Dropdown hinzufügen
    mainLayout->addWidget(cameraInfo);
    mainLayout->addWidget(parameterWidget);
    mainLayout->addWidget(outputLog);
    mainLayout->addWidget(startStopButton);
    mainLayout->addWidget(startStopButton);
    // mainLayout->addWidget(helpButton); // Entfernt

    // Initialisiere das Dropdown-Menü für --codec
    codecLabel = new QLabel("Codec:", this);
    codecSelector = new QComboBox(this);
    codecSelector->addItems({"h264", "mjpeg", "yuv420", "libav"}); // Optionen für --codec
    codecSelector->setCurrentText("h264"); // Standardwert

    auto *codecLayout = new QHBoxLayout;
    codecLayout->addWidget(codecLabel);
    codecLayout->addWidget(codecSelector);

    mainLayout->addLayout(codecLayout);

    // Standardmäßig sichtbar
    codecLabel->setVisible(true);
    codecSelector->setVisible(true);

    // Save and Load Config Buttons
    QPushButton *saveConfigButton = new QPushButton("Save Config", this);
    QPushButton *loadConfigButton = new QPushButton("Load Config", this);

    connect(saveConfigButton, &QPushButton::clicked, this, [this]() {
        QString filePath = QFileDialog::getSaveFileName(this, "Save Configuration", "", "Config Files (*.txt);;All Files (*.*)");
        if (!filePath.isEmpty()) {
            saveConfigurationToFile(filePath);
        }
    });

    connect(loadConfigButton, &QPushButton::clicked, this, [this]() {
        QString filePath = QFileDialog::getOpenFileName(this, "Load Configuration", "", "Config Files (*.txt);;All Files (*.*)");
        if (!filePath.isEmpty()) {
            loadConfigurationFromFile(filePath);
        }
    });

    auto *configLayout = new QHBoxLayout;
    configLayout->addWidget(saveConfigButton);
    configLayout->addWidget(loadConfigButton);
    mainLayout->addLayout(configLayout);

    auto *centralWidget = new QWidget(this);
    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);

    // Connect signals
    connect(appSelector, &QComboBox::currentTextChanged, this, &MainWindow::updateCodecVisibility);
    connect(appSelector, &QComboBox::currentTextChanged, this, &MainWindow::updateParameterFields);
    connect(cameraSelector, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::updateCameraInfo);
    connect(resolutionSelector, &QComboBox::currentTextChanged, this, &MainWindow::updateFramerateOptions);
    connect(browseButton, &QPushButton::clicked, this, &MainWindow::openSaveFileDialog);
    connect(startStopButton, &QPushButton::clicked, this, &MainWindow::startRpiCamApp);
    // connect(helpButton, &QPushButton::clicked, this, &MainWindow::showHelp); // Entfernt
    connect(postProcessFileBrowseButton, &QPushButton::clicked, this, [this]() {
    QString fileName = QFileDialog::getOpenFileName(this, "Select Post-Process File", "", "JSON Files (*.json);;All Files (*.*)");
    if (!fileName.isEmpty()) {
        postProcessFileSelector->setCurrentText(fileName); // Setze den ausgewählten Dateinamen
    }
});
    // Signal verbinden
    connect(&process, &QProcess::stateChanged, this, &MainWindow::updateButtonVisibility);
    
    // Initiale Sichtbarkeit der Buttons festlegen
    updateButtonVisibility();

    // Initialize parameter fields
    updateParameterFields();

    // Parse camera list at startup
    QProcess process;
    process.start("rpicam-hello", QStringList() << "--list-cameras");
    process.waitForFinished();

    QString output = process.readAllStandardOutput();
    QString error = process.readAllStandardError();

    if (!error.isEmpty()) {
        outputLog->append("Error parsing camera list:\n" + error);
    } else {
        parseListCamerasOutput(output);
    }

    qDebug() << "cameraSelector:" << cameraSelector;
    qDebug() << "resolutionSelector:" << resolutionSelector;
    qDebug() << "framerateSelector:" << framerateSelector;

    updateCodecVisibility(appSelector->currentText());
    createMenus();
}

MainWindow::~MainWindow() {}

void MainWindow::updateParameterFields() {
    // Aktualisiere die Parameter-Felder basierend auf der App-Auswahl
}

void MainWindow::startRpiCamApp() {
    QString app = appSelector->currentText();
    QString camera = cameraSelector->currentText();
    QString resolution = resolutionSelector->currentText();
    QString framerate = framerateSelector->currentText();
    QString preview = previewSelector->currentText();
    QString timeout = timeoutSelector->currentText();
    QString outputFile = outputFileName->text();
    QString timelapse = timelapseInput->text();
    bool segmentationEnabled = segmentationCheckbox->isChecked();

    QStringList resolutionParts = resolution.split("x");
    QString width = resolutionParts.value(0);
    QString height = resolutionParts.value(1);

    QStringList arguments;
    arguments << "--camera" << camera
              << "--width" << width
              << "--height" << height
              << "--framerate" << framerate;

    if (!preview.isEmpty()) {
        arguments << preview;
        qDebug() << "Preview Option:" << preview; // Debug-Ausgabe
    }

    if (!timeout.isEmpty()) {
        bool ok;
        int timeoutMs = timeout.toInt(&ok);
        if (ok) {
            arguments << "-t" << QString::number(timeoutMs);
        } else {
            outputLog->append("Invalid timeout value. Using default.");
            arguments << "-t" << "0";
        }
    } else {
        arguments << "-t" << "0";
    }

    if (!timelapse.isEmpty()) {
        bool ok;
        int timelapseMs = timelapse.toInt(&ok);
        if (ok) {
            arguments << "--timelapse" << QString::number(timelapseMs);
        } else {
            outputLog->append("Invalid timelapse value. Skipping.");
        }
    }

    if (segmentationEnabled) {
        arguments << "--segment" << "_%04d";
    }

    if (!outputFile.isEmpty()) {
        arguments << "-o" << outputFile;
    } else {
        outputLog->append("No output file specified. Using default.");
    }

    // Füge die Post-Process-Datei hinzu
    QString postProcessFile = postProcessFileSelector->currentText();
    if (!postProcessFile.isEmpty()) {
        QString postProcessFilePath = "/home/admin/rpicam-apps/assets/" + postProcessFile;
        arguments << "--post-process-file" << postProcessFilePath;
        qDebug() << "Post-Process File Path:" << postProcessFilePath; // Debug-Ausgabe
    }

    // Füge die --preview-Option hinzu
    QString Box = BoxInput->text();
    if (!Box.isEmpty()) {
        arguments << "--preview" << Box;
        qDebug() << "Preview Box:" << Box; // Debug-Ausgabe
    }

    QString fullCommand = app + " " + arguments.join(" ");
    qDebug() << "Full command:" << fullCommand;

    process.start(app, arguments);
    updateButtonVisibility();
}

void MainWindow::stopRpiCamApp() {
    qDebug() << "Stopping process with PID:" << process.processId();

    if (process.state() == QProcess::Running) {
        kill(process.processId(), SIGINT); // Sende SIGINT an den Prozess

        if (!process.waitForFinished(5000)) { // Warte bis zu 5 Sekunden
            qDebug() << "Process did not terminate in time. Forcing kill.";
            process.kill(); // Erzwinge das Beenden, falls der Prozess nicht reagiert
            process.waitForFinished();
        }

        qDebug() << "Process terminated.";
        outputLog->append("Process terminated.");
    } else {
        qDebug() << "No running process to terminate.";
        outputLog->append("No running process to terminate.");
    }

    updateButtonVisibility();
}

void MainWindow::parseListCamerasOutput(const QString &output) {
    cameraSelector->clear();
    resolutionSelector->clear();
    framerateSelector->clear();
    cameraDetails.clear();

    QStringList lines = output.split("\n", Qt::SkipEmptyParts);
    for (const QString &line : lines) {
        QRegularExpression regexCamera(R"(^(\d+)\s*:\s*(.+))");
        QRegularExpression regexMode(R"((\d+x\d+)\s*\[\s*([\d.]+)\s*fps)");

        QRegularExpressionMatch matchCamera = regexCamera.match(line);
        if (matchCamera.hasMatch()) {
            QString cameraIndex = matchCamera.captured(1);
            QString cameraDescription = matchCamera.captured(2);

            cameraSelector->addItem(cameraIndex);
            cameraDetails[cameraIndex] = cameraDescription;
        }

        QRegularExpressionMatch matchMode = regexMode.match(line);
        if (matchMode.hasMatch()) {
            QString resolution = matchMode.captured(1);
            QString framerate = matchMode.captured(2);

            if (!resolutionSelector->findText(resolution)) {
                resolutionSelector->addItem(resolution);
            }
            if (!framerateSelector->findText(framerate)) {
                framerateSelector->addItem(framerate + " fps");
            }
        }
    }

    resolutionSelector->addItem("1332x990");
    resolutionSelector->addItem("2028x1080");
    resolutionSelector->addItem("2028x1520");
    resolutionSelector->addItem("4056x3040");

    if (cameraSelector->count() == 0) {
        outputLog->append("No cameras found.");
    } else {
        outputLog->append("Cameras detected and added to the dropdown.");
    }
}

void MainWindow::updateCameraInfo(int index) {
    if (index >= 0 && index < cameraSelector->count()) {
        QString selectedCamera = cameraSelector->itemText(index);
        QStringList lines = cameraInfo->toPlainText().split("\n", Qt::SkipEmptyParts);

        for (const QString &line : lines) {
            if (line.startsWith("Camera " + selectedCamera + ":")) {
                cameraInfo->setVisible(true); // Info-Feld anzeigen
                cameraInfo->setPlainText(line); // Zeige die Details der ausgewählten Kamera
                return;
            }
        }
    }

    cameraInfo->setVisible(false); // Info-Feld ausblenden, wenn keine Kamera ausgewählt ist
}

void MainWindow::updateFramerateOptions(const QString &resolution) {
    framerateSelector->clear();

    if (resolution == "1332x990") {
        framerateSelector->addItems({"120", "60", "30", "20", "10"});
    } else if (resolution == "2028x1080") {
        framerateSelector->addItems({"50", "40", "30", "20", "10"});
    } else if (resolution == "2028x1520") {
        framerateSelector->addItems({"40", "30", "20", "10"});
    } else if (resolution == "4056x3040") {
        framerateSelector->addItems({"10"});
    }
}

void MainWindow::openSaveFileDialog() {
    QString fileName = QFileDialog::getSaveFileName(this, "Save File", "", "All Files (*.*);;JPEG Files (*.jpg);;PNG Files (*.png)");
    if (!fileName.isEmpty()) {
        outputFileName->setText(fileName); // Setze den ausgewählten Dateinamen
    }
}

void MainWindow::updateButtonVisibility() {
    qDebug() << "Process state:" << process.state();

    // Entferne alte Verbindungen
    disconnect(startStopButton, &QPushButton::clicked, nullptr, nullptr);

    if (process.state() == QProcess::NotRunning) {
        startStopButton->setText("Start");
        connect(startStopButton, &QPushButton::clicked, this, &MainWindow::startRpiCamApp);
    } else if (process.state() == QProcess::Running) {
        startStopButton->setText("Stop");
        connect(startStopButton, &QPushButton::clicked, this, &MainWindow::stopRpiCamApp);
    }
}

void MainWindow::showHelp() {
    QMessageBox::information(this, "Help", 
        "Welcome to the RPiCam GUI!\n\n"
        "1. Select an app from the dropdown.\n"
        "2. Configure the camera, resolution, framerate, and other options.\n"
        "3. Click 'Start' to begin.\n"
        "4. Use 'Stop' to terminate the process.\n\n"
        "For more information, refer to the documentation.");
}

void MainWindow::updateCodecVisibility(const QString &selectedApp) {
    if (selectedApp == "rpicam-vid") {
        if (codecLabel && codecSelector) { // Überprüfen Sie, ob die Zeiger gültig sind
            codecLabel->setVisible(true);
            codecSelector->setVisible(true);
        }
    } else {
        if (codecLabel && codecSelector) { // Überprüfen Sie, ob die Zeiger gültig sind
            codecLabel->setVisible(false);
            codecSelector->setVisible(false);
        }
    }
}

void MainWindow::saveConfigurationToFile(const QString &filePath) {
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);

        out << "camera=" << cameraSelector->currentText() << "\n";
        out << "qt-preview=" << (previewSelector->currentText().isEmpty() ? "" : previewSelector->currentText()) << "\n";
        out << "timeout=" << timeoutSelector->currentText() << "\n";
        out << "preview=" << BoxInput->text() << "\n";
        out << "width=" << resolutionSelector->currentText().split("x").value(0) << "\n";
        out << "height=" << resolutionSelector->currentText().split("x").value(1) << "\n";
        out << "framerate=" << framerateSelector->currentText() << "\n";

        // Speichere den vollständigen Pfad der Post-Process-Datei
        QString postProcessFile = postProcessFileSelector->currentText();
        if (!postProcessFile.isEmpty() && !postProcessFile.startsWith("/")) {
            postProcessFile = "/home/admin/rpicam-apps/assets/" + postProcessFile;
        }
        out << "post-process-file=" << postProcessFile << "\n";

        file.close();
        outputLog->append("Configuration saved to " + filePath);
    } else {
        outputLog->append("Failed to save configuration to " + filePath);
    }
}

void MainWindow::loadConfigurationFromFile(const QString &filePath) {
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);

        QString width, height; // Temporäre Variablen für Breite und Höhe

        while (!in.atEnd()) {
            QString line = in.readLine().trimmed();
            if (line.startsWith("#") || line.isEmpty()) {
                continue; // Kommentare und leere Zeilen überspringen
            }

            QStringList parts = line.split("=", Qt::KeepEmptyParts);
            if (parts.size() != 2) {
                continue; // Ungültige Zeilen überspringen
            }

            QString key = parts[0].trimmed();
            QString value = parts[1].trimmed();

            if (key == "camera") {
                cameraSelector->setCurrentText(value);
            } else if (key == "qt-preview") {
                previewSelector->setCurrentText(value); // Setze das Argument direkt
            } else if (key == "timeout") {
                timeoutSelector->setCurrentText(value);
            } else if (key == "preview") {
                BoxInput->setText(value);
            } else if (key == "width") {
                width = value; // Speichere die Breite
            } else if (key == "height") {
                height = value; // Speichere die Höhe
            } else if (key == "framerate") {
                framerateSelector->setCurrentText(value);
            } else if (key == "post-process-file") {
                postProcessFileSelector->setCurrentText(value);
            }
        }

        // Setze die Auflösung, falls Breite und Höhe vorhanden sind
        if (!width.isEmpty() && !height.isEmpty()) {
            QString resolution = width + "x" + height;
            if (resolutionSelector->findText(resolution) == -1) {
                resolutionSelector->addItem(resolution); // Füge die Auflösung hinzu, falls sie nicht existiert
            }
            resolutionSelector->setCurrentText(resolution); // Setze die aktuelle Auflösung
        }

        file.close();
        outputLog->append("Configuration loaded from " + filePath);

        // Aktualisiere die GUI
        cameraSelector->update();
        previewSelector->update();
        resolutionSelector->update();
        framerateSelector->update();
        postProcessFileSelector->update();
    } else {
        outputLog->append("Failed to load configuration from " + filePath);
    }
}

void MainWindow::createMenus() {
    // Überprüfe, ob das Menü bereits existiert
    QMenu *helpMenu = menuBar()->findChild<QMenu *>("helpMenu");
    if (!helpMenu) {
        helpMenu = new QMenu(tr("&Help"), this);
        helpMenu->setObjectName("helpMenu"); // Eindeutiger Name für das Menü

        QAction *helpAction = helpMenu->addAction(tr("&Help"));
        connect(helpAction, &QAction::triggered, this, &MainWindow::showHelp);

        QAction *aboutAction = helpMenu->addAction(tr("&About"));
        connect(aboutAction, &QAction::triggered, this, &MainWindow::showAboutDialog);

        // Menü rechts in der Menüleiste positionieren
        menuBar()->addMenu(helpMenu);
        menuBar()->setCornerWidget(new QWidget(this), Qt::TopRightCorner);
    }
}

void MainWindow::showAboutDialog() {
    QMessageBox::about(this, tr("About RPiCam-GUI"),
                       tr("RPiCam-GUI\nVersion 1.0\n\n"
                          "A graphical user interface for controlling rpicam-apps."));
}