/**
 * Course ICP @ FIT VUT Brno, 2016
 * ICP 2016 Project - Othello
 *
 * @author Plaskon Pavol, xplask00@stud.fit.vutbr.cz
 * @author Postolka Matej, xposto02@stud.fit.vutbr.cz
 *
 * @brief GUI main window file.
 * @file mainwindow.cpp
 *
 * Unless otherwise stated, all code is licensed under a
 * GNU General Public License v2.0
 *
 */

#include <memory>

#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QPixmap>
#include <QString>
#include <QtConcurrentRun>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "stonewidget.h"
#include "welcomedialog.h"

#include "exception.h"
#include "table.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->gridLayout->setSpacing(0);

    pixmapBlack = QPixmap(":img/black.png");
    pixmapWhite = QPixmap(":img/white.png");

    playerLock = true;
}

MainWindow::~MainWindow()
{
    cleanGrid();
    delete ui;
}

void MainWindow::cleanGrid()
{
    for(StoneWidget* p : pieces)
        delete p;

    pieces.clear();
}

void MainWindow::initializeGrid()
{
    cleanGrid();

    const Table::Board board = games[currGame].table->getBoard();

    int rows = games[currGame].table->getRowCount();
    int cols = games[currGame].table->getColCount();

    for(int i = 0; i < rows; i++)
    {
        for(int j = 0; j < cols; j++)
        {
            int index = games[currGame].table->getVecIndex(std::make_pair(i, j));
            const Table::Stone stone = board.tableMatrix[index];

            StoneWidget *stoneWidget = new StoneWidget(i, j, this);
            pieces.push_back(stoneWidget);

            stoneWidget->setAlignment(Qt::AlignCenter);

            const char *css = "border-style: solid; border-width: 0 1px 1px 0";

            // Last column, last row
            if(i == rows - 1 && j == cols - 1)
                css = "border-style: none;";
            // Last column not in the last row
            else if(j == cols - 1)
                css = "border-style: solid; border-width: 0 0 1px 0";
            // Last row
            else if(i == rows - 1)
                css = "border-style: solid; border-width: 0 1px 0 0";

            stoneWidget->setStyleSheet(css);

            if(stone == Table::Stone::BLACK)
                stoneWidget->setPixmap(pixmapBlack);
            else if(stone == Table::Stone::WHITE)
                stoneWidget->setPixmap(pixmapWhite);

            ui->gridLayout->addWidget(stoneWidget, i, j);
        }
    }
}

void MainWindow::redrawGrid()
{

    const Table::Board board = games[currGame].table->getBoard();

    int rows = games[currGame].table->getRowCount();
    int cols = games[currGame].table->getColCount();

    for(int i = 0; i < rows; i++)
    {
        for(int j = 0; j < cols; j++)
        {
            int index = games[currGame].table->getVecIndex(std::make_pair(i, j));
            const Table::Stone stone = board.tableMatrix[index];

            StoneWidget *stoneWidget = pieces[i*cols + j];

            if(stone == Table::Stone::BLACK)
            {
                stoneWidget->setPixmap(pixmapBlack);
            }
            else if(stone == Table::Stone::WHITE)
            {
                stoneWidget->setPixmap(pixmapWhite);
            }
            else
            {
                stoneWidget->clear();
            }
        }
    }

    updateOnTurnIndicator();
    updateScore();
}

QString getPathToExamples()
{
    return QDir::cleanPath(qApp->applicationDirPath() + QDir::separator() + "examples");
}

void MainWindow::on_actionSave_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save game"), getPathToExamples());
    saveGame(fileName.toStdString());
}

void MainWindow::on_actionLoad_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Load game"), getPathToExamples());
    loadGame(fileName.toStdString());
    initializeGrid();
    playerLock = false;
}

void MainWindow::on_actionExit_triggered()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Othello", "Quit game?",
                                  QMessageBox::Yes|QMessageBox::No);

    if (reply == QMessageBox::Yes)
    {
        qApp->exit();
    }
}

bool MainWindow::currentGameValid()
{
    return (currGame < getOpenedGamesCount());
}

void MainWindow::aiMove()
{
    playerLock = true;

    auto stone = Table::Stone::WHITE;
    Table::Coords nc;

    try
    {
      nc = games[currGame].PC.nextMove(games[currGame].table, Table::Stone::WHITE);
    }
    catch (const OthelloError &e)
    {
      stone = Table::Stone::FREE;
    }

    putStoneIfPossible(nc, stone);

    playerLock = false;
}

void MainWindow::updateOnTurnIndicator()
{
    if ((games[currGame].table->getMoveCount() & 1) && firstPlayerStone == Table::Stone::BLACK)
    {
        ui->labelOnTurnStone->setPixmap(pixmapWhite);
    }
    else
    {
        ui->labelOnTurnStone->setPixmap(pixmapBlack);
    }
}

void MainWindow::updateScore()
{
    ui->scoreBlack->display(games[currGame].table->getBlackStonesCount());
    ui->scoreWhite->display(games[currGame].table->getWhiteStonesCount());
}

void MainWindow::slotClicked(StoneWidget *w)
{
    Table::Coords c = std::make_pair(w->getRow(), w->getCol());

    if(putStoneIfPossible(c, Table::Stone::BLACK) && !playerLock)
    {
        redrawGrid();

        QFuture<void> future = QtConcurrent::run(this, &MainWindow::aiMove);

        // This will wait for AI to finish but not the current redraw operation
        while(future.isRunning())
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

        redrawGrid();
        w->setAutoFillBackground(false);
    }
}

void MainWindow::slotLeft(StoneWidget *w)
{
    if(!playerLock)
        w->setAutoFillBackground(false);
}

void MainWindow::slotEntered(StoneWidget *w)
{
    if(!playerLock)
    {
        w->setAutoFillBackground(true);
        QPalette palette = w->palette();

        if(games[currGame].table->canPutStone(std::make_pair(w->getRow(), w->getCol()), Table::Stone::BLACK))
            palette.setColor(QPalette::Window, QColor(81, 207, 114, 50));
        else
            palette.setColor(QPalette::Window, QColor(207, 79, 79, 50));

        w->setPalette(palette);
    }
}

void MainWindow::showWelcomeDialog()
{
    WelcomeDialog w(this);

    if(w.exec() == QDialog::Accepted)
    {
        int tableSize = w.getTableSize();
        WelcomeDialog::Algo algo = w.getAlgo();

        switch(algo)
        {
        case WelcomeDialog::Algo::CHIMPANZEE:
            startNewGame(tableSize, true, AIPlayer(AIPlayer::AIPlayerType::Chimpanzee));
        case WelcomeDialog::Algo::MONKEY:
            startNewGame(tableSize, true, AIPlayer(AIPlayer::AIPlayerType::Monkey));
        default:
            startNewGame(tableSize, false);
        }

        initializeGrid();

        playerLock = false;

        return;
    }
}

void MainWindow::on_actionUndo_triggered()
{
    if(currentGameValid())
    {
        undoMove();
        redrawGrid();
    }
}

void MainWindow::on_actionRedo_triggered()
{
    if(currentGameValid())
    {
        redoMove();
        redrawGrid();
    }
}

void MainWindow::on_actionReset_triggered()
{
    if(currentGameValid())
    {
        resetCurrentGame();
        redrawGrid();
    }
}

void MainWindow::on_actionNew_Game_triggered()
{
    showWelcomeDialog();
}
