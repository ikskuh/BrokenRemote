#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "scripteditor.h"

#include <QApplication>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QPlainTextEdit>
#include <QMessageBox>
#include <QScrollBar>
#include <QTcpSocket>
#include <QFileDialog>
#include <QMenu>

#include <tuple>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->log("System ready", "SYSTEM");

    this->server = new QTcpServer(this);

    if(this->server->listen(QHostAddress::Any, 12000))
    {
        this->log("Server listening on port 12000", "NETWORK");
        connect(this->server, &QTcpServer::newConnection, this, &MainWindow::on_clientConnected);
    }
    else
    {
        this->log("Server cannot listen on port 12000, please check if another instance is still running.", "NETWORK");
    }
    qDebug() << QApplication::applicationDirPath();

    ui->menuCreate->clear();
    ui->menuCreate->setEnabled(false);

    this->loadPickups(this->ui->menuCard, "Data/cards.csv", "PickupVariant.PICKUP_TAROTCARD");
    this->loadPickups(this->ui->menuRune, "Data/runes.csv", "PickupVariant.PICKUP_TAROTCARD");
    this->loadPickups(this->ui->menuPill, "Data/pills.csv", "PickupVariant.PICKUP_PILL");
    this->loadPickups(this->ui->menuTrinket, "Data/trinkets.csv", "PickupVariant.PICKUP_TRINKET");
    this->loadPickups(this->ui->menuItem, "Data/items.csv", "PickupVariant.PICKUP_COLLECTIBLE");

    this->loadTemplates();

    this->loadEnemies();

    this->ui->mdiArea->setViewMode(QMdiArea::TabbedView);

    this->setCentralWidget(this->ui->mdiArea);

    this->renderWindowsMenu();

    this->updateActions();
}

MainWindow::~MainWindow()
{
    delete ui;
    this->ui = nullptr;
}

void MainWindow::renderWindowsMenu()
{
    /*
    QList<QMdiSubWindow *> windows = this->ui->mdiArea->subWindowList();
    ui->sepWindowList->setVisible(!windows.isEmpty());

    QList<QAction*> actions = ui->menuWindows->actions();

    int idx = actions.indexOf(ui->sepWindowList);
    */
    /*
    for (int i = 0; i < windows.size(); ++i) {
        QMdiSubWindow *mdiSubWindow = windows.at(i);
        MdiChild *child = qobject_cast<MdiChild *>(mdiSubWindow->widget());

        QString text;
        if (i < 9) {
            text = tr("&%1 %2").arg(i + 1)
                               .arg(child->userFriendlyCurrentFile());
        } else {
            text = tr("%1 %2").arg(i + 1)
                              .arg(child->userFriendlyCurrentFile());
        }
        QAction *action = windowMenu->addAction(text, mdiSubWindow, ActiveMdiSubWindowFunctor(mdiArea, mdiSubWindow));
        action->setCheckable(true);
        action ->setChecked(child == activeMdiChild());
    }
    */
}

void MainWindow::loadPickups(QMenu * menu, QString fileName, QString pickupVariant)
{
    menu->actions().clear();

    QFile file(fileName);
    if(file.open(QFile::ReadOnly) == false) {
        this->log("Failed to open " + fileName, "PICKUPS");
        menu->setEnabled(false);
        return;
    }
    while(file.atEnd() == false)
    {
        QString line = QString::fromUtf8(file.readLine()).trimmed();
        QStringList options = line.split('\t');
        if(options.length() != 2)
            continue;

        QStringList info;
        info << pickupVariant;
        info << options[0];

        QAction *action = menu->addAction(options[1]);
        action->setData(QVariant::fromValue(info));

        connect(action, &QAction::triggered, this, &MainWindow::on_pickupClick);
    }
    file.close();

    menu->setEnabled(menu->actions().count() > 0);
}

void MainWindow::loadTemplates()
{
    QDir dir("./Templates/", "*.lua");
    for(QFileInfo fileInfo : dir.entryInfoList(QDir::Files, QDir::Name))
    {
        QFile file(fileInfo.absoluteFilePath());
        if(file.open(QFile::ReadOnly)) {
            this->addTemplate(
                fileInfo.baseName(),
                QString::fromUtf8(file.readAll()));
        }
    }
}

