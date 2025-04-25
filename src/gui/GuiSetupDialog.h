#ifndef GUISETUPDIALOG_H
#define GUISETUPDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>

class GuiSetupDialog : public QDialog {
    Q_OBJECT

public:
    explicit GuiSetupDialog(QWidget *parent = nullptr);
    ~GuiSetupDialog();

    bool isSplashScreenEnabled() const; // Getter für den Splash-Screen-Status

private slots:
    void saveGuiSettings();
    void browseConfigFilePath(); // Neue Methode für den "Browse"-Button
    void browserpicamConfigFilePath(); // Neue Methode für den "Browse"-Button

private:
    QLineEdit *outputPathEdit;
    QLineEdit *postProcessPathEdit;
    QLineEdit *rpicamConfigPathEdit;
    QLineEdit *configFilePathEdit; // Neues Eingabefeld für den Speicherort der Konfigurationsdatei
    QCheckBox *splashScreenCheckbox; // Checkbox für den Splash Screen
};

#endif // GUISETUPDIALOG_H
