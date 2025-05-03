#include "MainWindow.h"
#include <QDateTime>
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <QProcess>
#include <QMessageBox>
#include <csignal>
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include "Defaults.h"

void MainWindow::startRpiCamApp() {
    QString app = appSelector->currentText();
    QString camera = cameraSelector->currentText();
    QString resolution = resolutionSelector->currentText();
    QString framerate = framerateSelector->currentText();
    QString preview = previewSelector->currentText();
    QString timeout = timeoutSelector->currentText();
    QString outputFile = outputFileName->text();
    QString path = guiOutputFilePath; // Initialisiere path mit guiOutputFilePath
    QString timelapse = timelapseInput->text();
    bool segmentationEnabled = segmentationCheckbox->isChecked();
    if (timestampCheckbox->isChecked() && !outputFile.isEmpty()) {
        QDateTime now = QDateTime::currentDateTime();
        QString timestamp = now.toString("yyyy-MM-dd_HH-mm-ss");
        QFileInfo fileInfo(outputFile);
        QString baseName = fileInfo.completeBaseName();
        QString suffix = fileInfo.suffix();
        QString directory = fileInfo.path();
        if (directory.isEmpty() || directory == ".") {
            directory = guiOutputFilePath;
        }
        outputFile = QDir(directory).filePath(baseName + "_" + timestamp + "." + suffix);
        outputLog->append("Output file with timestamp: " + outputFile);
    }
    QStringList resolutionParts = resolution.split("x");
    QString width = resolutionParts.value(0);
    QString height = resolutionParts.value(1);
    QStringList arguments;
    arguments << "--camera" << camera
              << "--width" << width
              << "--height" << height
              << "--framerate" << framerate;
    QString previewValue = previewSelector->currentData().toString();
    if (!previewValue.isEmpty()) {
        arguments << previewValue; // Füge den Parameter mit Präfix hinzu
    }
    if (!timeout.isEmpty()) {
        bool ok;
        int timeoutMs = timeout.toInt(&ok);
        if (ok) {
            arguments << "-t" << QString::number(timeoutMs);
        } else {
            outputLog->append("Invalid timeout value. Using default.");
            arguments << "-t" << "0";
        }
    } else {
        arguments << "-t" << "0";
    }
    if (!timelapse.isEmpty()) {
        bool ok;
        int timelapseMs = timelapse.toInt(&ok);
        if (ok) {
            arguments << "--timelapse" << QString::number(timelapseMs);
        } else {
            outputLog->append("Invalid timelapse value. Skipping.");
        }
    }
    if (segmentationEnabled) {
        arguments << "--segment" << "_%04d";
    }
    if (!outputFile.isEmpty() && !QFileInfo(outputFile).isAbsolute()) {
        outputFile = QDir(path).filePath(outputFile); // Kombiniere mit guiOutputFilePath
    }
    if (!outputFile.isEmpty()) {
        arguments << "-o" << outputFile;
    } else {
    }
    QString postProcessFile = postProcessFileSelector->currentText();
    if (!postProcessFile.isEmpty()) {
        QString postProcessFilePath;
        if (QFileInfo(postProcessFile).isAbsolute()) {
            postProcessFilePath = postProcessFile;
        } else {
            postProcessFilePath = QDir(guiPostProcessFilePath).filePath(postProcessFile); // <-- dynamischer Pfad aus Setup
        }
        arguments << "--post-process-file" << postProcessFilePath;
    }
    QString Box = BoxInput->text();
    if (!Box.isEmpty()) {
        arguments << "--preview" << Box;
    }
    if (app == "rpicam-vid") {
        QString codec = codecSelector->currentText();
        if (!codec.isEmpty()) {
            arguments << "--codec" << codec;
        }
    }
    QString awb = awbSelector->currentText();
    if (awb != "auto") { // Nur hinzufügen, wenn der Wert nicht "auto" ist
        arguments << "--awb" << awb;
    }
    double sharpness = sharpnessInput->text().toDouble();
    if (sharpness != 1.0) { // Nur hinzufügen, wenn nicht Standardwert
        arguments << "--sharpness" << QString::number(sharpness, 'f', 1);
    }
    double ev = evInput->text().toDouble();
    if (ev != 0.0) { // Nur hinzufügen, wenn nicht Standardwert
        arguments << "--ev" << QString::number(ev, 'f', 1);
    }
    double gain = gainInput->text().toDouble();
    if (gain != 0.0) { // Nur hinzufügen, wenn nicht Standardwert
        arguments << "--gain" << QString::number(gain, 'f', 1);
    }
    double brightness = brightnessInput->text().toDouble();
    if (brightness != 0.0) { // Nur hinzufügen, wenn nicht Standardwert
        arguments << "--brightness" << QString::number(brightness, 'f', 1);
    }
    double contrast = contrastInput->text().toDouble();
    if (contrast != 1.0) { // Nur hinzufügen, wenn nicht Standardwert
        arguments << "--contrast" << QString::number(contrast, 'f', 1);
    }
    double saturation = saturationInput->text().toDouble();
    if (saturation != 1.0) { // Nur hinzufügen, wenn nicht Standardwert
        arguments << "--saturation" << QString::number(saturation, 'f', 1);
    }
    // ...vor dem Start...
    if (hflipCheckbox->isChecked()) {
        arguments << "--hflip";
    }
    if (vflipCheckbox->isChecked()) {
        arguments << "--vflip";
    }

    if (rotationCheckbox && rotationCheckbox->isChecked()) {
        arguments << "--rotation" << "180";
    }
    // ...existing code...
    QString fullCommand = app + " " + arguments.join(" ");
    qDebug().noquote() << "Full command:" << fullCommand;
    process.start(app, arguments);
    if (!process.waitForStarted()) {
    } else {
    }
    updateButtonVisibility();
}

