#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QWidget window;
    QVBoxLayout *layout = new QVBoxLayout;

    QLabel *boxLabel = new QLabel("Box:");
    QLineEdit *boxInput = new QLineEdit;

    layout->addWidget(boxLabel);
    layout->addWidget(boxInput);

    window.setLayout(layout);
    window.show();

    return app.exec();
}