void MainWindow::loadEnemies()
{
    this->ui->menuSpawn_Enemy->actions().clear();

    QFile file("Data/enemies.csv");
    if(file.open(QFile::ReadOnly) == false) {
        this->log("Failed to open Data/enemies.csv", "ENEMIES");
        this->ui->menuSpawn_Enemy->setEnabled(false);
        return;
    }
    while(file.atEnd() == false)
    {
        QString line = QString::fromUtf8(file.readLine()).trimmed();
        QStringList options = line.split('\t');
        if(options.length() != 4)
            continue;

        QAction *action = this->ui->menuSpawn_Enemy->addAction(options[0]);
        action->setData(QVariant::fromValue(options));

        connect(action, &QAction::triggered, this, &MainWindow::on_spawnEnemyClick);
    }
    file.close();

    this->ui->menuSpawn_Enemy->setEnabled(this->ui->menuSpawn_Enemy->actions().count() > 0);
}

void MainWindow::on_spawnEnemyClick()
{
    QAction * action = (QAction*)sender();
    QStringList data = action->data().toStringList();
    this->spawn(data[1], data[2], data[3], false);
}

void MainWindow::addTemplate(QString name, QString contents)
{
    QAction * action = this->ui->menuCreate->addAction(name);
    action->setData(QVariant::fromValue(contents));
    connect(action, &QAction::triggered, this, &MainWindow::on_createTemplateClick);

    ui->menuCreate->setEnabled(true);
}

ScriptEditor * MainWindow::openFile(QString fileName)
{
    ScriptEditor * edit = new ScriptEditor(fileName);

    QMdiSubWindow *window = this->ui->mdiArea->addSubWindow(edit);
    // window->setWindowState(Qt::WindowMaximized);
    window->show();

    connect(window, &QMdiSubWindow::destroyed, this, &MainWindow::renderWindowsMenu);

    this->renderWindowsMenu();

    return edit;
}

void MainWindow::on_createTemplateClick()
{
    QAction * action = (QAction*)sender();
    QString data = action->data().toString();

    ScriptEditor *edit = this->openFile(QString());

    edit->setPlainText(data);
}

void MainWindow::on_pickupClick()
{
    QAction * action = (QAction*)sender();
    QStringList data = action->data().toStringList();

    this->spawn("EntityType.ENTITY_PICKUP", data[0], data[1]);
}

void MainWindow::on_actionQuit_triggered()
{
    QApplication::quit();
}

void MainWindow::on_actionNew_triggered()
{
    this->openFile(QString());
}

void MainWindow::on_actionClose_triggered()
{
    QMdiSubWindow * window = this->ui->mdiArea->activeSubWindow();
    if(window == nullptr) {
        return;
    }
    window->close();
}

void MainWindow::on_actionRun_triggered()
{
    QMdiSubWindow * window = this->ui->mdiArea->activeSubWindow();
    if(window == nullptr) {
        return;
    }
    ScriptEditor * editor = (ScriptEditor*) window->widget();
    QString code = editor->document()->toPlainText();

    this->executeRemoteCode(code);
}

void MainWindow::on_actionSave_triggered()
{
    QMdiSubWindow * window = this->ui->mdiArea->activeSubWindow();
    if(window == nullptr) {
        return;
    }
    ScriptEditor * editor = (ScriptEditor*) window->widget();
    editor->save();
}

void MainWindow::on_actionSave_As_triggered()
{
    QMdiSubWindow * window = this->ui->mdiArea->activeSubWindow();
    if(window == nullptr) {
        return;
    }
    ScriptEditor * editor = (ScriptEditor*) window->widget();
    editor->saveAs();
}

void MainWindow::on_actionOpen_triggered()
{
    QString file = QFileDialog::getOpenFileName(
        this,
        "Open file...",
        QString(),
        "Lua Files (*.lua)");
    if(file.isNull()) {
        return;
    }
    this->openFile(file);
}

void MainWindow::executeRemoteCode(QString code)
{
    // Prepare the code for transmittion: Replacing newlines with spaces,
    // because lua is awesome!
    QString prepped(code);
    prepped.replace("\'", "\\'").replace("\n", "\\n");

    QString message = QString("{ type='run', code = '%1' }").arg(prepped);

    this->sendRaw(message);

    if(this->ui->actionPrint_code_in_system_log->isChecked()) {
        this->log(code, "CODE");
    }
}

void MainWindow::sendRaw(QString message)
{
    QByteArray data = message
        .append('\n')
        .toUtf8();

    qDebug() << "Send" << message;
    for(QTcpSocket * sock : this->sockets)
    {
        sock->write(data);
    }
}

