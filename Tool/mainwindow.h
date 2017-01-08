#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpServer>
#include <QList>

namespace Ui {
    class MainWindow;
}

class QMdiSubWindow;

class ScriptEditor;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void executeRemoteCode(QString code);

    void log(QString message, QString category = "INFO");

private:
    void sendRaw(QString message);

    void spawn(QString type, QString subtype, QString variant, bool spawnPickup = true);

    void loadPickups(QMenu * menu, QString fileName, QString pickupVariant);

    ScriptEditor * openFile(QString fileName);

    void renderWindowsMenu();

    void updateActions();

    void loadTemplates();

    void loadEnemies();

    void addTemplate(QString name, QString contents);

    void on_createTemplateClick();

    void on_pickupClick();

    void on_spawnEnemyClick();

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

    void on_actionBattery_triggered();

    void on_actionSave_triggered();

    void on_actionSave_As_triggered();

    void on_actionOpen_triggered();

    void on_mdiArea_subWindowActivated(QMdiSubWindow *arg1);

    void on_actionTry_open_devil_deal_triggered();

    void on_actionTry_open_boss_rush_door_triggered();

    void on_actionTry_open_mega_satan_triggered();

    void on_actionOpen_The_Void_triggered();

    void on_actionOpen_Blue_Womb_triggered();

    void on_actionRestock_Shop_Partial_triggered();

    void on_actionRestock_Shop_Full_triggered();

    void on_actionShuffle_Shop_triggered();

    void on_actionRandom_Teleport_triggered();

    void on_actionRandom_Teleport_No_Error_Room_triggered();

    void on_actionReroll_Floor_Pickups_triggered();

    void on_actionReroll_Floor_Collectibles_triggered();

    void on_actionPrint_code_in_system_log_triggered(bool checked);

    void on_actionAbout_triggered();

    void on_actionTile_triggered();

private:
    Ui::MainWindow *ui;
    QTcpServer * server;
    QList<QTcpSocket*> sockets;
};

#endif // MAINWINDOW_H
