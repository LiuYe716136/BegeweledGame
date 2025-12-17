#include "GameWidget.h"
#include "ui_widget.h"

GameWidget::GameWidget(QWidget *parent) : QWidget(parent), ui(new Ui::Widget) {
  ui->setupUi(this);
}

GameWidget::~GameWidget() { delete ui; }
