#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "register.hpp"
#include "dynamic_hash.hpp"
#include "bptree.hpp"
#include "random_file.hpp"
#include "pagemanager.hpp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_search_btn_clicked()
{

}

void MainWindow::on_insert_btn_clicked()
{
    Register newRegister(
                ui->insert_dni->text().toInt(),
                ui->insert_nombres->text().toStdString(),
                ui->insert_apellidos->text().toStdString(),
                ui->insert_edad->text().toInt(),
                ui->insert_fecha->text().toStdString()
                );

    if (ui->combo_strat->currentText() == "Dynamic Hash") {
       DinHash<Register> dhash = new DinHash<Register>(20, "datos");
       dhash.insert(newRegister.id, newRegister);
    }

    if(ui->combo_strat->currentText() == "B+ Tree") {
        auto pm_ptr = std::make_shared<pagemanager>("btree_data_1.bin");
        btree<Register> bptree = new btree<Register>(pm_ptr);
        bptree.insert(newRegister);
    }

    if(ui->combo_strat->currentText() == "Random File") {
        RandomFile<Register> rf = new RandomFile<Register>("datos", "index");
        rf.add(newRegister);
    }
}
