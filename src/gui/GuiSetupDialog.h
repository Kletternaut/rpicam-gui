#ifndef GUISETUPDIALOG_H
#define GUISETUPDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>

class GuiSetupDialog : public QDialog {
    Q_OBJECT

public:
    explicit GuiSetupDialog(QWidget *parent = nullptr);

private slots:
    void saveGuiSettings();
    void browseConfigFilePath(); // Neue Methode für den "Browse"-Button

private:
    QLineEdit *outputPathEdit;
    QLineEdit *postProcessPathEdit;
    QLineEdit *configFilePathEdit; // Neues Eingabefeld für den Speicherort der Konfigurationsdatei
};

#endif // GUISETUPDIALOG_H