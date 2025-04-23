#ifndef SELECTIONOVERLAY_H
#define SELECTIONOVERLAY_H

#include <QWidget>
#include <QMouseEvent>

class SelectionOverlay : public QWidget {
    Q_OBJECT

public:
    explicit SelectionOverlay(QWidget *parent = nullptr);

    QRect getSelectedArea() const;

signals:
    void overlayClosed(); // Signal, um das Schließen des Overlays anzuzeigen
    void selectionChanged(const QRect &selection); // Signal für geänderte Auswahl

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    bool isSelecting;
    QPoint startPoint;
    QRect selectionRect;
};

#endif // SELECTIONOVERLAY_H