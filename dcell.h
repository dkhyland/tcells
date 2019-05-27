#ifndef DCELL_H
#define DCELL_H

#include "cell.h"

class Dcell: public Cell{
public:
    Dcell(Coordinate * coord,unsigned int id,unsigned int rad): Cell(coord,id,rad){
        //Set the variables
        p_available = 0;
    }

    ~Dcell(){}


    //Render method to draw the cell
    void render(QPainter &painter, unsigned int time){
        painter.setBrush(QBrush(Qt::black));
        //Draw the cell
        painter.drawEllipse(int(m_coordinate->getQtRenderingXCoordinate() - tcell_rad),
                            int(m_coordinate->getQtRenderingYCoordinate() - tcell_rad),
                            int(tcell_rad * 2),
                            int(tcell_rad * 2));
    }

    void setPAvailable(double val){
        p_available = val;
    }

    void addPAvailable(double val){
        p_available += val;
    }

    double getPAvailable(){
        return p_available;
    }

private:
    double p_available;
};

#endif // DCELL_H
