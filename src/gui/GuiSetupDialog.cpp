#include "GuiSetupDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QSettings>
#include <QFileDialog>
#include <QDir>
#include <QDebug>
#include <QCoreApplication>

GuiSetupDialog::GuiSetupDialog(QWidget *parent)
    : QDialog(parent), configFilePathEdit(nullptr), outputPathEdit(nullptr), postProcessPathEdit(nullptr), splashScreenCheckbox(new QCheckBox("Show splashscreen on startup", this)) {
    setWindowTitle("Setup");

    // Setze die Größe des Fensters
    if (parent) {
        resize(parent->size());
    } else {
        resize(800, 600);
    }

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Config File Path Group
    QGroupBox *configGroup = new QGroupBox("Configuration File Path", this);
    QVBoxLayout *configLayout = new QVBoxLayout(configGroup);
    configFilePathEdit = new QLineEdit(this); // Initialisiere das Eingabefeld
    QPushButton *browseConfigFileButton = new QPushButton("Browse", this);
    configLayout->addWidget(configFilePathEdit);
    configLayout->addWidget(browseConfigFileButton);
    connect(browseConfigFileButton, &QPushButton::clicked, this, &GuiSetupDialog::browseConfigFilePath);
    mainLayout->addWidget(configGroup);

    // Output Path Group
    QGroupBox *outputGroup = new QGroupBox("Output Path", this);
    QVBoxLayout *outputLayout = new QVBoxLayout(outputGroup);
    outputPathEdit = new QLineEdit(this); // Initialisiere das Eingabefeld
    QPushButton *browseOutputButton = new QPushButton("Browse", this);
    outputLayout->addWidget(outputPathEdit);
    outputLayout->addWidget(browseOutputButton);
    connect(browseOutputButton, &QPushButton::clicked, this, [this]() {
        QString initialPath = outputPathEdit->text();
        if (initialPath.isEmpty()) {
            initialPath = "/home/admin/rpicam-gui/output"; // Standardpfad, falls leer
        }

        QString dir = QFileDialog::getExistingDirectory(this, "Select Output Directory", initialPath);
        if (!dir.isEmpty()) {
            outputPathEdit->setText(dir);
        }
    });
    mainLayout->addWidget(outputGroup);

    // Post-Process Path Group
    QGroupBox *postProcessGroup = new QGroupBox("Post-Process Path", this);
    QVBoxLayout *postProcessLayout = new QVBoxLayout(postProcessGroup);
    postProcessPathEdit = new QLineEdit(this); // Initialisiere das Eingabefeld
    QPushButton *browsePostProcessButton = new QPushButton("Browse", this);
    postProcessLayout->addWidget(postProcessPathEdit);
    postProcessLayout->addWidget(browsePostProcessButton);
    connect(browsePostProcessButton, &QPushButton::clicked, this, [this]() {
        QString initialPath = postProcessPathEdit->text();
        if (initialPath.isEmpty()) {
            initialPath = "/home/admin/rpicam-apps/assets"; // Standardpfad, falls leer
        }

        QString dir = QFileDialog::getExistingDirectory(this, "Select Post-Process Directory", initialPath);
        if (!dir.isEmpty()) {
            postProcessPathEdit->setText(dir);
        }
    });
    mainLayout->addWidget(postProcessGroup);

    // Splash Screen Checkbox
    splashScreenCheckbox->setToolTip("Activate or deactivate the splashscreen on startup.");
    mainLayout->addWidget(splashScreenCheckbox);

    // Save and Cancel Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    QPushButton *saveButton = new QPushButton("Save", this);
    QPushButton *cancelButton = new QPushButton("Cancel", this);
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(cancelButton);
    mainLayout->addLayout(buttonLayout);

    connect(saveButton, &QPushButton::clicked, this, &GuiSetupDialog::saveGuiSettings);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

    setLayout(mainLayout);

    // Lade bestehende Einstellungen
    QString defaultConfigFilePath = "/home/admin/rpicam-gui/rpicam-gui.conf";
    QSettings settings(defaultConfigFilePath, QSettings::IniFormat);
    configFilePathEdit->setText(settings.value("Paths/ConfigFilePath", defaultConfigFilePath).toString());
    outputPathEdit->setText(settings.value("Paths/GuiOutputPath", "/home/admin/rpicam-gui/output").toString());
    postProcessPathEdit->setText(settings.value("Paths/GuiPostProcessPath", "/home/admin/rpicam-apps/assets").toString());
    splashScreenCheckbox->setChecked(settings.value("splashScreenEnabled", true).toBool());
}

GuiSetupDialog::~GuiSetupDialog() {
    // Status der Checkbox speichern
    QSettings settings("rpicam-gui", "settings");
    settings.setValue("splashScreenEnabled", splashScreenCheckbox->isChecked());
}

void GuiSetupDialog::browseConfigFilePath() {
    QString initialPath = configFilePathEdit->text();
    if (initialPath.isEmpty()) {
        initialPath = QCoreApplication::applicationDirPath(); // Standardpfad, falls leer
    }

    QString filePath = QFileDialog::getSaveFileName(this, "Select Configuration File", initialPath, "Config Files (*.conf)");
    if (!filePath.isEmpty()) {
        configFilePathEdit->setText(filePath);
    }
}

void GuiSetupDialog::saveGuiSettings() {
    // Standardpfad für die Konfigurationsdatei definieren
    QString defaultConfigFilePath = "/home/admin/rpicam-gui/rpicam-gui.conf";

    // Konfigurationsdateipfad aus dem Eingabefeld oder Standardwert verwenden
    QString configFilePath = configFilePathEdit->text().isEmpty() ? defaultConfigFilePath : configFilePathEdit->text();

    // QSettings verwenden, um die Konfiguration zu speichern
    QSettings settings(configFilePath, QSettings::IniFormat);

    settings.setValue("Paths/GuiOutputPath", outputPathEdit->text());
    settings.setValue("Paths/GuiPostProcessPath", postProcessPathEdit->text());
    settings.setValue("Paths/ConfigFilePath", configFilePath);
    settings.setValue("splashScreenEnabled", splashScreenCheckbox->isChecked());

    qDebug() << "Configuration saved to:" << configFilePath;
    accept();
}

bool GuiSetupDialog::isSplashScreenEnabled() const {
    return splashScreenCheckbox->isChecked();
}