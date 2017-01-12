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
#include <QJsonObject>

#include <tuple>



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

void MainWindow::on_actionBasement_I_triggered()
{
    this->changeStage("LevelStage.STAGE1_1", "StageType.STAGETYPE_ORIGINAL");
}

void MainWindow::on_actionBasement_II_triggered()
{
    this->changeStage("LevelStage.STAGE1_2", "StageType.STAGETYPE_ORIGINAL");
}

void MainWindow::on_actionCellar_I_triggered()
{
    this->changeStage("LevelStage.STAGE1_1", "StageType.STAGETYPE_WOTL");
}

void MainWindow::on_actionCellar_II_triggered()
{
    this->changeStage("LevelStage.STAGE1_2", "StageType.STAGETYPE_WOTL");
}

void MainWindow::on_actionBurning_Basement_I_triggered()
{
    this->changeStage("LevelStage.STAGE1_1", "StageType.STAGETYPE_AFTERBIRTH");
}

void MainWindow::on_actionBurning_Basement_II_triggered()
{
    this->changeStage("LevelStage.STAGE1_2", "StageType.STAGETYPE_AFTERBIRTH");
}

void MainWindow::on_actionThe_Void_triggered()
{
    this->changeStage("LevelStage.STAGE7", "StageType.STAGETYPE_ORIGINAL");
}

void MainWindow::on_actionThe_Dark_Room_triggered()
{
    this->changeStage("LevelStage.STAGE6", "StageType.STAGETYPE_ORIGINAL");
}

void MainWindow::on_actionThe_Chest_triggered()
{
    this->changeStage("LevelStage.STAGE6", "StageType.STAGETYPE_WOTL");
}

void MainWindow::on_actionSheol_triggered()
{
    this->changeStage("LevelStage.STAGE5", "StageType.STAGETYPE_ORIGINAL");
}

void MainWindow::on_actionThe_Cathedral_triggered()
{
    this->changeStage("LevelStage.STAGE5", "StageType.STAGETYPE_WOTL");
}

void MainWindow::on_actionThe_Depths_I_triggered()
{
    this->changeStage("LevelStage.STAGE3_1", "StageType.STAGETYPE_ORIGINAL");
}

void MainWindow::on_actionNecropolis_I_triggered()
{
    this->changeStage("LevelStage.STAGE3_1", "StageType.STAGETYPE_WOTL");
}

void MainWindow::on_actionThe_Depths_II_triggered()
{
    this->changeStage("LevelStage.STAGE3_2", "StageType.STAGETYPE_ORIGINAL");
}

void MainWindow::on_actionNecropolis_II_triggered()
{
    this->changeStage("LevelStage.STAGE3_2", "StageType.STAGETYPE_WOTL");
}

void MainWindow::on_actionCaves_I_triggered()
{
    this->changeStage("LevelStage.STAGE2_1", "StageType.STAGETYPE_ORIGINAL");
}

void MainWindow::on_actionCaves_II_triggered()
{
    this->changeStage("LevelStage.STAGE2_2", "StageType.STAGETYPE_ORIGINAL");
}

void MainWindow::on_actionFlooded_Caves_I_triggered()
{
    this->changeStage("LevelStage.STAGE2_1", "StageType.STAGETYPE_AFTERBIRTH");
}

void MainWindow::on_actionFlooded_Caves_II_triggered()
{
    this->changeStage("LevelStage.STAGE2_2", "StageType.STAGETYPE_AFTERBIRTH");
}

void MainWindow::on_actionThe_Womb_I_triggered()
{
    this->changeStage("LevelStage.STAGE4_1", "StageType.STAGETYPE_ORIGINAL");
}

void MainWindow::on_actionScarred_Womb_I_triggered()
{
    this->changeStage("LevelStage.STAGE4_1", "StageType.STAGETYPE_AFTERBIRTH");
}

void MainWindow::on_actionUtero_I_triggered()
{
    this->changeStage("LevelStage.STAGE4_1", "StageType.STAGETYPE_WOTL");
}

void MainWindow::on_actionThe_Womb_II_triggered()
{
    this->changeStage("LevelStage.STAGE4_2", "StageType.STAGETYPE_ORIGINAL");
}

void MainWindow::on_actionScarred_Womb_II_triggered()
{
    this->changeStage("LevelStage.STAGE4_2", "StageType.STAGETYPE_AFTERBIRTH");
}

void MainWindow::on_actionUtero_II_triggered()
{
    this->changeStage("LevelStage.STAGE4_2", "StageType.STAGETYPE_WOTL");
}

void MainWindow::on_actionDank_Depths_I_triggered()
{
    this->changeStage("LevelStage.STAGE3_1", "StageType.STAGETYPE_AFTERBIRTH");
}

void MainWindow::on_actionDank_Depths_II_triggered()
{
    this->changeStage("LevelStage.STAGE3_2", "StageType.STAGETYPE_AFTERBIRTH");
}

void MainWindow::on_actionHush_triggered()
{
    this->changeStage("LevelStage.STAGE4_3", "StageType.STAGETYPE_ORIGINAL");
}

void MainWindow::on_actionCatacombs_I_triggered()
{
    this->changeStage("LevelStage.STAGE2_1", "StageType.STAGETYPE_WOTL");
}

void MainWindow::on_actionCatacombs_II_triggered()
{
    this->changeStage("LevelStage.STAGE2_2", "StageType.STAGETYPE_WOTL");
}

