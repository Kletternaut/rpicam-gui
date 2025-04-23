#ifndef CUSTOMLINEEDIT_H
#define CUSTOMLINEEDIT_H

#include <QLineEdit>
#include <QMouseEvent>

class CustomLineEdit : public QLineEdit {
    Q_OBJECT

public:
    explicit CustomLineEdit(QWidget *parent = nullptr) : QLineEdit(parent) {}

signals:
    void doubleClicked(); // Signal für Doppelklick

protected:
    void mouseDoubleClickEvent(QMouseEvent *event) override {
        if (event->button() == Qt::LeftButton) {
            emit doubleClicked(); // Signal auslösen
        }
        QLineEdit::mouseDoubleClickEvent(event); // Standardverhalten beibehalten
    }
};

#endif // CUSTOMLINEEDIT_H