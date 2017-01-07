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

    this->loadPickups(this->ui->actionCard, "Data/cards.csv");
    this->loadPickups(this->ui->actionRune, "Data/runes.csv");
    this->loadPickups(this->ui->actionPill, "Data/pills.csv");
    this->loadPickups(this->ui->actionTrinket, "Data/trinkets.csv");
    this->loadPickups(this->ui->actionItem, "Data/items.csv");

    // TODO: Implement correct MDI interface
    // Open a single window here.
    this->on_actionNew_triggered();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::loadPickups(QAction * action, QString fileName)
{
    action->setEnabled(false);
}

void MainWindow::on_actionQuit_triggered()
{
    QApplication::quit();
}

void MainWindow::on_actionNew_triggered()
{
    ScriptEditor * edit = new ScriptEditor();

    QMdiSubWindow *window = this->ui->mdiArea->addSubWindow(edit);

    window->setWindowState(Qt::WindowMaximized);
    window->setWindowTitle("New Lua script");
    window->show();
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

void MainWindow::executeRemoteCode(QString code)
{
    // Prepare the code for transmittion: Replacing newlines with spaces,
    // because lua is awesome!
    QString prepped = code.replace("\'", "\\'").replace("\n", "\\n");

    QString message = QString("{ type='run', code = '%1' }").arg(prepped);

    this->sendRaw(message);
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

void MainWindow::log(const QString &message, const QString &category)
{
    this->ui->log->appendPlainText("[" + category + "]\t" + message); // Adds the message to the widget
    this->ui->log->verticalScrollBar()->setValue(ui->log->verticalScrollBar()->maximum()); // Scrolls to the bottom
}


void MainWindow::spawn(QString type, QString subtype, QString variant)
{
    QString code(
        "local p = Game():GetPlayer(0)\n"
        "local r = Game():GetRoom()\n"
        "local pos = r:FindFreePickupSpawnPosition(p.Position, 1.0, false)\n"
        "Isaac.Spawn(%1, %2, %3, pos, Vector(0, 0), nil)\n"
        "p:AnimateHappy()"
        );
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
