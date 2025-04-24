Änderungshistorie:

1. [Datum: 09.04.2025]
   - Datei: `MainWindow.h`
   - Änderung: Methode `stopRpiCamApp()` in der Header-Datei `MainWindow.h` deklariert.
   - Grund: Fehler behoben, bei dem die Methode in der `.cpp`-Datei definiert, aber nicht in der Header-Datei deklariert war.

2. [Datum: 09.04.2025]
   - Datei: `MainWindow.cpp`
   - Änderung: Methode `loadConfigurationFromFile()` erweitert, um Breite und Höhe aus der Konfigurationsdatei zu lesen und die Auflösung korrekt zu setzen.
   - Grund: Fehler behoben, bei dem die Auflösung nicht korrekt geladen wurde.
   - Code:
     if (!width.isEmpty() && !height.isEmpty()) {
         QString resolution = width + "x" + height;
         if (resolutionSelector->findText(resolution) == -1) {
             resolutionSelector->addItem(resolution);
         }
         resolutionSelector->setCurrentText(resolution);
     }

3. [Datum: 10.04.2025]
   - Datei: `MainWindow.cpp`
   - Änderung: Methode `saveConfigurationToFile()` hinzugefügt, um die aktuelle Konfiguration in eine Datei zu speichern.
   - Grund: Ermöglichen des Speicherns von Benutzereinstellungen.
   - Code:
     out << "camera=" << cameraSelector->currentText() << "\n";
     out << "qt-preview=" << previewSelector->currentText() << "\n";
     out << "timeout=" << timeoutSelector->currentText() << "\n";

4. [Datum: 10.04.2025]
   - Datei: `MainWindow.cpp`
   - Änderung: Debugging-Ausgaben in `startRpiCamApp()` hinzugefügt, um den vollständigen Befehl und die Vorschauoptionen zu überprüfen.
   - Grund: Verbesserung der Nachvollziehbarkeit bei der Ausführung.
   - Code:
     qDebug() << "Full command:" << fullCommand;
     qDebug() << "Preview Option:" << preview;

5. [Datum: 10.04.2025]
   - Datei: `MainWindow.cpp`
   - Änderung: Methode `updateButtonVisibility()` hinzugefügt, um die Sichtbarkeit der Start- und Stop-Buttons basierend auf dem Prozessstatus zu aktualisieren.
   - Grund: Verbesserung der Benutzeroberfläche.
   - Code:
     if (process.state() == QProcess::NotRunning) {
         startStopButton->setText("Start");
         connect(startStopButton, &QPushButton::clicked, this, &MainWindow::startRpiCamApp);
     } else if (process.state() == QProcess::Running) {
         startStopButton->setText("Stop");
         connect(startStopButton, &QPushButton::clicked, this, &MainWindow::stopRpiCamApp);
     }

6. [Datum: 10.04.2025]
   - Datei: `MainWindow.cpp`
   - Änderung: Methode `startRpiCamApp()` erweitert, um die Post-Process-Datei und die Vorschauoptionen hinzuzufügen.
   - Grund: Unterstützung zusätzlicher Optionen.
   - Code:
     if (!postProcessFile.isEmpty()) {
         arguments << "--post-process-file" << postProcessFilePath;
     }
     if (!Box.isEmpty()) {
         arguments << "--preview" << Box;
     }

7. [Datum: 11.04.2025]
   - Datei: `MainWindow.cpp`
   - Änderung: Methode `stopRpiCamApp()` angepasst, um ein `SIGINT`-Signal an den Prozess zu senden.
   - Grund: Sicherstellen, dass der Prozess sauber beendet wird und Videodateien korrekt finalisiert werden.
   - Code:
     kill(process.processId(), SIGINT);

8. [Datum: 11.04.2025]
   - Datei: `MainWindow.cpp`
   - Änderung: Header-Dateien `<csignal>` und `<unistd.h>` hinzugefügt.
   - Grund: Ermöglichen der Verwendung von `kill()` und `SIGINT`.
   - Code:
     #include <csignal>
     #include <unistd.h>

9. [Datum: 11.04.2025]
   - Datei: `MainWindow.cpp`
   - Änderung: Debugging-Ausgaben in `stopRpiCamApp()` hinzugefügt.
   - Grund: Überprüfung, ob der Prozess korrekt beendet wird.
   - Code:
     qDebug() << "Stopping process with PID:" << process.processId();
     qDebug() << "Process terminated.";