// --- Ab hier die ausgelagerten Funktionen ---

void MainWindow::saveConfigurationToFile(const QString &filePath) {
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);

        // Speichere die Kamera, falls gesetzt
        if (!cameraSelector->currentText().isEmpty()) {
            out << "camera=" << cameraSelector->currentText() << "\n";
        }

        // Speichere qt-preview, fullscreen oder nopreview
        QString previewValue = previewSelector->currentData().toString();
        if (previewValue == "--qt-preview") {
            out << "qt-preview=\n"; // Speichere qt-preview
        } else if (previewValue == "--fullscreen") {
            out << "fullscreen=1\n"; // Speichere fullscreen
        } else if (previewValue == "--nopreview") {
            out << "nopreview=1\n"; // Speichere nopreview
        }

        // Speichere Vorschaufenstermaße (preview=x,y,w,h)
        if (!BoxInput->text().isEmpty()) {
            out << "preview=" << BoxInput->text() << "\n";
        }

        QString width = resolutionSelector->currentText().split("x").value(0);
        QString height = resolutionSelector->currentText().split("x").value(1);
        if (!width.isEmpty() && !height.isEmpty()) {
            out << "width=" << width << "\n";
            out << "height=" << height << "\n";
        }

        if (!framerateSelector->currentText().isEmpty()) {
            out << "framerate=" << framerateSelector->currentText() << "\n";
        }

        if (!timeoutSelector->currentText().isEmpty()) {
            out << "timeout=" << timeoutSelector->currentText() << "\n";
        }

        if (awbSelector->currentText() != "auto") {
            out << "awb=" << awbSelector->currentText() << "\n";
        }

        double sharpness = sharpnessInput->text().toDouble();
        if (sharpness != DEFAULT_SHARPNESS) {
            out << "sharpness=" << QString::number(sharpness, 'f', 1) << "\n";
        }

        double ev = evInput->text().toDouble();
        if (ev != DEFAULT_EV) {
            out << "ev=" << QString::number(ev, 'f', 1) << "\n";
        }

        double gain = gainInput->text().toDouble();
        if (gain != DEFAULT_GAIN) {
            out << "gain=" << QString::number(gain, 'f', 1) << "\n";
        }

        double brightness = brightnessInput->text().toDouble();
        if (brightness != DEFAULT_BRIGHTNESS) {
            out << "brightness=" << QString::number(brightness, 'f', 1) << "\n";
        }

        double contrast = contrastInput->text().toDouble();
        if (contrast != DEFAULT_CONTRAST) {
            out << "contrast=" << QString::number(contrast, 'f', 1) << "\n";
        }

        double saturation = saturationInput->text().toDouble();
        if (saturation != DEFAULT_SATURATION) {
            out << "saturation=" << QString::number(saturation, 'f', 1) << "\n";
        }

        // Speichere den Codec, wenn die App rpicam-vid ist
        if (appSelector->currentText() == "rpicam-vid") {
            QString codec = codecSelector->currentText();
            if (!codec.isEmpty()) { // Speichere den Codec nur, wenn er nicht leer ist
                out << "codec=" << codec << "\n";
                qDebug() << "[DEBUG] Codec saved as:" << codec;
            }
        }

        // Speichere den vollständigen Pfad der Post-Process-Datei, falls vorhanden
        QString postProcessFile = postProcessFileSelector->currentText();
        if (!postProcessFile.isEmpty() && postProcessFile.trimmed() != "") {
            QString postProcessFilePath = "/home/admin/rpicam-apps/assets/" + postProcessFile;
            out << "post-process-file=" << postProcessFilePath << "\n";
        }

        // Speichere das Output-File, falls vorhanden
        QString outputFile = outputFileName->text();
        if (!outputFile.isEmpty()) {
            QString outputFilePath;
            if (QDir::isAbsolutePath(outputFile)) {
                // Wenn der Pfad bereits absolut ist, verwende ihn direkt
                outputFilePath = QDir::cleanPath(outputFile);
            } else {
                // Wenn der Pfad relativ ist, füge das Basisverzeichnis hinzu
                outputFilePath = QDir::cleanPath("/home/admin/rpicam-output/" + outputFile);
            }
            out << "output=" << outputFilePath << "\n";
        }

        // Speichere den Timelapse-Wert, falls gesetzt
        QString timelapseValue = timelapseInput->text();
        if (!timelapseValue.isEmpty()) {
            out << "timelapse=" << timelapseValue << "\n";
            qDebug() << "[DEBUG] Timelapse saved as:" << timelapseValue;
        }

        // ...nach timelapse...
        if (hflipCheckbox && hflipCheckbox->isChecked()) {
            out << "hflip=1\n";
        }
        if (vflipCheckbox && vflipCheckbox->isChecked()) {
            out << "vflip=1\n";
        }
        if (rotationCheckbox && rotationCheckbox->isChecked()) {
            out << "rotation=180\n";
        }

        file.close();
        outputLog->append("Configuration saved to " + filePath);
    } else {
        outputLog->append("Failed to save configuration to " + filePath);
    }
}