void MainWindow::on_actionGreedBasement_triggered()
{
    this->changeStage("LevelStage.STAGE1_GREED", "StageType.STAGETYPE_ORIGINAL");
}

void MainWindow::on_actionGreedCaves_triggered()
{
    this->changeStage("LevelStage.STAGE2_GREED", "StageType.STAGETYPE_ORIGINAL");
}

void MainWindow::on_actionGreedDepths_triggered()
{
    this->changeStage("LevelStage.STAGE3_GREED", "StageType.STAGETYPE_ORIGINAL");
}

void MainWindow::on_actionGreedSheol_triggered()
{
    this->changeStage("LevelStage.STAGE5_GREED", "StageType.STAGETYPE_ORIGINAL");
}

void MainWindow::on_actionGreedTheShop_triggered()
{
    this->changeStage("LevelStage.STAGE6_GREED", "StageType.STAGETYPE_ORIGINAL");
}

void MainWindow::on_actionGreedUltraGreed_triggered()
{
    this->changeStage("LevelStage.STAGE7_GREED", "StageType.STAGETYPE_ORIGINAL");
}

void MainWindow::on_actionSpecial_Rooms_Greed_triggered()
{
    this->changeStage("LevelStage.STAGE6", "StageType.STAGETYPE_AFTERBIRTH");
}

void MainWindow::on_actionGreedTheWomb_triggered()
{
    this->changeStage("LevelStage.STAGE4_GREED", "StageType.STAGETYPE_ORIGINAL");
}

void MainWindow::on_actionOpen_Secret_Rooms_triggered()
{
    this->executeRemoteCode("Game():GetLevel():SetCanSeeEverything(true)");
}

void MainWindow::on_actionShow_Layout_triggered()
{
    this->executeRemoteCode("Game():GetLevel():ApplyMapEffect()");
}

void MainWindow::on_actionShow_Secret_Rooms_triggered()
{
    this->executeRemoteCode("Game():GetLevel():ApplyBlueMapEffect()");
}

void MainWindow::on_actionShow_Rooms_triggered()
{
    this->executeRemoteCode("Game():GetLevel():ApplyCompassEffect()");
}

void MainWindow::on_actionShow_All_triggered()
{
    this->executeRemoteCode("Game():GetLevel():ShowMap() Game():GetLevel():ApplyBlueMapEffect()");
}

void MainWindow::on_actionTeleportToShop_triggered()
{
    this->moveToRoom("RoomType.ROOM_SHOP");
}

void MainWindow::on_actionTeleportToSecretRoom_triggered()
{
    this->moveToRoom("RoomType.ROOM_SECRET");
}

void MainWindow::on_actionTeleportToTreasureRoom_triggered()
{
    this->moveToRoom("RoomType.ROOM_TREASURE");
}

void MainWindow::on_actionTeleportToBossRoom_triggered()
{
    this->moveToRoom("RoomType.ROOM_BOSS", false);
}

void MainWindow::on_actionTeleportToStartRoom_triggered()
{
    this->moveToRoom("Game():GetLevel():GetStartingRoomIndex()", true);
}

void MainWindow::on_actionBlack_Market_triggered()
{
    this->moveToRoom("RoomType.ROOM_BLACK_MARKET");
}

void MainWindow::on_actionDice_Room_triggered()
{
    this->moveToRoom("RoomType.ROOM_DICE");
}

void MainWindow::on_actionBarren_triggered()
{
    this->moveToRoom("RoomType.ROOM_BARREN");
}

void MainWindow::on_actionIsaacs_Room_triggered()
{
    this->moveToRoom("RoomType.ROOM_ISAACS");
}

void MainWindow::on_actionBoss_Rush_Room_triggered()
{
    this->moveToRoom("RoomType.ROOM_BOSSRUSH");
}

void MainWindow::on_actionDungeon_triggered()
{
    this->moveToRoom("RoomType.ROOM_DUNGEON");
}

void MainWindow::on_actionLibrary_triggered()
{
    this->moveToRoom("RoomType.ROOM_LIBRARY");
}

void MainWindow::on_actionChallange_Room_triggered()
{
    this->moveToRoom("RoomType.ROOM_CHALLENGE");
}

void MainWindow::on_actionError_triggered()
{
    this->moveToRoom("RoomType.ROOM_ERROR");
}

void MainWindow::on_actionCurse_Room_triggered()
{
    this->moveToRoom("RoomType.ROOM_CURSE");
}

void MainWindow::on_actionArcade_triggered()
{
    this->moveToRoom("RoomType.ROOM_ARCADE");
}

void MainWindow::on_actionSuper_Secret_Room_triggered()
{
    this->moveToRoom("RoomType.ROOM_SUPERSECRET");
}

void MainWindow::on_actionAngel_Room_triggered()
{
    this->moveToRoom("RoomType.ROOM_ANGEL");
}

void MainWindow::on_actionDevil_Room_triggered()
{
    this->moveToRoom("RoomType.ROOM_DEVIL");
}

void MainWindow::on_actionSacrifice_Room_triggered()
{
    this->moveToRoom("RoomType.ROOM_SACRIFICE");
}

void MainWindow::on_actionMiniboss_Romm_triggered()
{
    this->moveToRoom("RoomType.ROOM_MINIBOSS");
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
    this->executeRemoteCode("Game():GetPlayer(0):FullCharge()");
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
