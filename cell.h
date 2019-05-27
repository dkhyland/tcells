#ifndef CELL_H
#define CELL_H

#pragma once

#include "coordinate.h"
#include "enums.h"
#include <cstdlib>
#include <QPainter>
#include <iostream>

//Provides basic methods cells can use
class Cell{
public:
    Cell(Coordinate *coord, unsigned int id, unsigned int rad): m_coordinate(coord), id(id), tcell_rad(rad){
        p_release_rate = 0;
        p_absorb_rate = 0;
        total_p = 0;
        divide_parameter = 0;
        death_rate = 0;
        p_move = 0;
        p_move_prev = 0;
        just_moved = false;
        type = NoCell;
        is_bound = false;
        p_bind = 0;
        p_unbind = 0;
    }

    virtual ~Cell(){}

    Coordinate *getCoordinate(){
        return m_coordinate;
    }

    //Render method to draw the cell
    virtual void render(QPainter &painter, unsigned int time){
        //painter.setBrush(QBrush(colours[0]));
        painter.setBrush(QBrush(Qt::black));
        //Draw the cell
        painter.drawEllipse(int(m_coordinate->getQtRenderingXCoordinate() - tcell_rad),
                            int(m_coordinate->getQtRenderingYCoordinate() - tcell_rad),
                            int(tcell_rad * 2),
                            int(tcell_rad * 2));
    }

    //Setter methods. Logic is handled outside of the Cell class.

    void setPSignal(double amount){
        total_p = amount;
    }

    //add the given coordinates to the cell's current one
    void addPosition(int y, int x){
        m_coordinate->changeInXCoordinate(x*2*int(tcell_rad));
        m_coordinate->changeInYCoordinate(- y*2*int(tcell_rad));
        //CONFUSING!!!
        m_coordinate->changeRow(y);
        m_coordinate->changeColumn(x);
    }

    //Set radius of the cell
    void setRadius(unsigned int rad){
        tcell_rad = rad;
    }

    void changeJustMoved(bool want){
        if(just_moved != want){
            just_moved = !just_moved;
        }
    }

    void setPMove(double p){
        p_move = p;
    }

    void setPMovePrev(double p){
        p_move_prev = p;
    }

    void setIsBound(bool b){
        is_bound = b;
    }

    //Getter methods
    std::map<std::string,double> getParameters(){
        std::map<std::string,double> param_dict;
        param_dict.emplace("PReleaseRate",p_release_rate);
        param_dict.emplace("PAbsorbRate",p_absorb_rate);
        param_dict.emplace("TotalPSignal",total_p);
        param_dict.emplace("DivideParameter",divide_parameter);
        param_dict.emplace("DeathRate",death_rate);
        param_dict.emplace("ProbabilityMove",p_move);
        param_dict.emplace("ProbabilityMovePrev",p_move_prev);
        param_dict.emplace("JustMoved",just_moved);
        param_dict.emplace("Type",type);
        param_dict.emplace("IsBound",is_bound);
        param_dict.emplace("ProbabilityBind",p_bind);
        param_dict.emplace("ProbabilityUnbind",p_unbind);
        return param_dict;
    }

    unsigned int getRadius(){
        return tcell_rad;
    }

    double getPReleaseRate(){
        return p_release_rate;
    }

    double getPAbsorbRate(){
        return p_absorb_rate;
    }

    double getDivideParam(){
        return divide_parameter;
    }

    double getTotalP(){
        return total_p;
    }

    double getDeathRate(){
        return death_rate;
    }

    double getPMove(){
        return p_move;
    }

    double getPMovePrev(){
        return p_move_prev;
    }

    unsigned int getXCoordinate(){
        return m_coordinate->getQtRenderingXCoordinate();
    }

    unsigned int getYCoordinate(){
        return m_coordinate->getQtRenderingYCoordinate();
    }

    unsigned int getRow(){
        return m_coordinate->getRow();
    }

    unsigned int getColumn(){
        return m_coordinate->getColumn();
    }

    unsigned int getId(){
        return id;
    }

    unsigned int getJustMoved(){
        return just_moved;
    }

    unsigned int getType(){
        return type;
    }

    bool getIsBound(){
        return is_bound;
    }

    double getPBind(){
        return p_bind;
    }

    double getPUnbind(){
        return p_unbind;
    }

protected:
    Coordinate * m_coordinate;
    unsigned int tcell_rad;
    unsigned int id;
    double p_release_rate;
    double p_absorb_rate;
    double total_p;
    double divide_parameter;
    double death_rate;
    double p_move;
    double p_move_prev;
    bool just_moved;
    unsigned int type;
    bool is_bound;
    double p_bind;
    double p_unbind;
    QColor colours[6] = {QColor(255,0,0), QColor(255,127,0), QColor(255,255,0),QColor(0,255,0), QColor(0,0,255), QColor(139,0,255)};
};

#endif // CELL_H