void MainWindow::log(QString message, QString category)
{
    message.replace("\n", "\n\t").trimmed();
    if(this->ui != nullptr) {
        this->ui->log->appendPlainText("[" + category + "]\t" + message); // Adds the message to the widget
        this->ui->log->verticalScrollBar()->setValue(ui->log->verticalScrollBar()->maximum()); // Scrolls to the bottom
    }
}


void MainWindow::spawn(QString type, QString subtype, QString variant, bool spawnPickup)
{
    QString code;
    if(spawnPickup)
    {
        code = QString(
            "local p = Game():GetPlayer(0)\n"
            "local r = Game():GetRoom()\n"
            "local pos = r:FindFreePickupSpawnPosition(p.Position, 1.0, false)\n"
            "Isaac.Spawn(%1, %2, %3, pos, Vector(0, 0), nil)\n"
            "p:AnimateHappy()"
            );
    } else {
        code = QString(
            "local p = Game():GetPlayer(0)\n"
            "local r = Game():GetRoom()\n"
            "local pos = r:FindFreeTilePosition(r:GetCenterPos(), 3.0)\n"
            "Isaac.Spawn(%1,%2,%3,pos,Vector(0,0),nil)"
            );
    }
    this->executeRemoteCode(code.arg(type, subtype, variant));
}

void MainWindow::on_clientConnected()
{
    QTcpSocket * sock = this->server->nextPendingConnection();
    connect(sock, &QTcpSocket::disconnected, this, &MainWindow::on_disconnected);

    this->log(
        "Client connected from " +
        sock->peerAddress().toString() +
        ":" +
        QString::number(sock->peerPort()),
        "NETWORK");

    this->sockets.append(sock);

    connect(sock, &QTcpSocket::readyRead, this, &MainWindow::on_readReady);
}

void MainWindow::on_disconnected()
{
    QTcpSocket * sock = (QTcpSocket*)sender();
    this->sockets.removeAll(sock);

    this->log(
        "Client from " +
        sock->peerAddress().toString() +
        ":" +
        QString::number(sock->peerPort()) +
        " disconnected.",
        "NETWORK");

    sock->deleteLater();
}

void MainWindow::on_readReady()
{
    QTcpSocket * sock = (QTcpSocket*)sender();

    while(sock->canReadLine())
    {
        QByteArray linebuf = sock->readLine();
        QString line = QString::fromUtf8(linebuf).trimmed();

        this->log(line, "MESSAGE");
    }
}

void MainWindow::on_actionAdd_Half_Red_triggered()
{
    this->executeRemoteCode("Game():GetPlayer(0):AddHearts(1)");
}

void MainWindow::on_actionAdd_Red_triggered()
{
    this->executeRemoteCode("Game():GetPlayer(0):AddHearts(2)");
}

void MainWindow::on_actionAdd_Half_Soul_triggered()
{
    this->executeRemoteCode("Game():GetPlayer(0):AddSoulHearts(1)");
}

void MainWindow::on_actionAdd_Soul_triggered()
{
    this->executeRemoteCode("Game():GetPlayer(0):AddSoulHearts(2)");
}

void MainWindow::on_actionAdd_Half_Black_triggered()
{
    this->executeRemoteCode("Game():GetPlayer(0):AddBlackHearts(1)");
}

void MainWindow::on_actionAdd_Black_triggered()
{
    this->executeRemoteCode("Game():GetPlayer(0):AddBlackHearts(2)");
}

void MainWindow::on_actionAdd_Golden_Heart_triggered()
{
    this->executeRemoteCode("Game():GetPlayer(0):AddGoldenHearts(1)");
}

void MainWindow::on_actionAdd_Eternal_Heart_triggered()
{
    this->executeRemoteCode("Game():GetPlayer(0):AddEternalHearts(1)");
}

void MainWindow::on_actionAdd_Half_Container_triggered()
{
    this->executeRemoteCode("Game():GetPlayer(0):AddMaxHearts(1, true)");
}

void MainWindow::on_actionAdd_Container_triggered()
{
    this->executeRemoteCode("Game():GetPlayer(0):AddMaxHearts(2, true)");
}

void MainWindow::on_actionAdd_Coin_triggered()
{
    this->executeRemoteCode("Game():GetPlayer(0):AddCoins(1)");
}

void MainWindow::on_actionAdd_Bomb_triggered()
{
    this->executeRemoteCode("Game():GetPlayer(0):AddBombs(1)");
}

void MainWindow::on_actionAdd_Key_triggered()
{
    this->executeRemoteCode("Game():GetPlayer(0):AddKeys(1)");
}

void MainWindow::on_actionAdd_Golden_Bomb_triggered()
{
    this->executeRemoteCode("Game():GetPlayer(0):AddGoldenBomb()");
}

