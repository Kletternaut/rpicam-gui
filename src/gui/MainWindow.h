#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#define APP_VERSION "0.0.57"

#include <QMainWindow>
#include <QComboBox>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QFormLayout>
#include <QWidget>
#include <QProcess>
#include <QLabel>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMap>
#include <QResizeEvent>
#include <QDebug>
#include "SelectionOverlay.h"
#include "CustomLineEdit.h" // Füge die benutzerdefinierte Klasse hinzu
#include <QShowEvent> // Für QShowEvent
#include <QSettings> // Füge diese Zeile hinzu
#include <QString> // Füge diese Zeile hinzu

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr); // Nur Deklaration
    ~MainWindow();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override; // Deklaration der showEvent-Methode

private slots:
    void startRpiCamApp(); // Nur Deklaration
    void stopRpiCamApp();
    void updateParameterFields();
    void parseListCamerasOutput(const QString &output);
    void showHelp();
    void showAboutDialog();
    void updateBoxInputFromSelection(const QRect &selection);
    void openGuiSetupDialog(); // Neue Methode für GUI-Setup
    void loadGuiConfiguration(); // Neue Methode für GUI-Konfiguration

private:
    void createMenus();
    QComboBox *appSelector;
    QComboBox *cameraSelector;
    QTextEdit *cameraInfo;
    QFormLayout *parameterLayout;
    QWidget *parameterWidget;
    QLineEdit *parameterInput;
    QTextEdit *outputLog;
    QPushButton *startButton;
    QMap<QString, QString> cameraDetails;
    QComboBox *resolutionSelector;
    QComboBox *framerateSelector;
    QComboBox *previewSelector;
    QLineEdit *outputFileName;
    QPushButton *browseButton;
    QLineEdit *timeoutInput;
    QComboBox *timeoutSelector;
    QLineEdit *timelapseInput;
    QCheckBox *segmentationCheckbox;
    QProcess process;
    QPushButton *stopButton;
    QPushButton *startStopButton;
    QComboBox *postProcessFileSelector;
    QPushButton *postProcessFileBrowseButton;
    QLineEdit *customPreviewInput;
    QComboBox *codecSelector;
    QLabel *codecLabel;
    CustomLineEdit *BoxInput; // Ersetze QLineEdit durch CustomLineEdit
    QTextEdit *debugLog;
    QCheckBox *timestampCheckbox;
    QCheckBox *autoNamingCheckbox; // Deklariere die Checkbox hier
    SelectionOverlay *selectionOverlay = nullptr; // Initialisiere als nullptr
    QComboBox *awbSelector; // AWB-Auswahl
    QSlider *sharpnessSlider; // Slider für Sharpness
    QLineEdit *sharpnessInput; // Eingabefeld für Sharpness
    QSlider *evSlider;           // Slider für EV
    QLineEdit *evInput;          // Eingabefeld für EV
    QSlider *gainSlider;         // Slider für Gain
    QLineEdit *gainInput;        // Eingabefeld für Gain
    QSlider *brightnessSlider;   // Slider für Brightness
    QLineEdit *brightnessInput;  // Eingabefeld für Brightness
    QSlider *contrastSlider;     // Slider für Contrast
    QLineEdit *contrastInput;    // Eingabefeld für Contrast
    QSlider *saturationSlider;   // Slider für Saturation
    QLineEdit *saturationInput;  // Eingabefeld für Saturation
    void updateCameraInfo(int index);
    void updateFramerateOptions(const QString &resolution);
    void openSaveFileDialog();
    void updateTimelapseField();
    void updateButtonVisibility();
    void updateCodecVisibility(const QString &selectedApp);
    void saveConfigurationToFile(const QString &filePath);
    void loadConfigurationFromFile(const QString &filePath);
    void setupLayout(); // Deklaration der Methode
    void updateResetButtonColor(QPushButton *button, double currentValue, double defaultValue);
    QString calculateBoxInput(int additionalOffsetY = 0);
    QString guiOutputFilePath; // Eindeutige Benennung für GUI-Output-Pfad
    QString guiPostProcessFilePath; // Eindeutige Benennung für GUI-Post-Process-Pfad
    QString configFilePath; // Variable für den Pfad der Konfigurationsdatei
    void parseConfigurationFile(const QString &filePath); // Deklaration der Methode
};

#endif // MAINWINDOW_H
