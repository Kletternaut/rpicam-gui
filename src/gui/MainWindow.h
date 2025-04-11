#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void startRpiCamApp();
    void stopRpiCamApp(); // Methode zum Stoppen des Prozesses
    void updateParameterFields(); // Aktualisiert die Parameter-Felder basierend auf der App-Auswahl
    void parseListCamerasOutput(const QString &output); // Parse camera list output and display in GUI
    void showHelp(); // Methode zum Anzeigen der Hilfe
    void showAboutDialog(); // Methode zum Anzeigen des "About"-Dialogs

private:
    void createMenus(); // Deklaration der Methode hinzufügen
    QComboBox *appSelector; // Dropdown für die Auswahl der App
    QComboBox *cameraSelector; // Dropdown für die Auswahl der Kamera
    QTextEdit *cameraInfo; // Info-Feld für Kamera-Details
    QFormLayout *parameterLayout; // Layout für dynamische Parameter
    QWidget *parameterWidget; // Container für Parameter-Felder
    QLineEdit *parameterInput; // Eingabefeld für Parameter
    QTextEdit *outputLog; // Textfeld für die Ausgabe
    QPushButton *startButton; // Start-Button
    QMap<QString, QString> cameraDetails; // Map für Kamera-Details
    QComboBox *resolutionSelector; // Dropdown für Auflösungen
    QComboBox *framerateSelector; // Dropdown für Framerates
    QComboBox *previewSelector; // Dropdown für Preview-Optionen
    QLineEdit *outputFileName; // Feld für den Output-Dateinamen
    QPushButton *browseButton; // Button für den "Speichern unter"-Dialog
    QLineEdit *timeoutInput; // Eingabefeld für das Timeout
    QComboBox *timeoutSelector; // Dropdown für Timeout-Werte
    // QPushButton *helpButton; // Entfernt
    QLineEdit *timelapseInput; // Eingabefeld für Timelapse-Intervall
    QCheckBox *segmentationCheckbox; // Checkbox für Dateinamensegmentierung
    QProcess process; // Prozess für die gestartete App
    QPushButton *stopButton; // Stop-Button für den Prozess
    QPushButton *startStopButton; // Ein Button für Start und Stop
    QComboBox *postProcessFileSelector; // Dropdown für Post-Process-Dateien
    QPushButton *postProcessFileBrowseButton; // Button für den Dateiauswahldialog
    QLineEdit *customPreviewInput; // Eingabefeld für die --preview-Option
    QComboBox *codecSelector; // Dropdown für --codec
    QLabel *codecLabel; // Label für das Codec-Dropdown
    QLineEdit *BoxInput; // Eingabefeld für die --preview-Option
    QTextEdit *debugLog; // Debug-Log-Feld

    void updateCameraInfo(int index); // Methode zum Aktualisieren der Kamera-Details
    void updateFramerateOptions(const QString &resolution); // Aktualisiert die Framerate-Liste
    void openSaveFileDialog();   // Methode zum Öffnen des "Speichern unter"-Dialogs
    void updateTimelapseField();     // Methode zum Anzeigen des Timelapse-Feldes
    void updateButtonVisibility(); // Aktualisiert die Sichtbarkeit von Start/Stop
    void updateCodecVisibility(const QString &selectedApp); // Methode zur Steuerung der Sichtbarkeit
    void saveConfigurationToFile(const QString &filePath);
    void loadConfigurationFromFile(const QString &filePath);
    void setCameraSelectorText(const QString &value) {
        cameraSelector->setCurrentText(value);
        cameraSelector->update(); // Aktualisiert die Anzeige
    }
};

#endif // MAINWINDOW_H
