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
#include <QThread> // Füge dies am Anfang der Datei hinzu
#include <QDateTime> // Für QDateTime
#include "SelectionOverlay.h"
#include <QtGlobal>
#include "../utils/DebugLogger.h"
#include "GuiSetupDialog.h" // Externe Datei für den Setup-Dialog
#include "../Version.h"
const double DEFAULT_SHARPNESS = 1.0;
const double DEFAULT_EV = 0.0;
const double DEFAULT_GAIN = 0.0;
const double DEFAULT_BRIGHTNESS = 0.0;
const double DEFAULT_CONTRAST = 1.0;
const double DEFAULT_SATURATION = 1.0;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {
    DebugLogger::initialize("debug.log");
    loadGuiConfiguration();
      appSelector = new QComboBox(this);
      cameraSelector = new QComboBox(this);
      resolutionSelector = new QComboBox(this);
      framerateSelector = new QComboBox(this);
      previewSelector = new QComboBox(this);
      cameraInfo = new QTextEdit(this);
      parameterLayout = new QFormLayout;
      parameterWidget = new QWidget(this);
      outputLog = new QTextEdit(this);
      startStopButton = new QPushButton("Start", this);
      outputFileName = new QLineEdit(this);
      browseButton = new QPushButton("Browse", this);
      timeoutSelector = new QComboBox(this);
      timelapseInput = new QLineEdit(this);
      segmentationCheckbox = new QCheckBox("Enable Segmentation (_%04d)", this);
      postProcessFileSelector = new QComboBox(this);
      postProcessFileBrowseButton = new QPushButton("Browse", this);
      customPreviewInput = new QLineEdit(this);
      BoxInput = new CustomLineEdit(this);
      timestampCheckbox = new QCheckBox("TS", this);
      selectionOverlay = new SelectionOverlay(nullptr); // Kein Eltern-Widget
      awbSelector = new QComboBox(this);
      sharpnessSlider = new QSlider(Qt::Horizontal, this);
      sharpnessInput = new QLineEdit(this);
      evSlider = new QSlider(Qt::Horizontal, this);
      evInput = new QLineEdit(this);
      gainSlider = new QSlider(Qt::Horizontal, this);
      gainInput = new QLineEdit(this);
      brightnessSlider = new QSlider(Qt::Horizontal, this);
      brightnessInput = new QLineEdit(this);
      contrastSlider = new QSlider(Qt::Horizontal, this);
      contrastInput = new QLineEdit(this);
      saturationSlider = new QSlider(Qt::Horizontal, this);
      saturationInput = new QLineEdit(this);
    initializeSelectionOverlay();
    initializeBoxInput();
    appSelector->setToolTip("Select the application to run (e.g., rpicam-still or rpicam-vid).");
    cameraSelector->setToolTip("Select the camera to use.");
    resolutionSelector->setToolTip("Choose the resolution for the camera.");
    framerateSelector->setToolTip("Select the desired framerate.");
    previewSelector->setToolTip("Choose a preview mode for the camera.");
    outputFileName->setToolTip("Specify the output file name.");
    browseButton->setToolTip("Browse for a location to save the output file.");
    timestampCheckbox->setToolTip("Enable this option to add a timestamp to the output file.");
    segmentationCheckbox->setToolTip("Enable segmentation to split output files into parts.");
    timelapseInput->setToolTip("Set the interval for timelapse photography.");
    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, size(), QGuiApplication::primaryScreen()->availableGeometry()));
    timeoutSelector->setEditable(true);
    timeoutSelector->addItems({"0", "5000", "10000", "15000", "20000"});
    timeoutSelector->setCurrentText("0");
    appSelector->addItems({"rpicam-vid", "rpicam-jpeg", "rpicam-still", "rpicam-raw", "rpicam-hello", "rpicam-focus", "rpicam-focus008"});
    timelapseInput->setPlaceholderText("Interval (ms)");
    timelapseInput->setVisible(true); // Standardmäßig ausgeblendet
    previewSelector->addItems({"", "--fullscreen", "--qt-preview", "-n" }); // Leere Option hinzufügen
    previewSelector->setCurrentText("--qt-preview"); // Standardwert setzen
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
    auto *mainLayout = new QVBoxLayout;
    auto *inputLayout = new QHBoxLayout;
    inputLayout->addWidget(new QLabel("App:"));
    inputLayout->addWidget(appSelector);
    inputLayout->addWidget(new QLabel("Cam:"));
    inputLayout->addWidget(cameraSelector);
    inputLayout->addWidget(new QLabel("Size:"));
    inputLayout->addWidget(resolutionSelector);
    inputLayout->addWidget(new QLabel("f/s:"));
    inputLayout->addWidget(framerateSelector);
    inputLayout->addWidget(new QLabel("Preview:"));
    inputLayout->addWidget(previewSelector);
    BoxInput->setPlaceholderText("Double-click to toggle overlay");
    BoxInput->setToolTip("Double-click to toggle the overlay visibility.");
    BoxInput->setFixedWidth(150); // Setze die Breite des Overlay-Input-Felds auf 150 Pixel
        auto *boxLayout = new QHBoxLayout;
    boxLayout->addWidget(new QLabel("Overlay:", this));
    boxLayout->addWidget(BoxInput);
    doubleSizeCheckbox = new QCheckBox("x2", this);
    boxLayout->addWidget(doubleSizeCheckbox);
    auto *resetButton = new QPushButton("x", this); // Reset-Button erstellen
    resetButton->setFixedWidth(20); // Breite des Buttons auf 20 Pixel setzen
    boxLayout->addWidget(resetButton); // Reset-Button rechts von BoxInput hinzufügen
    mainLayout->addLayout(boxLayout);
    connect(resetButton, &QPushButton::clicked, this, [this]() {
        QString defaultBoxValue = calculateBoxInput(+30);
        BoxInput->setText(defaultBoxValue);
    });
