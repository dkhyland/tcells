#ifndef HELPERCELL_H
#define HELPERCELL_H

#include "tcell.h"

class HelperCell: public Tcell{
public:
    HelperCell();
    HelperCell(Coordinate *coordinate, unsigned int id,unsigned int rad): Tcell(coordinate,id,rad){
        p_release_rate = 1;
        p_absorb_rate = 0;
        total_p = 0;
        //Pure probability of dividing
        divide_parameter = 0.051;
        death_rate = 0.05;
        p_move = 0.8;
        p_move_prev = p_move;
        just_moved = false;
        type = HCell;
    }

    ~HelperCell() override{}

    virtual void render(QPainter &painter, unsigned int time) override{
        if(just_moved){
            painter.setBrush(QBrush(colours[4]));
        }else{
            painter.setBrush(QBrush(colours[5]));
        }
        //painter.setBrush(QBrush(Qt::black));
        //Draw the cell
        painter.drawEllipse(int(m_coordinate->getQtRenderingXCoordinate() - tcell_rad),
                            int(m_coordinate->getQtRenderingYCoordinate() - tcell_rad),
                            int(tcell_rad * 2),
                            int(tcell_rad * 2));
    }
};

#endif // HELPERCELL_H
