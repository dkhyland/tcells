#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include <QTimer>
#include <QShortcut>
#include "allcells.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(string test = "", QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void renderCells();
    void updateCells();
    void onPauseClicked();
    void onExitClicked();

protected:
    void paintEvent(QPaintEvent *event);

private:
    Ui::MainWindow *ui;
    QGraphicsScene * scene;
    unsigned int rows;
    unsigned int frame_height;
    AllCells cells;
    unsigned int m_counter;
    bool continue_simulation;
    QTimer *cell_timer;
    QTimer *update_timer;
    QShortcut *pause_shortcut;
    QShortcut *exit_shortcut;
    bool is_paused;
};

#endif // MAINWINDOW_Hs
