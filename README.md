# rpicam-gui
<<<<<<< HEAD
Gui controller for the rpicam-apps
=======

rpicam-gui is a desktop application for controlling the raspberry pi's rpicam-apps (rpicam-vid, rpicam-jpeg, rpicam-still, rpicam-raw, rpicam-hello). The application provides an user-friendly interface to configure and start various parameters of these rpicam-apps.

rpicam-gui ist eine Desktop-Anwendung zur Steuerung der rpicam-Apps (rpicam-vid, rpicam-jpeg, rpicam-still, rpicam-raw, rpicam-hello). Die Anwendung bietet eine benutzerfreundliche Oberfläche, um verschiedene Parameter der rpicam-apps zu konfigurieren und diese Apps zu starten.

## Projektstruktur

```
rpicam-gui
├── src
│   ├── main.cpp                # Einstiegspunkt der Anwendung
│   ├── gui
│   │   ├── MainWindow.cpp      # Implementierung der Hauptfenster-Logik
│   │   ├── MainWindow.h        # Deklarationen der MainWindow-Klasse
│   │   └── MainWindow.ui       # Layout der Benutzeroberfläche
│   ├── controllers
│   │   ├── RpiCamController.cpp # Logik zur Steuerung der rpicam-Apps
│   │   └── RpiCamController.h   # Deklarationen der RpiCamController-Klasse
│   ├── models
│   │   ├── RpiCamParams.cpp     # Verwaltung der rpicam-Parameter
│   │   └── RpiCamParams.h       # Deklarationen der RpiCamParams-Klasse
│   └── utils
│       ├── CommandExecutor.cpp   # Ausführung von Shell-Befehlen
│       └── CommandExecutor.h     # Deklarationen der CommandExecutor-Klasse
├── CMakeLists.txt              # CMake-Konfigurationsdatei
├── README.md                    # Dokumentation des Projekts
└── LICENSE                      # Lizenzinformationen
```

## Installation

1. Klonen Sie das Repository:
   ```
   git clone <repository-url>
   cd rpicam-gui
   ```

2. Installieren Sie die erforderlichen Abhängigkeiten (z.B. Qt, CMake).

3. Erstellen Sie das Projekt:
   ```
   mkdir build
   cd build
   cmake ..
   make
   ```

## Verwendung

Starten Sie die Anwendung mit dem folgenden Befehl:
```
./rpicam-gui
```
The user interface allows you to set the parameters for the rpicam apps and start the desired apps. You can load and edit existing config files, as well as easily create and test new configurations without having to type a lot.

Die Benutzeroberfläche ermöglicht es Ihnen, die Parameter für die rpicam-apps einzustellen und die gewünschten Apps zu starten. Sie können die Konfiguration mit bereits vorhandenen config-Dateien laden und bearbeiten, sowie natürlich auch neue Konfigurationen bequem erstellen und testen, ohne große Tipperei.
>>>>>>> 0156e75 (rpicam-gui Initial commit)