connect(doubleSizeCheckbox, &QCheckBox::stateChanged, this, [this](int state) {
        Q_UNUSED(state); // Der Zustand wird hier nicht direkt verwendet
        QString updatedBoxValue = calculateBoxInput(+30); // Neuberechnung durchführen
        BoxInput->setText(updatedBoxValue); // Aktualisiere das BoxInput-Feld
        qDebug() << "BoxInput updated after x2 checkbox state change:" << updatedBoxValue;
    });
    auto *outputLayout = new QHBoxLayout;
    outputLayout->addWidget(new QLabel("Output File:"));
    outputLayout->addWidget(outputFileName);
    outputLayout->addWidget(browseButton);
    autoNamingCheckbox = new QCheckBox("AN", this);
    autoNamingCheckbox->setToolTip("enable auto naming");
    outputLayout->addWidget(autoNamingCheckbox);
    outputLayout->addWidget(segmentationCheckbox);
    outputLayout->addWidget(timestampCheckbox);
    auto *timeoutLayout = new QHBoxLayout;
    timeoutLayout->addWidget(new QLabel("Timeout (ms):"));
    timeoutLayout->addWidget(timeoutSelector);
    timeoutSelector->setFixedWidth(170);
    auto *timelapseLayout = new QHBoxLayout;
    timelapseLayout->addWidget(new QLabel("Timelapse:"));
    timelapseLayout->addWidget(timelapseInput);
    timelapseInput->setFixedWidth(170);
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
    codecLabel = new QLabel("Codec:", this);
    codecSelector = new QComboBox(this);
    codecSelector->addItem(""); // Leeres Feld für "null"
    codecSelector->addItems({"h264", "mjpeg", "yuv420", "libav"});
    codecSelector->setCurrentText("h264"); // Standardwert aktiv setzen
    codecSelector->setToolTip("Select the codec to use. Leave empty for default (null).");
    auto *codecLayout = new QHBoxLayout;
    codecLayout->addWidget(codecLabel);
    codecLayout->addWidget(codecSelector);
    mainLayout->addLayout(codecLayout);
    codecLabel->setVisible(true);
    codecSelector->setVisible(true);
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
    auto *centralWidget = new QWidget(this);
    centralWidget->setLayout(mainLayout);
    auto *outerLayout = new QVBoxLayout;
    outerLayout->addWidget(centralWidget); // Füge das zentrale Widget hinzu
    auto *configLayout = new QHBoxLayout;
    configLayout->addWidget(saveConfigButton);
    configLayout->addWidget(loadConfigButton);
    outerLayout->addLayout(configLayout); // Buttons ganz unten hinzufügen
    auto *outerWidget = new QWidget(this);
    outerWidget->setLayout(outerLayout);
    setCentralWidget(outerWidget);
    connect(appSelector, &QComboBox::currentTextChanged, this, &MainWindow::updateCodecVisibility);
    connect(appSelector, &QComboBox::currentTextChanged, this, &MainWindow::updateParameterFields);
    connect(cameraSelector, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::updateCameraInfo);
    connect(resolutionSelector, &QComboBox::currentTextChanged, this, &MainWindow::updateFramerateOptions);
    connect(browseButton, &QPushButton::clicked, this, &MainWindow::openSaveFileDialog);
    connect(startStopButton, &QPushButton::clicked, this, &MainWindow::startRpiCamApp);
    connect(postProcessFileBrowseButton, &QPushButton::clicked, this, [this]() {
    QString fileName = QFileDialog::getOpenFileName(this, "Select Post-Process File", "", "JSON Files (*.json);;All Files (*.*)");
    if (!fileName.isEmpty()) {
        postProcessFileSelector->setCurrentText(fileName); // Setze den ausgewählten Dateinamen
    }
});
    connect(&process, &QProcess::stateChanged, this, &MainWindow::updateButtonVisibility);
    updateButtonVisibility();
    updateParameterFields();
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
    updateCodecVisibility(appSelector->currentText());
    createMenus();
    connect(selectionOverlay, &SelectionOverlay::selectionChanged, this, &MainWindow::updateBoxInputFromSelection);
    if (selectionOverlay) {
        connect(selectionOverlay, &SelectionOverlay::selectionChanged, this, &MainWindow::updateBoxInputFromSelection);
    }
    if (!selectionOverlay) {
        return;
    } else {
    }
    connect(selectionOverlay, &SelectionOverlay::selectionChanged, this, [this](const QRect &selection) {
        updateBoxInputFromSelection(selection);
    });
    awbSelector->addItems({"auto", "incandescent", "tungsten", "fluorescent", "indoor", "daylight", "cloudy", "custom"});
    awbSelector->setCurrentText("auto"); // Standardwert setzen
    awbSelector->setToolTip("Select the AWB (Auto White Balance) mode.");
    auto *awbLayout = new QHBoxLayout;
    awbLayout->addWidget(new QLabel("AWB:", this));
    awbLayout->addWidget(awbSelector);
    mainLayout->addLayout(awbLayout);
    sharpnessSlider->setRange(0, 50); // Wertebereich von 0 bis 5 (multipliziert mit 10)
    sharpnessSlider->setSingleStep(1); // Schritte von 0.1 (multipliziert mit 10)
    sharpnessSlider->setValue(0); // Standardwert
    sharpnessInput->setValidator(new QDoubleValidator(0.0, 5.0, 1, this)); // Wertebereich 0.0 bis 5.0, 1 Dezimalstelle
    sharpnessInput->setText("1.0"); // Standardwert
    sharpnessInput->setToolTip("Set the sharpness value (1.0 to 5.0).");
    auto *sharpnessResetButton = new QPushButton("x", this);
    sharpnessResetButton->setFixedWidth(20); // Breite auf 20 Pixel festlegen
    connect(sharpnessSlider, &QSlider::valueChanged, this, [this, sharpnessResetButton](int value) {
        double sharpness = value / 10.0;
        sharpnessInput->setText(QString::number(sharpness, 'f', 1));
        updateResetButtonColor(sharpnessResetButton, sharpness, DEFAULT_SHARPNESS);
    });
    connect(sharpnessInput, &QLineEdit::textChanged, this, [this, sharpnessResetButton](const QString &text) {
        double sharpness = text.toDouble();
        sharpnessSlider->setValue(static_cast<int>(sharpness * 10));
        updateResetButtonColor(sharpnessResetButton, sharpness, DEFAULT_SHARPNESS);
    });
    connect(sharpnessResetButton, &QPushButton::clicked, this, [this]() {
        sharpnessSlider->setValue(10); // Standardwert (1.0 * 10)
        sharpnessInput->setText("1.0");
    });
    auto *sharpnessLayout = new QHBoxLayout;
    sharpnessLayout->addWidget(new QLabel("Sharpness:", this));
    sharpnessLayout->addWidget(sharpnessSlider);
    sharpnessLayout->addWidget(sharpnessInput);
    sharpnessLayout->addWidget(sharpnessResetButton);
    mainLayout->addLayout(sharpnessLayout);
    evSlider->setRange(-99, 99); // Wertebereich von -5 bis 5 (multipliziert mit 10)
    evSlider->setSingleStep(1);
    evSlider->setValue(0); // Standardwert
    evInput->setValidator(new QDoubleValidator(-9.9, 9.9, 1, this));
    evInput->setText("0.0");
    auto *evResetButton = new QPushButton("x", this);
    evResetButton->setFixedWidth(20);
    connect(evResetButton, &QPushButton::clicked, this, [this]() {
    evSlider->setValue(0); // Standardwert (0.0 * 10)
    evInput->setText("0.0");
});
auto *evLayout = new QHBoxLayout;
evLayout->addWidget(new QLabel("EV:", this));
evLayout->addWidget(evSlider);
evLayout->addWidget(evInput);
evLayout->addWidget(evResetButton);
mainLayout->addLayout(evLayout);
    gainSlider->setRange(-50, 50); // Wertebereich von -5 bis 5 (multipliziert mit 10)
    gainSlider->setSingleStep(1);
    gainSlider->setValue(0); // Standardwert
    gainInput->setValidator(new QDoubleValidator(-5.0, 5.0, 1, this));
    gainInput->setText("0.0");
    auto *gainResetButton = new QPushButton("x", this);
    gainResetButton->setFixedWidth(20);
    connect(gainResetButton, &QPushButton::clicked, this, [this]() {
        gainSlider->setValue(0); // Standardwert (0.0 * 10)
        gainInput->setText("0.0");
    });
    auto *gainLayout = new QHBoxLayout;
    gainLayout->addWidget(new QLabel("Gain:", this));
    gainLayout->addWidget(gainSlider);
    gainLayout->addWidget(gainInput);
    gainLayout->addWidget(gainResetButton);
    mainLayout->addLayout(gainLayout);
    brightnessSlider->setRange(-10, 10); // Wertebereich von -1 bis 1 (multipliziert mit 10)
    brightnessSlider->setSingleStep(1);
    brightnessSlider->setValue(0); // Standardwert
    brightnessInput->setValidator(new QDoubleValidator(-1.0, 1.0, 1, this));
    brightnessInput->setText("0.0");
    auto *brightnessResetButton = new QPushButton("x", this);
    brightnessResetButton->setFixedWidth(20);
    connect(brightnessResetButton, &QPushButton::clicked, this, [this]() {
        brightnessSlider->setValue(0); // Standardwert (0.0 * 10)
        brightnessInput->setText("0.0");
    });
    auto *brightnessLayout = new QHBoxLayout;
    brightnessLayout->addWidget(new QLabel("Brightness:", this));
    brightnessLayout->addWidget(brightnessSlider);
    brightnessLayout->addWidget(brightnessInput);
    brightnessLayout->addWidget(brightnessResetButton);
    mainLayout->addLayout(brightnessLayout);
    contrastSlider->setRange(0, 50); // Wertebereich von 0 bis 5 (multipliziert mit 10)
    contrastSlider->setSingleStep(1); // Schritte von 0.1 (multipliziert mit 10)
    contrastSlider->setValue(10); // Standardwert (1.0 * 10)
    contrastInput->setValidator(new QDoubleValidator(0.0, 5.0, 1, this)); // Wertebereich 0.0 bis 5.0, 1 Dezimalstelle
    contrastInput->setText("1.0");
    auto *contrastResetButton = new QPushButton("x", this);
    contrastResetButton->setFixedWidth(20);
    connect(contrastResetButton, &QPushButton::clicked, this, [this]() {
        contrastSlider->setValue(10); // Standardwert (1.0 * 10)
        contrastInput->setText("1.0");
    });
    auto *contrastLayout = new QHBoxLayout;
    contrastLayout->addWidget(new QLabel("Contrast:", this));
    contrastLayout->addWidget(contrastSlider);
    contrastLayout->addWidget(contrastInput);
    contrastLayout->addWidget(contrastResetButton);
    mainLayout->addLayout(contrastLayout);
    saturationSlider->setRange(0, 10); // Wertebereich von 0 bis 1 (multipliziert mit 10)
    saturationSlider->setSingleStep(1);
    saturationSlider->setValue(10); // Standardwert (1.0)
    saturationInput->setValidator(new QDoubleValidator(0.0, 1.0, 1, this));
    saturationInput->setText("1.0");
    auto *saturationResetButton = new QPushButton("x", this);
    saturationResetButton->setFixedWidth(20);
    connect(saturationResetButton, &QPushButton::clicked, this, [this]() {
        saturationSlider->setValue(10); // Standardwert (1.0 * 10)
        saturationInput->setText("1.0");
    });
    auto *saturationLayout = new QHBoxLayout;
    saturationLayout->addWidget(new QLabel("Saturation:", this));
    saturationLayout->addWidget(saturationSlider);
    saturationLayout->addWidget(saturationInput);
    saturationLayout->addWidget(saturationResetButton);
    mainLayout->addLayout(saturationLayout);
    connect(evSlider, &QSlider::valueChanged, this, [this](int value) {
        double ev = value / 10.0;
        evInput->setText(QString::number(ev, 'f', 1));
    });
    connect(evInput, &QLineEdit::textChanged, this, [this](const QString &text) {
        double ev = text.toDouble();
        evSlider->setValue(static_cast<int>(ev * 10));
    });
    connect(gainSlider, &QSlider::valueChanged, this, [this](int value) {
        double gain = value / 10.0;
        gainInput->setText(QString::number(gain, 'f', 1));
    });
    connect(gainInput, &QLineEdit::textChanged, this, [this](const QString &text) {
        double gain = text.toDouble();
        gainSlider->setValue(static_cast<int>(gain * 10));
    });
    connect(brightnessSlider, &QSlider::valueChanged, this, [this](int value) {
        double brightness = value / 10.0;
        brightnessInput->setText(QString::number(brightness, 'f', 1));
    });
    connect(brightnessInput, &QLineEdit::textChanged, this, [this](const QString &text) {
        double brightness = text.toDouble();
        brightnessSlider->setValue(static_cast<int>(brightness * 10));
    });
    connect(contrastSlider, &QSlider::valueChanged, this, [this](int value) {
        double contrast = value / 10.0; // Konvertiere von Ganzzahl zu Dezimal
        contrastInput->setText(QString::number(contrast, 'f', 1)); // Setze den Wert im Eingabefeld
    });
    connect(contrastInput, &QLineEdit::textChanged, this, [this](const QString &text) {
        double contrast = text.toDouble();
        contrastSlider->setValue(static_cast<int>(contrast * 10)); // Konvertiere von Dezimal zu Ganzzahl
    });
    connect(saturationSlider, &QSlider::valueChanged, this, [this](int value) {
        double saturation = value / 10.0;
        saturationInput->setText(QString::number(saturation, 'f', 1));
    });
    connect(saturationInput, &QLineEdit::textChanged, this, [this](const QString &text) {
        double saturation = text.toDouble();
        saturationSlider->setValue(static_cast<int>(saturation * 10));
    });
    sharpnessInput->setFixedWidth(40);
    evInput->setFixedWidth(40);
    gainInput->setFixedWidth(40);
    brightnessInput->setFixedWidth(40);
    contrastInput->setFixedWidth(40);
    saturationInput->setFixedWidth(40);
    sharpnessSlider->setFixedWidth(300);
    evSlider->setFixedWidth(300);
    gainSlider->setFixedWidth(300);
    brightnessSlider->setFixedWidth(300);
    contrastSlider->setFixedWidth(300);
    saturationSlider->setFixedWidth(300);
    connect(evSlider, &QSlider::valueChanged, this, [this, evResetButton](int value) {
        double ev = value / 10.0;
        evInput->setText(QString::number(ev, 'f', 1));
        updateResetButtonColor(evResetButton, ev, DEFAULT_EV);
    });
    connect(evInput, &QLineEdit::textChanged, this, [this, evResetButton](const QString &text) {
        double ev = text.toDouble();
        evSlider->setValue(static_cast<int>(ev * 10));
        updateResetButtonColor(evResetButton, ev, DEFAULT_EV);
    });
    connect(gainSlider, &QSlider::valueChanged, this, [this, gainResetButton](int value) {
        double gain = value / 10.0;
        gainInput->setText(QString::number(gain, 'f', 1));
        updateResetButtonColor(gainResetButton, gain, DEFAULT_GAIN);
    });
    connect(gainInput, &QLineEdit::textChanged, this, [this, gainResetButton](const QString &text) {
        double gain = text.toDouble();
        gainSlider->setValue(static_cast<int>(gain * 10));
        updateResetButtonColor(gainResetButton, gain, DEFAULT_GAIN);
    });
    connect(brightnessSlider, &QSlider::valueChanged, this, [this, brightnessResetButton](int value) {
        double brightness = value / 10.0;
        brightnessInput->setText(QString::number(brightness, 'f', 1));
        updateResetButtonColor(brightnessResetButton, brightness, DEFAULT_BRIGHTNESS);
    });
    connect(brightnessInput, &QLineEdit::textChanged, this, [this, brightnessResetButton](const QString &text) {
        double brightness = text.toDouble();
        brightnessSlider->setValue(static_cast<int>(brightness * 10));
        updateResetButtonColor(brightnessResetButton, brightness, DEFAULT_BRIGHTNESS);
    });
    connect(contrastSlider, &QSlider::valueChanged, this, [this, contrastResetButton](int value) {
        double contrast = value / 10.0;
        contrastInput->setText(QString::number(contrast, 'f', 1));
        updateResetButtonColor(contrastResetButton, contrast, DEFAULT_CONTRAST);
    });
    connect(contrastInput, &QLineEdit::textChanged, this, [this, contrastResetButton](const QString &text) {
        double contrast = text.toDouble();
        contrastSlider->setValue(static_cast<int>(contrast * 10));
        updateResetButtonColor(contrastResetButton, contrast, DEFAULT_CONTRAST);
    });
    connect(saturationSlider, &QSlider::valueChanged, this, [this, saturationResetButton](int value) {
        double saturation = value / 10.0;
        saturationInput->setText(QString::number(saturation, 'f', 1));
        updateResetButtonColor(saturationResetButton, saturation, DEFAULT_SATURATION);
    });
    connect(saturationInput, &QLineEdit::textChanged, this, [this, saturationResetButton](const QString &text) {
        double saturation = text.toDouble();
        saturationSlider->setValue(static_cast<int>(saturation * 10));
        updateResetButtonColor(saturationResetButton, saturation, DEFAULT_SATURATION);
    });
    connect(selectionOverlay, &SelectionOverlay::overlayClosed, this, [this]() {
    });
    connect(selectionOverlay, &SelectionOverlay::overlayClosed, this, [this]() {
    });
    connect(selectionOverlay, &SelectionOverlay::overlayClosed, this, [this]() {
    QRect selectedArea = selectionOverlay->getSelectedArea();
    if (selectedArea.width() <= 0 || selectedArea.height() <= 0) {
        QMessageBox::warning(this, "Ungültige Auswahl", "Bitte eine gültige Auswahl treffen.");
        return;
    }
});
    connect(autoNamingCheckbox, &QCheckBox::toggled, this, [this](bool checked) {
    if (checked) {
        timestampCheckbox->setChecked(true);
        QString app = appSelector->currentText();
        QString baseName;
        if (app == "rpicam-vid" || app == "rpicam-focus" || app == "rpicam-focus008") {
            baseName = "video";
        } else if (app == "rpicam-jpeg" || app == "rpicam-still") {
            baseName = "image";
        } 
        QString extension = (app == "rpicam-vid" || app == "rpicam-focus" || app == "rpicam-focus008") ? ".mp4" : ".jpg";
        QString fileName = QDir(guiOutputFilePath).filePath(baseName + extension);
        outputFileName->setText(fileName);
    } else {
        timestampCheckbox->setChecked(false); // Deaktiviere die Timestamp-Checkbox
        outputFileName->clear(); // Leere das Output-Feld
    }
});
}