void MainWindow::loadConfigurationFromFile(const QString &filePath) {
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);

        QString width, height; // Temporäre Variablen für Breite und Höhe

        while (!in.atEnd()) {
            QString line = in.readLine().trimmed();
            if (line.startsWith("#") || line.isEmpty()) {
                continue; // Kommentare und leere Zeilen überspringen
            }

            QStringList parts = line.split("=", Qt::KeepEmptyParts);
            if (parts.size() != 2) {
                continue; // Ungültige Zeilen überspringen
            }

            QString key = parts[0].trimmed();
            QString value = parts[1].trimmed();

            if (key == "camera") {
                cameraSelector->setCurrentText(value);
            } else if (key == "qt-preview") {
                previewSelector->setCurrentIndex(previewSelector->findData("--qt-preview"));
            } else if (key == "fullscreen") {
                previewSelector->setCurrentIndex(previewSelector->findData("--fullscreen"));
            } else if (key == "nopreview") {
                previewSelector->setCurrentIndex(previewSelector->findData("--nopreview"));
            } else if (key == "timeout") {
                timeoutSelector->setCurrentText(value);
            } else if (key == "preview") {
                BoxInput->setText(value);
            } else if (key == "width") {
                width = value; // Speichere die Breite
            } else if (key == "height") {
                height = value; // Speichere die Höhe
            } else if (key == "framerate") {
                framerateSelector->setCurrentText(value);
            } else if (key == "awb") {
                awbSelector->setCurrentText(value);
            } else if (key == "sharpness") {
                sharpnessInput->setText(value);
                sharpnessSlider->setValue(static_cast<int>(value.toDouble() * 10));
            } else if (key == "ev") {
                evInput->setText(value);
                evSlider->setValue(static_cast<int>(value.toDouble() * 10));
            } else if (key == "gain") {
                gainInput->setText(value);
                gainSlider->setValue(static_cast<int>(value.toDouble() * 10));
            } else if (key == "brightness") {
                brightnessInput->setText(value);
                brightnessSlider->setValue(static_cast<int>(value.toDouble() * 10));
            } else if (key == "contrast") {
                contrastInput->setText(value);
                contrastSlider->setValue(static_cast<int>(value.toDouble() * 10));
            } else if (key == "saturation") {
                saturationInput->setText(value);
                saturationSlider->setValue(static_cast<int>(value.toDouble() * 10));
            } else if (key == "post-process-file") {
                postProcessFileSelector->setCurrentText(value);
            } else if (key == "output") {
                outputFileName->setText(value);
            } else if (key == "timelapse") {
                timelapseInput->setText(value);
            }
        }

        // Setze die Auflösung, falls Breite und Höhe vorhanden sind
        if (!width.isEmpty() && !height.isEmpty()) {
            QString resolution = width + "x" + height;
            if (resolutionSelector->findText(resolution) == -1) {
                resolutionSelector->addItem(resolution); // Füge die Auflösung hinzu, falls sie nicht existiert
            }
            resolutionSelector->setCurrentText(resolution); // Setze die aktuelle Auflösung
        }

        file.close();
        outputLog->append("Configuration loaded from " + filePath);

        // Aktualisiere die GUI
        cameraSelector->update();
        previewSelector->update();
        resolutionSelector->update();
        framerateSelector->update();
        postProcessFileSelector->update();
    } else {
        outputLog->append("Failed to load configuration from " + filePath);
    }
}

