/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 *
 * @author Prasanth_B
 */

import java.awt.Color;
import java.awt.Graphics;

public abstract class FigureDecorator extends Figure{
    protected final Figure decoratedFigure;
    
    public FigureDecorator(Figure s){
        this.decoratedFigure = s;
    }
    
    @Override
    public void setX(int inpX){
        decoratedFigure.setX(inpX);
    }
    
    @Override
    public int getX(){
        return decoratedFigure.getX();
    }
    
    @Override
    public void setY(int inpY){
        decoratedFigure.setY(inpY);
    }
    
    @Override
    public int getY(){
        return decoratedFigure.getY();
    }
    
    @Override
    public void setSpeed(int inpSpeed){
        decoratedFigure.setSpeed(inpSpeed);
    }
    
    @Override
    public int getSpeed(){
        return decoratedFigure.getSpeed();
    }
    
    @Override
    public void setColor(Color inpColor){
        decoratedFigure.setColor(inpColor);
    }
    
    @Override
    public Color getColor(){
        return decoratedFigure.getColor();
    }
    
    @Override
    public void setName(String inpName){
        decoratedFigure.setName(inpName);
    }
    
    @Override
    public String getName(){
        return decoratedFigure.getName();
    }
    
    @Override
    public void setMovingStrategy(MovingStrategy strategy){
        decoratedFigure.setMovingStrategy(strategy);
    }
    
    @Override
    public MovingStrategy getMovingStrategy(){
        return decoratedFigure.getMovingStrategy();
    }
    
    @Override
    public void draw (Graphics g){
        decoratedFigure.draw(g);
    }

    @Override
    public void move(){
        decoratedFigure.move();
    }
}