MainWindow::~MainWindow() {
    DebugLogger::shutdown();
}
void MainWindow::updateParameterFields() {
    parameterWidget->update();
    parameterWidget->repaint();
}
void MainWindow::startRpiCamApp() {
    QString app = appSelector->currentText();
    QString camera = cameraSelector->currentText();
    QString resolution = resolutionSelector->currentText();
    QString framerate = framerateSelector->currentText();
    QString preview = previewSelector->currentText();
    QString timeout = timeoutSelector->currentText();
    QString outputFile = outputFileName->text();
    QString path = guiOutputFilePath; // Initialisiere path mit guiOutputFilePath
    QString timelapse = timelapseInput->text();
    bool segmentationEnabled = segmentationCheckbox->isChecked();
    if (timestampCheckbox->isChecked() && !outputFile.isEmpty()) {
        QDateTime now = QDateTime::currentDateTime();
        QString timestamp = now.toString("yyyy-MM-dd_HH-mm-ss");
        QFileInfo fileInfo(outputFile);
        QString baseName = fileInfo.completeBaseName();
        QString suffix = fileInfo.suffix();
        QString directory = fileInfo.path();
        if (directory.isEmpty() || directory == ".") {
            directory = guiOutputFilePath;
        }
        outputFile = QDir(directory).filePath(baseName + "_" + timestamp + "." + suffix);
        outputLog->append("Output file with timestamp: " + outputFile);
    }
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
    if (!outputFile.isEmpty() && !QFileInfo(outputFile).isAbsolute()) {
        outputFile = QDir(path).filePath(outputFile); // Kombiniere mit guiOutputFilePath
    }
    if (!outputFile.isEmpty()) {
        arguments << "-o" << outputFile;
    } else {
    }
    QString postProcessFile = postProcessFileSelector->currentText();
    if (!postProcessFile.isEmpty()) {
        QString postProcessFilePath = "/home/admin/rpicam-apps/assets/" + postProcessFile;
        arguments << "--post-process-file" << postProcessFilePath;
    }
    QString Box = BoxInput->text();
    if (!Box.isEmpty()) {
        arguments << "--preview" << Box;
    }
    if (app == "rpicam-vid") {
        QString codec = codecSelector->currentText();
        if (!codec.isEmpty()) {
            arguments << "--codec" << codec;
        }
    }
    QString awb = awbSelector->currentText();
    if (awb != "auto") { // Nur hinzufügen, wenn der Wert nicht "auto" ist
        arguments << "--awb" << awb;
    }
    double sharpness = sharpnessInput->text().toDouble();
if (sharpness != 1.0) { // Nur hinzufügen, wenn nicht Standardwert
    arguments << "--sharpness" << QString::number(sharpness, 'f', 1);
    }
    double ev = evInput->text().toDouble();
    if (ev != 0.0) { // Nur hinzufügen, wenn nicht Standardwert
    arguments << "--ev" << QString::number(ev, 'f', 1);
    }
    double gain = gainInput->text().toDouble();
    if (gain != 0.0) { // Nur hinzufügen, wenn nicht Standardwert
        arguments << "--gain" << QString::number(gain, 'f', 1);
    }
    double brightness = brightnessInput->text().toDouble();
    if (brightness != 0.0) { // Nur hinzufügen, wenn nicht Standardwert
    arguments << "--brightness" << QString::number(brightness, 'f', 1);
    }
    double contrast = contrastInput->text().toDouble();
    if (contrast != 1.0) { // Nur hinzufügen, wenn nicht Standardwert
        arguments << "--contrast" << QString::number(contrast, 'f', 1);
    }
    double saturation = saturationInput->text().toDouble();
    if (saturation != 1.0) { // Nur hinzufügen, wenn nicht Standardwert
        arguments << "--saturation" << QString::number(saturation, 'f', 1);
    }
    QString fullCommand = app + " " + arguments.join(" ");
    qDebug().noquote() << "Full command:" << fullCommand;
    process.start(app, arguments);
    if (!process.waitForStarted()) {
    } else {
    }
    updateButtonVisibility();
}
void MainWindow::stopRpiCamApp() {
    if (process.state() == QProcess::Running) {
        kill(process.processId(), SIGINT);
        if (!process.waitForFinished(5000)) { // Warte bis zu 5 Sekunden
            process.kill(); // Erzwinge das Beenden, falls der Prozess nicht reagiert
            process.waitForFinished();
        }
        outputLog->append("Process terminated.");
    } else {
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
    QString initialPath = outputFileName->text();
    if (initialPath.isEmpty() || !QFileInfo(initialPath).isAbsolute()) {
        initialPath = QDir(guiOutputFilePath).filePath(initialPath);
    }
    QString fileName = QFileDialog::getSaveFileName(this, "Save File", initialPath, "All Files (*.*);;JPEG Files (*.jpg);;PNG Files (*.png)");
    if (!fileName.isEmpty()) {
        outputFileName->setText(fileName); // Setze den ausgewählten Dateinamen
    } else {
    }
}
void MainWindow::updateButtonVisibility() {
    disconnect(startStopButton, &QPushButton::clicked, nullptr, nullptr);
    if (process.state() == QProcess::NotRunning) {
        startStopButton->setText("Start");
        startStopButton->setStyleSheet("color: black;");
        connect(startStopButton, &QPushButton::clicked, this, &MainWindow::startRpiCamApp);
    } else if (process.state() == QProcess::Running) {
        startStopButton->setText("Stop");
        startStopButton->setStyleSheet("color: red;");
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
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
outputLog->append("Failed to save configuration to " + filePath);
        return;
    }

        QTextStream out(&file);
        QMap<QString, QString> config = {
            {"camera", cameraSelector->currentText()},
        {"preview", previewSelector->currentText()},
        {"timeout", timeoutSelector->currentText()},
        {"width", resolutionSelector->currentText().split("x").value(0)},
        {"height", resolutionSelector->currentText().split("x").value(1)},
        {"framerate", framerateSelector->currentText()},
        {"awb", awbSelector->currentText()},
        {"sharpness", sharpnessInput->text()},
        {"ev", evInput->text()},
        {"gain", gainInput->text()},
        {"brightness", brightnessInput->text()},
        {"contrast", contrastInput->text()},
        {"saturation", saturationInput->text()},
        {"post-process-file", postProcessFileSelector->currentText()},
        {"output", outputFileName->text()},
        {"timelapse", timelapseInput->text()}
    };

    for (auto it = config.begin(); it != config.end(); ++it) {
        if (!it.value().isEmpty()) {
            out << it.key() << "=" << it.value() << "\n";
        }
}

        file.close();
        outputLog->append("Configuration saved to " + filePath);
    }
void MainWindow::loadConfigurationFromFile(const QString &filePath) {
    QFile file(filePath);
    if (!file.exists()) {
        outputLog->append("Configuration file does not exist: " + filePath);
        return;
    }
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        outputLog->append("Failed to open configuration file: " + filePath);
        return;
    }
    QTextStream in(&file);
    QString width, height; // Temporäre Variablen für Breite und Höhe
    QString resolution;
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
        } else if (key == "awb") {
            awbSelector->setCurrentText(value);
        } else if (key == "sharpness") {
            sharpnessInput->setText(value);
            sharpnessSlider->setValue(static_cast<int>(value.toDouble() * 10));
        } else if (key == "ev") {
            evInput->setText(value);
            evSlider->setValue(static_cast<int>(value.toDouble() * 10));
        } else if (key == "gain") {
            gainInput->setText(value);
            gainSlider->setValue(static_cast<int>(value.toDouble() * 10));
        } else if (key == "brightness") {
            brightnessInput->setText(value);
            brightnessSlider->setValue(static_cast<int>(value.toDouble() * 10));
        } else if (key == "contrast") {
            contrastInput->setText(value);
            contrastSlider->setValue(static_cast<int>(value.toDouble() * 10));
        } else if (key == "saturation") {
            saturationInput->setText(value);
            saturationSlider->setValue(static_cast<int>(value.toDouble() * 10));
        } else if (key == "post-process-file") {
            postProcessFileSelector->setCurrentText(value);
        }
        if (key == "width" || key == "height") {
            resolution = QString("%1x%2").arg(width).arg(height);
        }
    }
    if (!resolution.isEmpty()) {
        if (resolutionSelector->findText(resolution) == -1) {
            resolutionSelector->addItem(resolution);
        }
        resolutionSelector->setCurrentText(resolution);
    }
    file.close();
    outputLog->append("Configuration successfully loaded from " + filePath);
    cameraSelector->update();
    previewSelector->update();
    resolutionSelector->update();
    framerateSelector->update();
    postProcessFileSelector->update();
}
void MainWindow::createMenus() {
    QMenu *setupMenu = menuBar()->addMenu(tr("&Setup"));
    QAction *setupAction = setupMenu->addAction(tr("Configure Settings"));
    connect(setupAction, &QAction::triggered, this, &MainWindow::openGuiSetupDialog); // Aktualisierte Methode
    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
        QAction *helpAction = helpMenu->addAction(tr("&Help"));
        connect(helpAction, &QAction::triggered, this, &MainWindow::showHelp);
        QAction *aboutAction = helpMenu->addAction(tr("&About"));
        connect(aboutAction, &QAction::triggered, this, &MainWindow::showAboutDialog);
}
void MainWindow::showAboutDialog() {
    QDialog aboutDialog(this);
    aboutDialog.setWindowTitle(tr("About rpicam-gui"));
    aboutDialog.setFixedSize(400, 473);

    // Hintergrundgrafik
    QLabel *backgroundLabel = new QLabel(&aboutDialog);
        backgroundLabel->setPixmap(QPixmap("/home/admin/rpicam-gui/resources/images/rpicam-gui.png"));
    backgroundLabel->setScaledContents(true);
    backgroundLabel->setGeometry(0, 0, 400, 473);

    // Textinhalt
    QLabel *textLabel = new QLabel(&aboutDialog);
    textLabel->setText(QString(
        "<h1 style='color: white;'>rpicam-gui</h1>"
                "<p style='color: white;'>A graphical user interface</p>"
        "<p style='color: white;'>for controlling rpicam-apps.</p>"
        "<p style='color: white;'>Built with Qt version: %1</p>"
        "<p style='color: white;'>Developed by <b>Kletternaut</b>, 2025</p>"
).arg(QT_VERSION_STR));
    textLabel->setAlignment(Qt::AlignCenter);
    textLabel->setGeometry(50, 10, 300, 200);

    // Versionsnummer unten rechts
    QLabel *versionLabel = new QLabel(&aboutDialog);
    versionLabel->setText(QString("Version: %1").arg(APP_VERSION));
    versionLabel->setStyleSheet("color: white;"); // Weißer Text
    versionLabel->setAlignment(Qt::AlignRight | Qt::AlignBottom);
    // Geometrie mit Abstand vom rechten und unteren Rand
    versionLabel->setGeometry(260, 430, 120, 30); // Abstand von 20px rechts und unten

    aboutDialog.exec();
}
void MainWindow::updateBoxInputFromSelection(const QRect &selection) {
    QString boxText = QString("%1,%2,%3,%4")
                          .arg(selection.x())
                          .arg(selection.y())
                          .arg(selection.width())
                          .arg(selection.height());
    BoxInput->setText(boxText);
}
void MainWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);
    if (selectionOverlay && selectionOverlay->isVisible()) {
        QScreen *screen = QGuiApplication::primaryScreen();
        if (screen) {
            selectionOverlay->setGeometry(screen->geometry()); // Vollbild setzen
        }
    }
}
void MainWindow::setupLayout() {
    QVBoxLayout *mainLayout = new QVBoxLayout;
    auto *appLayout = new QHBoxLayout;
    appLayout->addWidget(new QLabel("App:", this));
    appLayout->addWidget(appSelector);
    mainLayout->addLayout(appLayout);
    auto *cameraLayout = new QHBoxLayout;
    cameraLayout->addWidget(new QLabel("Cam:", this));
    cameraLayout->addWidget(cameraSelector);
    mainLayout->addLayout(cameraLayout);
    auto *resolutionLayout = new QHBoxLayout;
    resolutionLayout->addWidget(new QLabel("Size:", this));
    resolutionLayout->addWidget(resolutionSelector);
    mainLayout->addLayout(resolutionLayout);
    auto *framerateLayout = new QHBoxLayout;
    framerateLayout->addWidget(new QLabel("Framerate:", this));
    framerateLayout->addWidget(framerateSelector);
    mainLayout->addLayout(framerateLayout);
    auto *boxLayout = new QVBoxLayout; // Verwende ein separates vertikales Layout
    auto *boxLabel = new QLabel("Box:", this);
    boxLayout->addWidget(boxLabel);   // Label in einer eigenen Zeile
    boxLayout->addWidget(BoxInput);   // QLineEdit in einer eigenen Zeile
    mainLayout->addLayout(boxLayout); // Füge das vertikale Layout zum Hauptlayout hinzu
    parameterWidget->setLayout(mainLayout);
}
void MainWindow::updateResetButtonColor(QPushButton *button, double currentValue, double defaultValue) {
    if (currentValue != defaultValue) {
        button->setStyleSheet("color: red;"); // Rot, wenn der Wert nicht Standard ist
    } else {
        button->setStyleSheet("color: black;"); // Schwarz, wenn der Wert Standard ist
    }
}
void MainWindow::showEvent(QShowEvent *event) {
    QMainWindow::showEvent(event);
    QString defaultBoxValue = calculateBoxInput();
    BoxInput->setText(defaultBoxValue);
}
QString MainWindow::calculateBoxInput(int additionalOffsetY) {
    int mainWindowWidth = this->geometry().width();
    int mainWindowHeight = this->geometry().height();
    int mainWindowX = this->geometry().x();
    int mainWindowY = this->geometry().y();
    int frameOffsetY = this->frameGeometry().y() - this->geometry().y();
    const int manualOffsetX = -3;  // Korrektur für die X-Koordinate
    const int manualOffsetY = -65; // Korrektur für die Y-Koordinate
    int boxWidth = mainWindowWidth / 2; // Breite des Vorschaufensters (Hälfte der MainWindow-Breite)
    int boxHeight = static_cast<int>(boxWidth * (3.0 / 4.0)); // Höhe des Vorschaufensters (Verhältnis 4:3)
    // Überprüfen, ob die x2-Checkbox aktiviert ist
    if (doubleSizeCheckbox && doubleSizeCheckbox->isChecked()) {
        boxWidth *= 2;  // Breite verdoppeln
        boxHeight *= 2; // Höhe verdoppeln
    }    if (boxWidth % 2 != 0) {
        boxWidth -= 1; // Reduziere um 1, wenn ungerade
    }
    if (boxHeight % 2 != 0) {
        boxHeight -= 1; // Reduziere um 1, wenn ungerade
    }
    int boxX = mainWindowX + manualOffsetX; // X-Koordinate des Vorschaufensters
    int boxY = mainWindowY - boxHeight + frameOffsetY + manualOffsetY + additionalOffsetY; // Y-Koordinate des Vorschaufensters
    return QString("%1,%2,%3,%4")
               .arg(boxX)
               .arg(boxY)
               .arg(boxWidth)
               .arg(boxHeight);
}
void MainWindow::openGuiSetupDialog() {
    GuiSetupDialog setupDialog(this);
    if (setupDialog.exec() == QDialog::Accepted) {
        loadGuiConfiguration(); // Konfiguration neu laden
    }
}
void MainWindow::loadGuiConfiguration() {
    QString defaultConfigFilePath = "/home/admin/rpicam-gui/rpicam-gui.conf";
    QSettings settings(defaultConfigFilePath, QSettings::IniFormat);
    guiOutputFilePath = QDir::cleanPath(settings.value("Paths/GuiOutputPath", "/home/admin/rpicam-gui/output").toString());
    guiPostProcessFilePath = QDir::cleanPath(settings.value("Paths/GuiPostProcessPath", "/home/admin/rpicam-apps/assets").toString());
}
void MainWindow::parseConfigurationFile(const QString &filePath) {
    QFile file(filePath);
    if (!file.exists()) {
        outputLog->append("Configuration file does not exist: " + filePath);
        return;
    }
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        outputLog->append("Failed to open configuration file: " + filePath);
        return;
    }
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.startsWith("#") || line.isEmpty()) {
            continue;
        }
        QStringList parts = line.split("=", Qt::KeepEmptyParts);
        if (parts.size() != 2) {
            continue;
        }
        QString key = parts[0].trimmed();
        QString value = parts[1].trimmed();
        if (key == "ConfigFilePath") {
            configFilePath = value;
        } else if (key == "GuiOutputPath") {
            guiOutputFilePath = QDir::cleanPath(value + "/");
        } else if (key == "GuiPostProcessPath") {
            guiPostProcessFilePath = QDir::cleanPath(value + "/");
        } else {
        }
    }
    file.close();
    outputLog->append("Configuration successfully loaded from " + filePath);
}
void MainWindow::updateSelectionOverlayGeometry() {
    QScreen *screen = QGuiApplication::primaryScreen();
    if (screen) {
        selectionOverlay->setGeometry(screen->geometry());
    }
}
void MainWindow::initializeBoxInput() {
    BoxInput->setPlaceholderText("Double-click to toggle overlay");
    BoxInput->setToolTip("Double-click to toggle the overlay visibility.");
    BoxInput->setFixedWidth(150); // Setze die Breite des Overlay-Input-Felds auf 150 Pixel
    connect(BoxInput, &CustomLineEdit::doubleClicked, this, [this]() {
        updateSelectionOverlayGeometry();
        selectionOverlay->setVisible(!selectionOverlay->isVisible());
    });
}
void MainWindow::initializeSelectionOverlay() {
    selectionOverlay->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool); // Ohne Rahmen
    selectionOverlay->setAttribute(Qt::WA_TranslucentBackground); // Transparenter Hintergrund
    selectionOverlay->setAttribute(Qt::WA_ShowWithoutActivating); // Nicht den Fokus stehlen
    selectionOverlay->setWindowOpacity(0.01);
    connect(selectionOverlay, &SelectionOverlay::selectionChanged, this, &MainWindow::updateBoxInputFromSelection);
    connect(selectionOverlay, &SelectionOverlay::overlayClosed, this, [this]() {
        QRect selectedArea = selectionOverlay->getSelectedArea();
        if (selectedArea.width() <= 0 || selectedArea.height() <= 0) {
            QMessageBox::warning(this, "Ungültige Auswahl", "Bitte eine gültige Auswahl treffen.");
        }
    });
}
void MainWindow::setupLayouts() {
    mainLayout = new QVBoxLayout;
    setupInputLayout();
    setupOutputLayout();
    setupSliderLayout();
    setupAdvancedOptionsLayout();
}

