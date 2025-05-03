//#include "MainWindow.h"
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
    QGroupBox *configGroup = new QGroupBox("rpicam-gui config file path", this);
    QVBoxLayout *configLayout = new QVBoxLayout(configGroup);
    configFilePathEdit = new QLineEdit(this); // Initialisiere das Eingabefeld
    QPushButton *browseConfigFileButton = new QPushButton("Browse", this);
    configLayout->addWidget(configFilePathEdit);
    configLayout->addWidget(browseConfigFileButton);
    connect(browseConfigFileButton, &QPushButton::clicked, this, &GuiSetupDialog::browseConfigFilePath);
    mainLayout->addWidget(configGroup);

    // Output Files Path Group
    QGroupBox *outputGroup = new QGroupBox("Output file path", this);
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

    // Post-Process Files Path Group
    QGroupBox *postProcessGroup = new QGroupBox("Post-process file path", this);
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

    // LoadSave rpicamConfig files path Group
    QGroupBox *rpicamConfigGroup = new QGroupBox("rpicam config file path", this);
    QVBoxLayout *rpicamLayout = new QVBoxLayout(rpicamConfigGroup); // Richtiges Layout verwenden
    rpicamConfigPathEdit = new QLineEdit(this); // Initialisiere das Eingabefeld
    QPushButton *browserpicamConfigButton = new QPushButton("Browse", this);
    rpicamLayout->addWidget(rpicamConfigPathEdit); // Füge das Eingabefeld zum richtigen Layout hinzu
    rpicamLayout->addWidget(browserpicamConfigButton); // Füge den Button zum richtigen Layout hinzu
    connect(browserpicamConfigButton, &QPushButton::clicked, this, [this]() {
        QString initialPath = rpicamConfigPathEdit->text();
        if (initialPath.isEmpty()) {
            initialPath = "/home/admin/rpicam-gui/config"; // Standardpfad, falls leer
        }

        QString dir = QFileDialog::getExistingDirectory(this, "Select rpicamConfig Directory", initialPath);
        if (!dir.isEmpty()) {
            rpicamConfigPathEdit->setText(dir);
        }
    });
    mainLayout->addWidget(rpicamConfigGroup); // Füge die Gruppe zum Hauptlayout hinzu

    // Custom Applications Group
    setupCustomAppInputs(mainLayout); // Füge die Eingabefelder für CustomApps hinzu

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
    loadGuiSettings();
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
    QString defaultConfigFilePath = "/home/admin/rpicam-gui/rpicam-gui.conf";
    QString configFilePath = configFilePathEdit->text().isEmpty() ? defaultConfigFilePath : configFilePathEdit->text();

    QSettings settings(configFilePath, QSettings::IniFormat);

    settings.setValue("Paths/GuiOutputPath", outputPathEdit->text());
    settings.setValue("Paths/GuiPostProcessPath", postProcessPathEdit->text());
    settings.setValue("Paths/GuiRpicamConfigPath", rpicamConfigPathEdit->text());
    settings.setValue("Paths/ConfigFilePath", configFilePath);
    settings.setValue("splashScreenEnabled", splashScreenCheckbox->isChecked());

    for (int i = 0; i < customAppInputs.size(); ++i) {
        QString key = QString("CustomApp%1").arg(i + 1);
        settings.setValue(key, customAppInputs[i]->text());
    }

    qDebug() << "Configuration saved to:" << configFilePath;
    accept();
}

bool GuiSetupDialog::isSplashScreenEnabled() const {
    return splashScreenCheckbox->isChecked();
}

void GuiSetupDialog::browserpicamConfigFilePath() {
    QString initialPath = rpicamConfigPathEdit->text();
    if (initialPath.isEmpty()) {
        initialPath = "/home/admin/rpicam-gui/config"; // Standardpfad, falls leer
    }

    QString dir = QFileDialog::getExistingDirectory(this, "Select rpicamConfig Directory", initialPath);
    if (!dir.isEmpty()) {
        rpicamConfigPathEdit->setText(dir);
    }
}

void GuiSetupDialog::setupCustomAppInputs(QVBoxLayout *layout) {
    QGroupBox *customAppsGroup = new QGroupBox("Custom Applications", this);
    QVBoxLayout *customAppsLayout = new QVBoxLayout(customAppsGroup);

    for (int i = 0; i < 3; ++i) { // Maximal 5 benutzerdefinierte Apps
        QLineEdit *input = new QLineEdit(this);
        input->setPlaceholderText(QString("Custom App %1").arg(i + 1)); // Platzhaltertext hinzufügen
        customAppsLayout->addWidget(input);
        customAppInputs.append(input); // Eingabefeld speichern
    }

    layout->addWidget(customAppsGroup);
}

void GuiSetupDialog::loadGuiSettings() {
    QString defaultConfigFilePath = "/home/admin/rpicam-gui/rpicam-gui.conf";

    // Erzwinge das Neuladen der Einstellungen
    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings settings(defaultConfigFilePath, QSettings::IniFormat);

    configFilePathEdit->setText(settings.value("Paths/ConfigFilePath", defaultConfigFilePath).toString());
    outputPathEdit->setText(settings.value("Paths/GuiOutputPath", "/home/admin/rpicam-gui/output").toString());
    postProcessPathEdit->setText(settings.value("Paths/GuiPostProcessPath", "/home/admin/rpicam-apps/assets").toString());
    rpicamConfigPathEdit->setText(settings.value("Paths/GuiRpicamConfigPath", "/home/admin/rpicam-gui/config").toString());
    splashScreenCheckbox->setChecked(settings.value("splashScreenEnabled", true).toBool());

    // Leere die Listen
    customAppEntries.clear();
    for (QLineEdit *input : customAppInputs) {
        input->clear();
    }

    // Lade die CustomApps in customAppEntries
    for (int i = 0; i < 3; ++i) {
        QString key = QString("CustomApp%1").arg(i + 1);
        QString value = settings.value(key, "").toString();
        if (!value.isEmpty()) { // Nur nicht-leere Werte hinzufügen
            customAppEntries.append(value);
        }
    }

    // Synchronisiere die Werte mit den QLineEdit-Feldern
    for (int i = 0; i < customAppInputs.size(); ++i) {
        if (i < customAppEntries.size()) {
            customAppInputs[i]->setText(customAppEntries[i]);
        } else {
            customAppInputs[i]->clear();
        }
    }
}

QStringList GuiSetupDialog::getCustomAppEntries() const {
    QStringList entries;
    for (QLineEdit *input : customAppInputs) {
        if (!input->text().isEmpty()) {
            entries.append(input->text());
        }
    }
    return entries;
}
