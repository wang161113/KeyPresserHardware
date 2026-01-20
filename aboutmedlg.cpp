#include "aboutmedlg.h"
#include "ui_aboutmedlg.h"

AboutMeDlg::AboutMeDlg(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AboutMeDlg)
{
    ui->setupUi(this);
}

AboutMeDlg::~AboutMeDlg()
{
    delete ui;
}
