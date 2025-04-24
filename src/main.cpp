#include <QApplication>
#include <QLabel>
#include <QMovie>
#include <QTimer>
#include <QScreen>
#include <QSettings>
#include "MainWindow.h"
#include "../Version.h" // Header für die Versionsnummer einbinden

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Splash Screen nur anzeigen, wenn aktiviert
    QSettings settings("rpicam-gui", "settings");
    bool splashScreenEnabled = settings.value("splashScreenEnabled", true).toBool();

    QLabel splash;
    QMovie *movie = new QMovie("/home/admin/rpicam-gui/resources/images/rpicam-gui_splash2.gif");
    splash.setMovie(movie);
    splash.setFixedSize(400, 473); // Größe des Splash-Fensters
    splash.setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    splash.setAlignment(Qt::AlignCenter); // Zentriere das GIF

    if (splashScreenEnabled) {
        // Splash Screen zentrieren und anzeigen
        QRect screenGeometry = QApplication::primaryScreen()->geometry();
        int x = (screenGeometry.width() - splash.width()) / 2;
        int y = (screenGeometry.height() - splash.height()) / 2;
        splash.move(x, y); // Splash Screen zentrieren

        // Versionsnummer hinzufügen
        QLabel *versionLabel = new QLabel(&splash); // Dynamisch erstellen
        versionLabel->setText(QString("Version: %1").arg(APP_VERSION)); // APP_VERSION aus Version.h
        versionLabel->setStyleSheet("color: white; font-size: 16px;");
        versionLabel->setGeometry(10, splash.height() - 30, splash.width() - 20, 20); // Abstand von 10px zu beiden Seiten
        versionLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter); // Rechtsbündig und vertikal zentriert

        movie->start(); // Animation starten
        splash.show();
    }

    // Hauptfenster erstellen
    MainWindow mainWindow;

    // Splash Screen für 3 Sekunden anzeigen lassen
    if (splashScreenEnabled) {
        QTimer::singleShot(3000, &splash, &QWidget::close); // Splash schließen
        QTimer::singleShot(3000, &mainWindow, &MainWindow::show); // Hauptfenster anzeigen
    } else {
        mainWindow.show(); // Direkt anzeigen, wenn Splash deaktiviert ist
    }

    return app.exec();
}