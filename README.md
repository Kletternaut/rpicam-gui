# rpicam-gui

![Splash Screen](resources/images/rpicam-gui_splash2.gif)

**rpicam-gui** is a desktop application for controlling the Raspberry Pi's rpicam-apps (`rpicam-vid`, `rpicam-jpeg`, `rpicam-still`, `rpicam-raw`, `rpicam-hello`). The application provides a user-friendly interface to configure and start various parameters of these rpicam-apps.

**rpicam-gui** ist eine Desktop-Anwendung zur Steuerung der rpicam-Apps (`rpicam-vid`, `rpicam-jpeg`, `rpicam-still`, `rpicam-raw`, `rpicam-hello`). Die Anwendung bietet eine benutzerfreundliche Oberfläche, um verschiedene Parameter der rpicam-apps zu konfigurieren und diese Apps zu starten.


**Demo Video**
The video was recorded on a Raspberry Pi 5 / 8GB with a Dual HQ-Camera, Pimoroni NVMe Base Duo with an NVMe SSD, and a Hailo8 AI processor via xrdp Remote Desktop. It showcases the early and not yet error-free rpicam-gui version 0.0.65, demonstrating its basic usage and features. The primary goal of this app was and still is to fulfill the need for a quickly operable camera.

Das video wurde auf einem Raspberry Pi5 / 8GB mit Dual HQ-Camera,Pimoroni NVMe Base Duo mit einer NVMe-SSD und einem Hailo8-AI Prozessor über xrdp-Remote Desktop aufgenommen. Es zeigt die noch frühe und nicht fehlerfreie rpicam-gui Version 0.0.65. in der grundlegenden Handhabung und deren Features. Das dem allenvoran gestellte Ziel dieser App war und ist aus dem Bedürfnis entstanden, eine schnell aufnahmefähige Camera zu haben.

![Splash Screen](resources/videos/rpicam-gui.mp4)

## **Projektstruktur**

```
rpicam-gui
├── src
│   ├── main.cpp                # Einstiegspunkt der Anwendung
│   ├── gui
│   │   ├── MainWindow.cpp      # Implementierung der Hauptfenster-Logik
│   │   ├── MainWindow.h        # Deklarationen der MainWindow-Klasse
│   │   ├── GuiSetupDialog.cpp  # Dialog zur Konfiguration der GUI
│   │   ├── GuiSetupDialog.h    # Deklarationen des GuiSetupDialog
│   │   ├── SelectionOverlay.cpp # Overlay für visuelle Auswahl
│   │   └── SelectionOverlay.h   # Deklarationen des SelectionOverlay
│   ├── utils
│   │   ├── CommandExecutor.cpp # Ausführung von Shell-Befehlen
│   │   ├── CommandExecutor.h   # Deklarationen der CommandExecutor-Klasse
│   │   ├── DebugLogger.cpp     # Debugging- und Logging-Funktionen
│   │   └── DebugLogger.h       # Deklarationen des DebugLogger
│   ├── CustomLineEdit.h        # Benutzerdefinierte Eingabefelder
│   └── Version.h               # Definiert die aktuelle Version der Anwendung
├── resources
│   ├── images                  # Enthält Bilder und Splash-Screens
│   │   └── rpicam-gui_splash2.gif # Animiertes GIF für den Splash Screen
│   └── resources.qrc           # Qt-Ressourcendatei
├── CMakeLists.txt              # CMake-Konfigurationsdatei
├── README.md                   # Dokumentation des Projekts
├── CHANGELOG.md                # Änderungsverlauf
└── LICENSE                     # Lizenzinformationen
```

---

## **Neue Funktionen**

### **Splash Screen**
- Beim Start der Anwendung wird ein animierter Splash Screen angezeigt.
- Die aktuelle Versionsnummer wird unten rechts im Splash Screen eingeblendet.

### **Versionsanzeige**
- Die aktuelle Version der Anwendung wird beim Start im Splash Screen angezeigt.
- Beispiel: `Version: 1.0.0`

