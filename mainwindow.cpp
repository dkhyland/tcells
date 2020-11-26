#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(string test, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    rows(100),
    frame_height(1000),
    cells(rows,frame_height,"simulation_data"+ test + ".txt"),
    m_counter(0),
    continue_simulation(true),
    is_paused(false)
{
    ui->setupUi(this);
    this->resize(int(frame_height),int(frame_height+30));
    // Add/remove d cells when switching signal model
    cells.initialise(50,50,0);

    //Timer for updating positions
    update_timer = new QTimer(this);
    connect(update_timer,SIGNAL(timeout()),this,SLOT(updateCells()));
    update_timer->start(10);

    //Timer for rendering
    cell_timer = new QTimer(this);
    connect(cell_timer,SIGNAL(timeout()),this,SLOT(renderCells()));
    cell_timer->start(10);

    //Pause shortcut
    pause_shortcut = new QShortcut(QKeySequence(" "), this);
    connect(pause_shortcut, SIGNAL(activated()), this, SLOT(onPauseClicked()));

    //Exit shortcut
    exit_shortcut = new QShortcut(QKeySequence("e"), this);
    connect(exit_shortcut, SIGNAL(activated()), this, SLOT(onExitClicked()));
}

void MainWindow::paintEvent(QPaintEvent *event){
    QPainter painter(this);
    painter.setPen(Qt::white);
//    painter.setBrush(QBrush(Qt::black));
    cells.renderAll(painter,m_counter);
    m_counter++;
}

void MainWindow::onPauseClicked(){
    if(!is_paused){
        cell_timer->stop();
        update_timer->stop();
        is_paused = true;
    }else{
        cell_timer->start();
        update_timer->start();
        is_paused = false;
    }
}

void MainWindow::onExitClicked(){
    onPauseClicked();
    cells.saveGame();
    close();
}

void MainWindow::renderCells(){
    update();
}

void MainWindow::updateCells(){
    continue_simulation = cells.updateAll();
    if(!continue_simulation){
        qDebug() << "Have a good day!";
        onExitClicked();
    }
}

MainWindow::~MainWindow(){
    delete ui;
}
