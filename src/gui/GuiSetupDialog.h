#ifndef GUISETUPDIALOG_H
#define GUISETUPDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QVBoxLayout>

class GuiSetupDialog : public QDialog {
    Q_OBJECT

public:
    explicit GuiSetupDialog(QWidget *parent = nullptr);
    ~GuiSetupDialog();

    bool isSplashScreenEnabled() const;
    void loadGuiSettings();
    QStringList getCustomAppEntries() const;

private slots:
    void saveGuiSettings();
    void browseConfigFilePath();
    void browserpicamConfigFilePath();

private:
    QLineEdit *outputPathEdit;
    QLineEdit *postProcessPathEdit;
    QLineEdit *rpicamConfigPathEdit;
    QLineEdit *configFilePathEdit;
    QCheckBox *splashScreenCheckbox;

    QStringList customAppEntries; // Liste für benutzerdefinierte Apps
    QList<QLineEdit *> customAppInputs; // Eingabefelder für benutzerdefinierte Apps

    void setupCustomAppInputs(QVBoxLayout *layout); // Methode zur Einrichtung der Eingabefelder
};

#endif // GUISETUPDIALOG_H
