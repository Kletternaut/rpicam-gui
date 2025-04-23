#include "SelectionOverlay.h"
#include <QPainter>
#include <QMouseEvent>
#include <QDebug>

SelectionOverlay::SelectionOverlay(QWidget *parent)
    : QWidget(parent), isSelecting(false) {
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint); // Ohne Rahmen und immer im Vordergrund
    setAttribute(Qt::WA_TranslucentBackground); // Transparenter Hintergrund
    setAttribute(Qt::WA_ShowWithoutActivating); // Nicht den Fokus stehlen
    this->setStyleSheet("background: transparent;");

    // Setze den Hintergrund explizit auf transparent
    QPalette palette = this->palette();
    palette.setColor(QPalette::Window, Qt::transparent);
    this->setPalette(palette);

    qDebug() << "SelectionOverlay erstellt mit Geometrie:" << geometry();
}

QRect SelectionOverlay::getSelectedArea() const {
    return selectionRect;
}

void SelectionOverlay::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.setBrush(Qt::NoBrush); // Kein Hintergrund
    painter.setPen(QPen(Qt::red, 2, Qt::DashLine)); // Roter Rahmen
    painter.drawRect(selectionRect); // Zeichnet das Auswahlrechteck

    // Zeichne die Position und Größe der Auswahl
    if (!selectionRect.isNull()) {
        QString selectionInfo = QString("x: %1, y: %2, w: %3, h: %4")
                                    .arg(selectionRect.x())
                                    .arg(selectionRect.y())
                                    .arg(selectionRect.width())
                                    .arg(selectionRect.height());
        painter.setPen(Qt::white); // Weißer Text
        painter.drawText(selectionRect.topLeft() + QPoint(5, -5), selectionInfo); // Text leicht oberhalb der Auswahl
    }
}

void SelectionOverlay::mousePressEvent(QMouseEvent *event) {
    isSelecting = true;
    startPoint = event->pos();
    selectionRect = QRect(); // Alte Selektion zurücksetzen
    qDebug() << "Neue Selektion gestartet. Alte Selektion zurückgesetzt. Startpunkt:" << startPoint;
    update();
}

// Die Implementierung der Methode in der Implementierungsdatei
void SelectionOverlay::mouseMoveEvent(QMouseEvent *event) {
    if (isSelecting) {
        QRect rawRect = QRect(startPoint, event->pos()).normalized();

        int x = (rawRect.x() / 10) * 10;
        int y = (rawRect.y() / 10) * 10;
        int width = ((rawRect.width() + 5) / 10) * 10;
        int height = ((rawRect.height() + 5) / 10) * 10;

        selectionRect = QRect(x, y, width, height);

        // Entferne die Debug-Ausgabe hier
        // qDebug() << "Selection updated:" << selectionRect;

        emit selectionChanged(selectionRect);
        update();
    }
}

void SelectionOverlay::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        // Debug-Ausgabe nur beim Abschluss der Selektion
        qDebug() << "Selektion abgeschlossen. Auswahlrechteck:"
                 << "x=" << selectionRect.x()
                 << ", y=" << selectionRect.y()
                 << ", width=" << selectionRect.width()
                 << ", height=" << selectionRect.height();

        emit overlayClosed();
        selectionRect = QRect(); // Auswahl zurücksetzen
        isSelecting = false;     // Auswahlmodus beenden
        update();                // Overlay neu zeichnen
        this->hide();            // Overlay schließen
    }
    QWidget::mouseReleaseEvent(event);
}