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
#include <QGroupBox>
#include "Defaults.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {
    DebugLogger::initialize("debug.log");
    postProcessFileSelector = new QComboBox(this);
    resetPostProcessFileButton = new QPushButton("✕", this);
    resetPostProcessFileButton->setFixedWidth(24); // gleiche Größe wie bei den Slidern
    resetPostProcessFileButton->setToolTip("Reset Auswahl");
    qDebug() << "postProcessFileSelector:" << postProcessFileSelector;
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
    hflipCheckbox = new QCheckBox("", this);
    vflipCheckbox = new QCheckBox("", this);
    rotationCheckbox = new QCheckBox("", this);
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
    appSelector->addItems({"rpicam-vid", "rpicam-jpeg", "rpicam-still", "rpicam-raw", "rpicam-hello"});
    // Lade benutzerdefinierte Apps aus der Konfiguration
    customAppEntries.clear();
    QSettings settings("/home/admin/rpicam-gui/rpicam-gui.conf", QSettings::IniFormat); // QSettings-Instanz erstellen
    for (int i = 0; i < 5; ++i) {
        QString key = QString("CustomApp%1").arg(i + 1);
        QString value = settings.value(key, "").toString();
        if (!value.isEmpty()) {
            customAppEntries.append(value); // Füge den Namen zur Liste hinzu
    }
}
    updateAppSelector();
    timelapseInput->setPlaceholderText("Interval (ms)");
    timelapseInput->setVisible(true); // Standardmäßig ausgeblendet
    previewSelector->clear();
    previewSelector->addItem("", "");
    previewSelector->addItem("Fullscreen", "--fullscreen");
    previewSelector->addItem("Qt-Preview", "--qt-preview");
    previewSelector->addItem("No Preview", "--nopreview");
    previewSelector->setCurrentIndex(previewSelector->findData("--qt-preview")); // Standardwert setzen
    postProcessFileSelector->setEditable(true); // Benutzerdefinierte Eingaben erlauben
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
    // Erstelle eine QGroupBox für Overlay-Optionen
    QGroupBox *overlayGroup = new QGroupBox("", this);
    overlayGroup->setFixedWidth(330); // Breite der QGroupBox auf 330 Pixel setzen
    QVBoxLayout *overlayLayout = new QVBoxLayout;

    // Füge die Elemente zur QGroupBox hinzu
        auto *boxLayout = new QHBoxLayout;
    boxLayout->addWidget(new QLabel("Overlay:", this));
    boxLayout->addWidget(BoxInput);
    doubleSizeCheckbox = new QCheckBox("x2", this);
    boxLayout->addWidget(doubleSizeCheckbox);
    auto *resetButton = new QPushButton("✕", this);
    resetButton->setFixedWidth(20);
    boxLayout->addWidget(resetButton);
    overlayLayout->addLayout(boxLayout);
    overlayGroup->setLayout(overlayLayout);

    // Füge die QGroupBox zum Hauptlayout hinzu
    mainLayout->addWidget(overlayGroup);
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
    postProcessLayout->addWidget(resetPostProcessFileButton); // Füge den Reset-Button hinzu
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
    //codecSelector->setFixedWidth(150); // Setzt die Breite des Dropdowns auf 150 Pixel

    resetCodecButton = new QPushButton("✕", this);
    resetCodecButton->setFixedWidth(20);
    resetCodecButton->setToolTip("Reset Codec selection");
    
    auto *codecLayout = new QHBoxLayout;
    codecLayout->addWidget(codecLabel);
    codecLayout->addWidget(codecSelector);
    codecLayout->addWidget(resetCodecButton); // Reset-Button hinzufügen
    mainLayout->addLayout(codecLayout);
    
    // Verbindung für den Reset-Button
    connect(resetCodecButton, &QPushButton::clicked, this, [this]() {
        codecSelector->setCurrentIndex(0);
        codecSelector->setCurrentText("h264");
        updateResetButtonColor(resetCodecButton, 0, 0);
    });
    connect(codecSelector, &QComboBox::currentTextChanged, this, [this](const QString &text) {
        bool hasSelection = !text.isEmpty();
        updateResetButtonColor(resetCodecButton, hasSelection ? 1 : 0, 0);
    });

    QPushButton *saveConfigButton = new QPushButton("Save Config", this);
    QPushButton *loadConfigButton = new QPushButton("Load Config", this);
    connect(saveConfigButton, &QPushButton::clicked, this, [this]() {
        QString filePath = QFileDialog::getSaveFileName(
            this,
            "Save Configuration",
            rpicamConfigPath,
            "Config Files (*.txt);;All Files (*.*)"
        );
        if (!filePath.isEmpty()) {
            if (!filePath.endsWith(".txt", Qt::CaseInsensitive)) {
                filePath += ".txt";
            }
            saveConfigurationToFile(filePath);
        }
    });
    connect(loadConfigButton, &QPushButton::clicked, this, [this]() {
        QString filePath = QFileDialog::getOpenFileName(this, "Load Configuration", rpicamConfigPath, "Config Files (*.txt);;All Files (*.*)");
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
        QString initialPath = postProcessFileSelector->currentText();
        if (initialPath.isEmpty() || !QFileInfo(initialPath).isAbsolute()) {
            initialPath = QDir(guiPostProcessFilePath).filePath(initialPath);
        }
        QString fileName = QFileDialog::getOpenFileName(this, "Select Post-Process File", initialPath, "JSON Files (*.json);;All Files (*.*)");
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
    //codecSelector->setFixedWidth(150); // Feste Breite von 150 Pixeln

    resetAwbButton = new QPushButton("✕", this);
    resetAwbButton->setFixedWidth(20);
    resetAwbButton->setToolTip("Reset AWB selection");
    
    auto *awbLayout = new QHBoxLayout;
    awbLayout->addWidget(new QLabel("AWB:", this));
    awbLayout->addWidget(awbSelector);
    awbLayout->addWidget(resetAwbButton); // Reset-Button hinzufügen
    mainLayout->addLayout(awbLayout);
    
    // Verbindung für den Reset-Button
    connect(resetAwbButton, &QPushButton::clicked, this, [this]() {
        awbSelector->setCurrentIndex(0);
        awbSelector->setCurrentText("auto");
        updateResetButtonColor(resetAwbButton, 0, 0);
    });
    connect(awbSelector, &QComboBox::currentTextChanged, this, [this](const QString &text) {
        bool hasSelection = (text != "auto");
        updateResetButtonColor(resetAwbButton, hasSelection ? 1 : 0, 0);
    });    
    sharpnessSlider->setRange(0, 50); // Wertebereich von 0 bis 5 (multipliziert mit 10)
    sharpnessSlider->setSingleStep(1); // Schritte von 0.1 (multipliziert mit 10)
    sharpnessSlider->setValue(0); // Standardwert
    sharpnessInput->setValidator(new QDoubleValidator(0.0, 5.0, 1, this)); // Wertebereich 0.0 bis 5.0, 1 Dezimalstelle
    sharpnessInput->setText("1.0"); // Standardwert
    sharpnessInput->setToolTip("Set the sharpness value (1.0 to 5.0).");
    auto *sharpnessResetButton = new QPushButton("✕", this);
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
    auto *evResetButton = new QPushButton("✕", this);
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
    auto *gainResetButton = new QPushButton("✕", this);
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
    brightnessSlider->setRange(-10, 10); // Wertebereich von -1 bis 1 (multiplziert mit 10)
    brightnessSlider->setSingleStep(1);
    brightnessSlider->setValue(0); // Standardwert
    brightnessInput->setValidator(new QDoubleValidator(-1.0, 1.0, 1, this));
    brightnessInput->setText("0.0");
    auto *brightnessResetButton = new QPushButton("✕", this);
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
    auto *contrastResetButton = new QPushButton("✕", this);
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
    auto *saturationResetButton = new QPushButton("✕", this);
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
    sharpnessSlider->setFixedWidth(270);
    evSlider->setFixedWidth(270);
    gainSlider->setFixedWidth(270);
    brightnessSlider->setFixedWidth(270);
    contrastSlider->setFixedWidth(270);
    saturationSlider->setFixedWidth(270);
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
connect(resetPostProcessFileButton, &QPushButton::clicked, this, [this]() {
    postProcessFileSelector->setCurrentIndex(-1);
    postProcessFileSelector->setCurrentText("");
    updateResetButtonColor(resetPostProcessFileButton, 0, 0); // Farbe zurücksetzen
});
connect(postProcessFileSelector, &QComboBox::currentTextChanged, this, [this](const QString &text) {
    bool hasSelection = !text.isEmpty();
    updateResetButtonColor(resetPostProcessFileButton, hasSelection ? 1 : 0, 0);
});


auto *hflipResetButton = new QPushButton("✕", this);
hflipResetButton->setFixedWidth(20);
hflipResetButton->setToolTip("Reset Horizontal Flip");

auto *vflipResetButton = new QPushButton("✕", this);
vflipResetButton->setFixedWidth(20);
vflipResetButton->setToolTip("Reset Vertical Flip");

auto *rotationResetButton = new QPushButton("✕", this);
rotationResetButton->setFixedWidth(20);
rotationResetButton->setToolTip("Reset Rotation");
/**
mainLayout->addWidget(hflipCheckbox);
mainLayout->addWidget(vflipCheckbox);
mainLayout->addWidget(rotationCheckbox);
 */
auto *hflipLayout = new QHBoxLayout;
hflipLayout->addStretch(); // <-- rechtsbündig
hflipLayout->addWidget(hflipCheckbox);
hflipLayout->addWidget(hflipResetButton);
mainLayout->addLayout(hflipLayout);

auto *vflipLayout = new QHBoxLayout;
vflipLayout->addStretch(); // <-- rechtsbündig
vflipLayout->addWidget(vflipCheckbox);
vflipLayout->addWidget(vflipResetButton);
mainLayout->addLayout(vflipLayout);

auto *rotationLayout = new QHBoxLayout;
rotationLayout->addStretch(); // <-- rechtsbündig
rotationLayout->addWidget(rotationCheckbox);
rotationLayout->addWidget(rotationResetButton);
mainLayout->addLayout(rotationLayout);

// Labels erstellen
auto *hflipLabel = new QLabel("Horizontal Flip:", this);
auto *vflipLabel = new QLabel("Vertical Flip:", this);
auto *rotationLabel = new QLabel("Rotation 180°:", this);

// Layouts anpassen
hflipLayout->addStretch();
hflipLayout->addWidget(hflipLabel);
hflipLayout->addWidget(hflipCheckbox);
hflipLayout->addWidget(hflipResetButton);
mainLayout->addLayout(hflipLayout);

vflipLayout->addStretch();
vflipLayout->addWidget(vflipLabel);
vflipLayout->addWidget(vflipCheckbox);
vflipLayout->addWidget(vflipResetButton);
mainLayout->addLayout(vflipLayout);

rotationLayout->addStretch();
rotationLayout->addWidget(rotationLabel);
rotationLayout->addWidget(rotationCheckbox);
rotationLayout->addWidget(rotationResetButton);
mainLayout->addLayout(rotationLayout);

connect(hflipResetButton, &QPushButton::clicked, this, [this]() {
    hflipCheckbox->setChecked(false);
});
connect(vflipResetButton, &QPushButton::clicked, this, [this]() {
    vflipCheckbox->setChecked(false);
});
connect(rotationResetButton, &QPushButton::clicked, this, [this]() {
    rotationCheckbox->setChecked(false);
});

// Hilfsfunktion für Button-Farbe
auto updateResetButtonColor = [](QPushButton *button, bool active) {
    if (active) {
        button->setStyleSheet("color: red;");
    } else {
        button->setStyleSheet("color: black;");
    }
};

// Direkt nach dem Erstellen der Buttons und Checkboxen:
updateResetButtonColor(hflipResetButton, hflipCheckbox->isChecked());
updateResetButtonColor(vflipResetButton, vflipCheckbox->isChecked());
updateResetButtonColor(rotationResetButton, rotationCheckbox->isChecked());

// Signal-Slots verbinden
connect(hflipCheckbox, &QCheckBox::stateChanged, this, [this, hflipResetButton, updateResetButtonColor](int){
    updateResetButtonColor(hflipResetButton, hflipCheckbox->isChecked());
});
connect(vflipCheckbox, &QCheckBox::stateChanged, this, [this, vflipResetButton, updateResetButtonColor](int){
    updateResetButtonColor(vflipResetButton, vflipCheckbox->isChecked());
});
connect(rotationCheckbox, &QCheckBox::stateChanged, this, [this, rotationResetButton, updateResetButtonColor](int){
    updateResetButtonColor(rotationResetButton, rotationCheckbox->isChecked());
});

}

MainWindow::~MainWindow() {
    DebugLogger::shutdown();
}
void MainWindow::updateParameterFields() {
    parameterWidget->update();
    parameterWidget->repaint();
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
    // Überprüfen, ob die App Codec-Optionen benötigt
    bool showCodec = (selectedApp == "rpicam-vid" || selectedApp == "rpicam-focus" || selectedApp == "rpicam-focus008");

    if (codecLabel && codecSelector && resetCodecButton) { // Überprüfen, ob die Zeiger gültig sind
        codecLabel->setVisible(showCodec);
        codecSelector->setVisible(showCodec);
        resetCodecButton->setVisible(showCodec); // Reset-Button ebenfalls ein-/ausblenden
    }
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
    setupDialog.loadGuiSettings();

    if (setupDialog.exec() == QDialog::Accepted) {
        customAppEntries = setupDialog.getCustomAppEntries(); // Benutzerdefinierte Apps abrufen
        updateAppSelector(); // Dropdown aktualisieren
        loadGuiConfiguration(); // GUI-Konfiguration laden
    }
}

void MainWindow::loadGuiConfiguration() {
    QString defaultConfigFilePath = "/home/admin/rpicam-gui/rpicam-gui.conf";
    QSettings settings(defaultConfigFilePath, QSettings::IniFormat);

    guiOutputFilePath = QDir::cleanPath(settings.value("Paths/GuiOutputPath", "/home/admin/rpicam-gui/output").toString());
    guiPostProcessFilePath = QDir::cleanPath(settings.value("Paths/GuiPostProcessPath", "/home/admin/rpicam-apps/assets").toString());
    rpicamConfigPath = QDir::cleanPath(settings.value("Paths/GuiRpicamConfigPath", "/home/admin/rpicam-gui/config").toString());

    qDebug() << "Loaded rpicamConfigPath:" << rpicamConfigPath;

    updatePostProcessFileDropdown(); // <-- hier Dropdown füllen
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
void MainWindow::updatePostProcessFileDropdown() {
    if (!postProcessFileSelector) return;
    if (guiPostProcessFilePath.isEmpty()) return;
    QDir dir(guiPostProcessFilePath);
    if (!dir.exists()) return;

    // Vorherige Auswahl merken
    QString previousSelection = postProcessFileSelector->currentText();

    postProcessFileSelector->clear();
    QStringList jsonFiles = dir.entryList(QStringList() << "*.json", QDir::Files);
    for (const QString &file : jsonFiles) {
        postProcessFileSelector->addItem(file);
    }

    // Versuche, die vorherige Auswahl wiederherzustellen
    int idx = postProcessFileSelector->findText(previousSelection);
    if (idx >= 0) {
        postProcessFileSelector->setCurrentIndex(idx);
    } else {
        postProcessFileSelector->setCurrentIndex(-1);
        postProcessFileSelector->setCurrentText("");
    }

    qDebug() << "updatePostProcessFileDropdown called";
    qDebug() << "guiPostProcessFilePath:" << guiPostProcessFilePath;
}

void MainWindow::updateAppSelector() {
    appSelector->clear();
    appSelector->addItems({"rpicam-vid", "rpicam-jpeg", "rpicam-still", "rpicam-raw", "rpicam-hello"});
    appSelector->addItems(customAppEntries); // Benutzerdefinierte Apps hinzufügen
}