void MainWindow::parseListCamerasOutput(const QString &output) {
    cameraSelector->clear();
    resolutionSelector->clear();
    framerateSelector->clear();
    cameraDetails.clear();
    QStringList lines = output.split("\n", Qt::SkipEmptyParts);
    for (const QString &line : lines) {
        QRegularExpression regexCamera(R"(^(\d+)\s*:\s*(.+))");
        QRegularExpression regexMode(R"((\d+x\d+)\s*\[\s*([\d.]+)\s*fps)");
        QRegularExpressionMatch matchCamera = regexCamera.match(line);
        if (matchCamera.hasMatch()) {
            QString cameraIndex = matchCamera.captured(1);
            QString cameraDescription = matchCamera.captured(2);
            cameraSelector->addItem(cameraIndex);
            cameraDetails[cameraIndex] = cameraDescription;
        }
        QRegularExpressionMatch matchMode = regexMode.match(line);
        if (matchMode.hasMatch()) {
            QString resolution = matchMode.captured(1);
            QString framerate = matchMode.captured(2);
            if (!resolutionSelector->findText(resolution)) {
                resolutionSelector->addItem(resolution);
            }
            if (!framerateSelector->findText(framerate)) {
                framerateSelector->addItem(framerate + " fps");
            }
        }
    }
    resolutionSelector->addItem("1332x990");
    resolutionSelector->addItem("2028x1080");
    resolutionSelector->addItem("2028x1520");
    resolutionSelector->addItem("4056x3040");
    if (cameraSelector->count() == 0) {
        outputLog->append("No cameras found.");
    } else {
        outputLog->append("Cameras detected and added to the dropdown.");
    }
}

void MainWindow::stopRpiCamApp() {
    if (process.state() == QProcess::Running) {
        kill(process.processId(), SIGINT);
        if (!process.waitForFinished(5000)) { // Warte bis zu 5 Sekunden
            process.kill(); // Erzwinge das Beenden, falls der Prozess nicht reagiert
            process.waitForFinished();
        }
        outputLog->append("Process terminated.");
    } else {
        outputLog->append("No running process to terminate.");
    }
    updateButtonVisibility();
}