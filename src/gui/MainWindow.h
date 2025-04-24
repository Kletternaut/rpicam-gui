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
#include <QResizeEvent>
#include <QDebug>
#include "SelectionOverlay.h"
#include "CustomLineEdit.h"
#include <QShowEvent>
#include <QSettings>
#include <QString>
#include "../Version.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;

private slots:
    void startRpiCamApp();
    void stopRpiCamApp();
    void updateParameterFields();
    void parseListCamerasOutput(const QString &output);
    void showHelp();
    void showAboutDialog();
    void updateBoxInputFromSelection(const QRect &selection);
    void openGuiSetupDialog();
    void loadGuiConfiguration();

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
    CustomLineEdit *BoxInput;
    QTextEdit *debugLog;
    QCheckBox *timestampCheckbox;
    QCheckBox *autoNamingCheckbox;
    SelectionOverlay *selectionOverlay = nullptr;
    QComboBox *awbSelector;
    QSlider *sharpnessSlider;
    QLineEdit *sharpnessInput;
    QSlider *evSlider;
    QLineEdit *evInput;
    QSlider *gainSlider;
    QLineEdit *gainInput;
    QSlider *brightnessSlider;
    QLineEdit *brightnessInput;
    QSlider *contrastSlider;
    QLineEdit *contrastInput;
    QSlider *saturationSlider;
    QLineEdit *saturationInput;
    void updateCameraInfo(int index);
    void updateFramerateOptions(const QString &resolution);
    void openSaveFileDialog();
    void updateTimelapseField();
    void updateButtonVisibility();
    void updateCodecVisibility(const QString &selectedApp);
    void saveConfigurationToFile(const QString &filePath);
    void loadConfigurationFromFile(const QString &filePath);
    void setupLayout();
    void updateResetButtonColor(QPushButton *button, double currentValue, double defaultValue);
    QString calculateBoxInput(int additionalOffsetY = 0);
    QString guiOutputFilePath;
    QString guiPostProcessFilePath;
    QString configFilePath;
    void parseConfigurationFile(const QString &filePath);
    void initializeSelectionOverlay();
    void initializeBoxInput();
    void updateSelectionOverlayGeometry();
    void setupLayouts();
    void setupInputLayout();
    void setupOutputLayout();
    void setupSliderLayout();
    void setupAdvancedOptionsLayout();

    QVBoxLayout *mainLayout;
    QCheckBox *doubleSizeCheckbox; // Checkbox für die Verdopplung der Größe
};
#endif // MAINWINDOW_H