10. [Datum: 11.04.2025]
    - Datei: `MainWindow.cpp`
    - Änderung: Fehlerbehebung bei der Initialisierung von `process` im Konstruktor.
    - Grund: Sicherstellen, dass `process` korrekt initialisiert wird.
    - Code:
      process(this);

11. [Datum: 11.04.2025]
    - Datei: `MainWindow.cpp`
    - Änderung: Methode `updateButtonVisibility()` aufgerufen, um die Sichtbarkeit der Buttons nach Start/Stopp zu aktualisieren.
    - Grund: Verbesserung der Benutzeroberfläche.
    - Code:
      updateButtonVisibility();

12. [Datum: 13.04.2025]
    - Datei: `MainWindow.cpp`
    - Änderung: Tooltips für verschiedene GUI-Elemente hinzugefügt.
    - Grund: Verbesserung der Benutzerfreundlichkeit durch Bereitstellung von Hilfetexten.
    - Code:
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

13. [Datum: 13.04.2025]
    - Datei: `MainWindow.cpp`
    - Änderung: Fehler behoben, bei dem `QDateTime` nicht korrekt eingebunden war.
    - Grund: Sicherstellen, dass die Klasse `QDateTime` verwendet werden kann.
    - Code:
      #include <QDateTime>

14. [Datum: 24.04.2025]
    - Datei: `MainWindow.cpp`
    - Änderung: `x2`-Checkbox hinzugefügt, um die Größe des Vorschaufensters zu verdoppeln.
    - Grund: Verbesserung der Benutzerfreundlichkeit durch eine einfache Möglichkeit, die Vorschaugröße zu ändern.
    - Code:
      doubleSizeCheckbox = new QCheckBox("x2", this);
      connect(doubleSizeCheckbox, &QCheckBox::stateChanged, this, [this](int state) {
          Q_UNUSED(state);
          QString updatedBoxValue = calculateBoxInput(+30);
          BoxInput->setText(updatedBoxValue);
          qDebug() << "BoxInput updated after x2 checkbox state change:" << updatedBoxValue;
      });

15. [Datum: 24.04.2025]
    - Datei: `MainWindow.cpp`
    - Änderung: Funktion `calculateBoxInput` erweitert, um die Verdopplung der Vorschaugröße zu unterstützen.
    - Grund: Dynamische Anpassung der Vorschaugröße basierend auf dem Zustand der `x2`-Checkbox.
    - Code:
      if (doubleSizeCheckbox && doubleSizeCheckbox->isChecked()) {
          boxWidth *= 2;
          boxHeight *= 2;
      }

16. [Datum: 24.04.2025]
    - Datei: `MainWindow.cpp`
    - Änderung: Versionsanzeige beim Programmstart hinzugefügt.
    - Grund: Verbesserung der Benutzerfreundlichkeit durch Anzeige der aktuellen Version.
    - Code:
      QLabel *versionLabel = new QLabel("Version: 1.0.0", this);
      splashScreenLayout->addWidget(versionLabel);

17. [Datum: 24.04.2025]
    - Datei: `resources.qrc`, `MainWindow.cpp`
    - Änderung: Splash Screen mit animierter Grafik implementiert.
    - Grund: Visuelles Feedback beim Start der Anwendung.
    - Code:
      QMovie *splashMovie = new QMovie(":/images/rpicam-gui_splash2.gif");
      splashLabel->setMovie(splashMovie);
      splashMovie->start();

18. [Datum: 24.04.2025]
    - Datei: `MainWindow.cpp`, `MainWindow.h`
    - Änderung: Versionsanzeige im "About"-Dialog wieder hinzugefügt.
    - Grund: Verbesserung der Benutzerfreundlichkeit durch Anzeige der aktuellen Version im "About"-Dialog.
    - Code:
      void MainWindow::showAboutDialog() {
          QMessageBox aboutBox(this);
          aboutBox.setWindowTitle("About rpicam-gui");
          aboutBox.setText(
              "<h3>rpicam-gui</h3>"
              "<p>Version: 1.0.0</p>"
              "<p>rpicam-gui ist eine Desktop-Anwendung zur Steuerung der rpicam-Apps.</p>"
              "<p>Entwickelt von Kletternaut und Beitragenden.</p>"
          );
          aboutBox.setIcon(QMessageBox::Information);
          aboutBox.exec();
      }