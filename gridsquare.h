#ifndef GRIDSQUARE_H
#define GRIDSQUARE_H

#include "enums.h"
#include "cell.h"
#include <QDebug>

class GridSquare{
public:
    GridSquare(double signal = 0, unsigned int cell = NoCell): signal_value(signal), occupying_cell(cell){}

    ~GridSquare(){}

    //Signal methods
    double getSignalValue(){
        return signal_value;
    }

    void addSignal(double sig){
        signal_value += sig;
    }

    bool isOccupied(){
        return !(occupying_cell == NoCell);
    }

    bool isOccupiedByTCell(){
        if ((occupying_cell == TCell) || (occupying_cell == KCell) || (occupying_cell == HCell)){
            return true;
        }
        return false;
    }

    bool isOccupiedByCell(unsigned int cell){
        return occupying_cell == cell;
    }

    //Cell methods
    unsigned int getCell(){
        return occupying_cell;
    }

    Cell * getCellPtr(){
        return cell_ptr;
    }

    void addCellPtr(Cell * cell){
        cell_ptr = cell;
    }

    void addCell(unsigned int cellType){
        occupying_cell = cellType;
    }



private:
    double signal_value;
    //Makes use of the enum
    unsigned int occupying_cell;
    //Pointer to occupying cell
    Cell * cell_ptr;
};

#endif // GRIDSQUARE_H
