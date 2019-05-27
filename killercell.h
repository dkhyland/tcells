#ifndef KILLERCELL_H
#define KILLERCELL_H

#include "tcell.h"

class KillerCell: public Tcell{
public:
    KillerCell();
    KillerCell(Coordinate *coordinate, unsigned int id,unsigned int rad): Tcell(coordinate,id,rad){
        p_release_rate = 0;
        p_absorb_rate = 0.1;
        total_p = 0;
        divide_parameter = 0.1;
        death_rate = 0.01;
        p_move = 0.8;
        p_move_prev = p_move;
        just_moved = false;
        type = KCell;
    }

    ~KillerCell() override{}

    virtual void render(QPainter &painter, unsigned int time) override{
        if(just_moved){
            painter.setBrush(QBrush(colours[0]));
        }else{
            painter.setBrush(QBrush(colours[1]));
        }
        //painter.setBrush(QBrush(Qt::black));
        //Draw the cell
        painter.drawEllipse(int(m_coordinate->getQtRenderingXCoordinate() - tcell_rad),
                            int(m_coordinate->getQtRenderingYCoordinate() - tcell_rad),
                            int(tcell_rad * 2),
                            int(tcell_rad * 2));
    }
};

#endif // KILLERCELL_H
