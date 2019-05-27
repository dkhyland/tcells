#ifndef COORDINATE_H
#define COORDINATE_H

#include <stdlib.h>

class Coordinate
{
public:   
    Coordinate(unsigned int row,
               unsigned int column,
               unsigned int xCoordinate = 0,
               unsigned int yCoordinate = 0,
               unsigned int frameHeight = 0
               ):m_row(row),
                 m_column(column),
                 m_xCoordinate(xCoordinate),
                 m_yCoordinate(yCoordinate),
                 m_frameHeight(frameHeight){}

    unsigned int getQtRenderingXCoordinate(){
        return (m_xCoordinate);
    }

    unsigned int getQtRenderingYCoordinate(){
        return (m_frameHeight - m_yCoordinate);
    }

    unsigned int getActualYCoordinate(){
        return (m_yCoordinate);
    }

    unsigned int getColumn(){
        return m_column;
    }

    unsigned int getRow(){
        return m_row;
    }

    void changeInXCoordinate(int change){
        m_xCoordinate = unsigned(int(m_xCoordinate) + change);
    }

    void changeInYCoordinate(int change){
        m_yCoordinate = unsigned(int(m_yCoordinate)+change);
    }

    void changeRow(int change){
        m_row = unsigned(int(m_row)+change);
    }

    void changeColumn(int change){
        m_column = unsigned(int(m_column)+change);
    }

    void setYCoordinateToZero(int offset){
        m_yCoordinate = 0;
        changeInYCoordinate(offset);
    }

    unsigned int getFrameHeight(){
        return m_frameHeight;
    }

    //Same coordinate on the screen
    bool sameCoordinate(Coordinate other_coordinate){
        if((m_xCoordinate == other_coordinate.getQtRenderingXCoordinate()) &&
           (m_frameHeight - m_yCoordinate == other_coordinate.getQtRenderingYCoordinate())){
            return true;
        }
        return false;
    }

    //Same coordinate on the grid
    bool samePosition(Coordinate other_coordinate){
        if((m_row == other_coordinate.getRow()) &&
           (m_column == other_coordinate.getColumn())){
            return true;
        }
        return false;
    }

private:
    unsigned int m_row;
    unsigned int m_column;
    unsigned int m_xCoordinate;
    unsigned int m_yCoordinate;
    unsigned int m_frameHeight;
};

#endif // COORDINATE_H