void MainWindow::setupInputLayout() {
    auto *inputLayout = new QHBoxLayout;
    inputLayout->addWidget(new QLabel("App:"));
    inputLayout->addWidget(appSelector);
    inputLayout->addWidget(new QLabel("Cam:"));
    inputLayout->addWidget(cameraSelector);
    inputLayout->addWidget(new QLabel("Size:"));
    inputLayout->addWidget(resolutionSelector);
    inputLayout->addWidget(new QLabel("f/s:"));
    inputLayout->addWidget(framerateSelector);
    mainLayout->addLayout(inputLayout); // Nutze das Klassenmitglied mainLayout
}

void MainWindow::setupOutputLayout() {
    auto *outputLayout = new QHBoxLayout;
    outputLayout->addWidget(new QLabel("Output File:"));
    outputLayout->addWidget(outputFileName);
    outputLayout->addWidget(browseButton);
    mainLayout->addLayout(outputLayout);
}

void MainWindow::setupSliderLayout() {
    auto *sliderLayout = new QVBoxLayout;
    sliderLayout->addWidget(new QLabel("Adjust Parameters:"));
    sliderLayout->addWidget(sharpnessSlider);
    sliderLayout->addWidget(brightnessSlider);
    sliderLayout->addWidget(contrastSlider);
    mainLayout->addLayout(sliderLayout);
}

void MainWindow::setupAdvancedOptionsLayout() {
    auto *advancedLayout = new QVBoxLayout;
    advancedLayout->addWidget(new QLabel("Advanced Options:"));
    advancedLayout->addWidget(segmentationCheckbox);
    advancedLayout->addWidget(timestampCheckbox);
    mainLayout->addLayout(advancedLayout);
}
