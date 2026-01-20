#ifndef ABOUTMEDLG_H
#define ABOUTMEDLG_H

#include <QDialog>

namespace Ui {
class AboutMeDlg;
}

class AboutMeDlg : public QDialog
{
    Q_OBJECT

public:
    explicit AboutMeDlg(QWidget *parent = nullptr);
    ~AboutMeDlg();

private:
    Ui::AboutMeDlg *ui;
};

#endif // ABOUTMEDLG_H