void MainWindow::on_actionAdd_Golden_Key_triggered()
{
    this->executeRemoteCode("Game():GetPlayer(0):AddGoldenKey()");
}

void MainWindow::on_actionFull_Charge_triggered()
{
    this->executeRemoteCode("Game():GetPlayer(0):SetActiveCharge(1000)");
}

void MainWindow::on_actionIncrease_Charge_triggered()
{
    this->executeRemoteCode(
        "local p = Game():GetPlayer(0)\n"
        "local c = p:GetActiveCharge()\n"
        "p:SetActiveCharge(c + 1)");
}

void MainWindow::on_actionDischarge_triggered()
{
    this->executeRemoteCode("Game():GetPlayer(0):SetActiveCharge(0)");
}

void MainWindow::on_actionAdd_Nickel_triggered()
{
    this->executeRemoteCode("Game():GetPlayer(0):AddCoins(5)");
}

void MainWindow::on_actionAdd_Dime_triggered()
{
    this->executeRemoteCode("Game():GetPlayer(0):AddCoins(10)");
}

void MainWindow::on_actionReroll_Enemies_triggered()
{
    this->executeRemoteCode("for i,v in ipairs(Isaac.GetRoomEntities()) do local npc = v:ToNPC() if npc then Game():RerollEnemy(v) end end");
}

void MainWindow::on_actionBattery_triggered()
{
    this->spawn("EntityType.ENTITY_PICKUP", "PickupVariant.PICKUP_LIL_BATTERY", "0");
}

void MainWindow::on_mdiArea_subWindowActivated(QMdiSubWindow *arg1)
{
    this->updateActions();
}

void MainWindow::updateActions()
{
    bool hasWindow = (this->ui->mdiArea->activeSubWindow() != nullptr);
    bool hasClients = (this->sockets.count() > 0);

    this->ui->actionSave->setEnabled(hasWindow);
    this->ui->actionSave_As->setEnabled(hasWindow);
    this->ui->actionClose->setEnabled(hasWindow);
}

void MainWindow::on_actionTry_open_devil_deal_triggered()
{
    this->executeRemoteCode("Game():GetRoom():TrySpawnDevilRoomDoor(true)");
}

void MainWindow::on_actionTry_open_boss_rush_door_triggered()
{
    this->executeRemoteCode("Game():GetRoom():TrySpawnBossRushDoor(true)");
}

void MainWindow::on_actionTry_open_mega_satan_triggered()
{
    // TODO: Does this really open the door? Maybe it's
    // required to call more code...
    this->executeRemoteCode("Game():GetRoom():TrySpawnMegaSatanRoomDoor()");
}

void MainWindow::on_actionOpen_The_Void_triggered()
{
    this->executeRemoteCode("Game():GetRoom():TrySpawnTheVoidDoor()");
}

void MainWindow::on_actionOpen_Blue_Womb_triggered()
{
    this->executeRemoteCode("Game():GetRoom():TrySpawnBlueWombDoor (true, true)");
}

void MainWindow::on_actionRestock_Shop_Partial_triggered()
{
    this->executeRemoteCode("Game():GetRoom()ShopRestockPartial()");
}

void MainWindow::on_actionRestock_Shop_Full_triggered()
{
    this->executeRemoteCode("Game():GetRoom():ShopRestockFull()");
}

void MainWindow::on_actionShuffle_Shop_triggered()
{
    this->executeRemoteCode("Game():GetRoom():ShopReshuffle(false, true)");
}

void MainWindow::on_actionRandom_Teleport_triggered()
{
    this->executeRemoteCode("Game():MoveToRandomRoom(true)");
}

void MainWindow::on_actionRandom_Teleport_No_Error_Room_triggered()
{
    this->executeRemoteCode("Game():MoveToRandomRoom(false)");
}

void MainWindow::on_actionReroll_Floor_Pickups_triggered()
{
    this->executeRemoteCode("Game():RerollLevelPickups(0)");
}

void MainWindow::on_actionReroll_Floor_Collectibles_triggered()
{
    this->executeRemoteCode("Game():RerollLevelCollectibles()");
}

void MainWindow::on_actionPrint_code_in_system_log_triggered(bool checked)
{

}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(
        this,
        this->windowTitle(),
        "The BrokenRemote tool \n"
        "Some new line\n"
        "Bla Blipp");
}

void MainWindow::on_actionTile_triggered()
{
    this->ui->mdiArea->tileSubWindows();
}
