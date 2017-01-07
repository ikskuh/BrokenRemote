#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpServer>
#include <QList>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void executeRemoteCode(QString const & code);

    void log(QString const & message, QString const & category = "INFO");

private slots:
    void on_actionQuit_triggered();

    void on_actionNew_triggered();

    void on_actionClose_triggered();

    void on_actionRun_triggered();

    void on_clientConnected();

    void on_disconnected();

    void on_readReady();

    void on_actionAdd_Half_Red_triggered();

    void on_actionAdd_Red_triggered();

    void on_actionAdd_Half_Soul_triggered();

    void on_actionAdd_Soul_triggered();

    void on_actionAdd_Half_Black_triggered();

    void on_actionAdd_Black_triggered();

    void on_actionAdd_Golden_Heart_triggered();

    void on_actionAdd_Eternal_Heart_triggered();

    void on_actionAdd_Half_Container_triggered();

    void on_actionAdd_Container_triggered();

    void on_actionAdd_Coin_triggered();

    void on_actionAdd_Bomb_triggered();

    void on_actionAdd_Key_triggered();

    void on_actionAdd_Golden_Bomb_triggered();

    void on_actionAdd_Golden_Key_triggered();

    void on_actionFull_Charge_triggered();

    void on_actionIncrease_Charge_triggered();

    void on_actionDischarge_triggered();

    void on_actionAdd_Nickel_triggered();

    void on_actionAdd_Dime_triggered();

    void on_actionReroll_Enemies_triggered();

private:
    Ui::MainWindow *ui;
    QTcpServer * server;
    QList<QTcpSocket*> sockets;
};

#endif // MAINWINDOW_H