### **Versionsanzeige im "About"-Dialog**
- Die aktuelle Version der Anwendung wird im "About"-Dialog angezeigt.
- Zugriff über das Menü "Help" > "About".

### **Konfigurationsdateien**
- Konfigurationsdateien können geladen, bearbeitet und gespeichert werden.
- Unterstützung für benutzerdefinierte Parameter und automatische Validierung.

### **Visuelle Auswahl**
- Die `SelectionOverlay`-Klasse ermöglicht es, visuelle Bereiche in der Benutzeroberfläche auszuwählen und zu bearbeiten.

### Neue Funktionen (Stand: 24.04.2025)

1. **`x2`-Checkbox für die Vorschaugröße**:
   - Eine neue Checkbox wurde hinzugefügt, mit der die Größe des Vorschaufensters verdoppelt werden kann.
   - Aktivieren Sie die Checkbox `x2`, um die Breite und Höhe der Vorschau zu verdoppeln. Deaktivieren Sie sie, um zur Standardgröße zurückzukehren.

2. **Dynamische Anpassung der Vorschaugröße**:
   - Die Vorschaugröße wird automatisch neu berechnet, wenn die Checkbox `x2` aktiviert oder deaktiviert wird.
   - Die berechneten Werte werden im Eingabefeld `BoxInput` angezeigt.

### Verwendung der `x2`-Checkbox

1. **Aktivieren der Checkbox**:
   - Klicken Sie auf die Checkbox `x2`, um die Vorschaugröße zu verdoppeln.
   - Die neuen Werte werden automatisch im Eingabefeld `BoxInput` angezeigt.

2. **Deaktivieren der Checkbox**:
   - Deaktivieren Sie die Checkbox, um zur Standardgröße zurückzukehren.
   - Die Vorschaugröße wird entsprechend angepasst.

### Debugging

- Verwenden Sie die Debugging-Ausgaben, um die berechneten Werte zu überprüfen:
  - Beispiel: `BoxInput updated after x2 checkbox state change: "x,y,width,height"`

### Anforderungen

- Qt 5.15 oder höher
- C++17-kompatibler Compiler

---

## **Installation**

1. **Repository klonen:**
   ```bash
   git clone <repository-url>
   cd rpicam-gui
   ```

2. **Erforderliche Abhängigkeiten installieren:**
   - **Qt5 oder Qt6** (Widgets, Core, GUI)
   - **CMake** (mindestens Version 3.16)
   - **Compiler** (z. B. GCC oder Clang)

3. **Projekt erstellen:**
   ```bash
   mkdir build
   cd build
   cmake ..
   make
   ```

---

## **Verwendung**

1. **Anwendung starten:**
   ```bash
   ./rpicam-gui
   ```

2. **Funktionen der Benutzeroberfläche:**
   - **Parameter einstellen:** Wählen Sie die gewünschten Parameter für die rpicam-Apps aus.
   - **Konfiguration laden:** Öffnen Sie bestehende Konfigurationsdateien.
   - **Konfiguration speichern:** Speichern Sie Ihre Einstellungen in einer Datei.
   - **Apps starten:** Starten Sie die rpicam-Apps direkt aus der Benutzeroberfläche.

---

## **Beispiel für den Splash Screen**

Beim Start der Anwendung wird ein animierter Splash Screen angezeigt, der wie folgt aussieht:

- **GIF:** `rpicam-gui_splash2.gif`
- **Versionsnummer:** Wird unten rechts angezeigt (z. B. `Version: 1.0.0`).

---

## **Lizenz**

Dieses Projekt steht unter der [Bier-Lizenz](LICENSE).

---

## **Mitwirkende**

- **Hauptentwickler:** [Kletternaut]
- **Beitragende:** [Github-Copilot]

---

Falls Sie Fragen oder Probleme haben, erstellen Sie bitte ein Issue im Repository.
