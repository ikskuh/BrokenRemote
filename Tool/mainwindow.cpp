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
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

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

    connect(ui->menuTeleport_to, &QMenu::aboutToShow, this, &MainWindow::on_teleportToAboutToShow);
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
        if(line == "--") {
            menu->addSeparator();
            continue;
        }
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
        if(line == "--") {
            this->ui->menuSpawn_Enemy->addSeparator();
            continue;
        }
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
    prepped
        .replace("\\", "\\\\")
        .replace("\'", "\\'")
        .replace("\n", "\\n");

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

void MainWindow::moveToRoom(QString roomType, bool roomTypeIsActuallyAnIndex)
{
    QString id;
    if(roomTypeIsActuallyAnIndex) {
        id = roomType;
    } else {
        id = QString("l:QueryRoomTypeIndex (%1, true, RNG())").arg(roomType);
    }
    QString code(
        "local l = Game():GetLevel()\n"
        "local idx = %1\n"
        "if idx ~= -1 then while l:GetCurrentRoomIndex() ~= idx do\n"
            "l:ChangeRoom(idx)\n"
        "end else Game():GetPlayer(0):AnimateSad() end"
        );
    this->executeRemoteCode(code.arg(id));
}

void MainWindow::changeStage(QString stage, QString variant)
{
    QString script(
        "Game():GetLevel():SetStage(%1, %2)\n"
        "Game():StartStageTransition(true, 1)");
    this->executeRemoteCode(script.arg(stage, variant));
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

        QJsonDocument doc = QJsonDocument::fromJson(linebuf);

        QJsonObject obj = doc.object();
        qDebug() << obj;

        QString type = obj["type"].toString();

        if(type == "msg") {
            this->log(obj["msg"].toString(), "MESSAGE");
        }
        else if(type == "err") {
            this->log(obj["msg"].toString(), "ERROR");
        }
        else if(type == "roomlist") {
            this->setTeleportListStateFromJson(obj);
        }
        else if(type == "active-charge") {
            this->setActiveItemChargeOptionsFromJson(obj);
        }
        else {
            this->log("Received unsupported message", "NETWORK");
        }
    }
}

void MainWindow::updateActions()
{
    bool hasWindow = (this->ui->mdiArea->activeSubWindow() != nullptr);
    bool hasClients = (this->sockets.count() > 0);

    this->ui->actionSave->setEnabled(hasWindow);
    this->ui->actionSave_As->setEnabled(hasWindow);
    this->ui->actionClose->setEnabled(hasWindow);

    this->ui->actionRun->setEnabled(hasWindow);
}

void MainWindow::on_teleportToAboutToShow()
{
    this->sendRaw("{ type='roomlist' }");
}

void MainWindow::setTeleportListStateFromJson(QJsonObject & obj)
{
    QJsonArray array = obj["rooms"].toArray();

    const int roomCount = 21;
    QAction * alist[roomCount] =
    {
        ui->actionTeleportToStartRoom,
        ui->actionTeleportToShop,
        ui->actionTeleportToBossRoom,
        ui->actionTeleportToTreasureRoom,
        ui->actionTeleportToSecretRoom,
        ui->actionAngel_Room,
        ui->actionDevil_Room,
        ui->actionSuper_Secret_Room,
        ui->actionArcade,
        ui->actionSacrifice_Room,
        ui->actionCurse_Room,
        ui->actionChallenge_Room,
        ui->actionLibrary,
        ui->actionDungeon,
        ui->actionIsaacs_Room,
        ui->actionBarren,
        ui->actionDice_Room,
        ui->actionError,
        ui->actionBlack_Market,
        ui->actionBoss_Rush_Room,
        ui->actionMiniboss_Romm
    };

    for(int i = 0; i < qMin(array.size(), roomCount); i++)
    {
        alist[i]->setEnabled(array[i].toBool());
    }
    for(int i = array.size(); i < roomCount; i++)
    {
        alist[i]->setEnabled(false);
    }
}

void MainWindow::setActiveItemChargeOptionsFromJson(QJsonObject & obj)
{
    bool hasActiveItem = obj["hasActiveItem"].toBool(false);
    int currentCharge = obj["currentCharge"].toInt(0);
    bool needsCharge = obj["needsCharge"].toBool(true);

    if(hasActiveItem) {
        this->ui->actionFull_Charge->setEnabled(needsCharge);
        this->ui->actionIncrease_Charge->setEnabled(needsCharge);
        this->ui->actionDischarge->setEnabled(currentCharge > 0);
    } else {
        this->ui->actionFull_Charge->setEnabled(false);
        this->ui->actionIncrease_Charge->setEnabled(false);
        this->ui->actionDischarge->setEnabled(false);
    }
}
