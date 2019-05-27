#ifndef TCELL_H
#define TCELL_H

#include "cell.h"

//TCELL CLASS
class Tcell: public Cell{
public:
    Tcell();
    Tcell(Coordinate *coordinate, unsigned int id, unsigned int rad): Cell(coordinate,id,rad){
        p_release_rate = 1;
        p_absorb_rate = 1;
        total_p = 0;
        divide_parameter = 1;
        death_rate = 0.2;
        p_move = 0.5;
        just_moved = false;
        type = TCell;
        is_bound = false;
        p_bind = 1;
        p_unbind = 0.01;
    }

    ~Tcell() override{}

    //Render method to draw the cell
    virtual void render(QPainter &painter, unsigned int time) override{
        painter.setBrush(QBrush(Qt::gray));
        //Draw the cell
        painter.drawEllipse(int(m_coordinate->getQtRenderingXCoordinate() - tcell_rad),
                            int(m_coordinate->getQtRenderingYCoordinate() - tcell_rad),
                            int(tcell_rad * 2),
                            int(tcell_rad * 2));
    }

};

#endif // TCELL_H
