#ifndef UPDATEWINDOW_H
#define UPDATEWINDOW_H

#include <QDialog>

namespace Ui {
class UpdateWindow;
}

class UpdateWindow : public QDialog
{
    Q_OBJECT

public:
    explicit UpdateWindow(QWidget *parent = nullptr);
    ~UpdateWindow();

private:
    Ui::UpdateWindow *ui;
};

#endif // UPDATEWINDOW_H
