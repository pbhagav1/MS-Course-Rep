/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 *
 * @author Prasanth_B
 */
import java.awt.Graphics;
import java.awt.Color;

public class Circle extends Figure {
    int diameter;
    
    public Circle (){
        super();
        diameter = 50;
    }
    
    public Circle (int inpX, int inpY, int inpDiameter, Color inpColor, String inpName){
        super(inpX, inpY, inpColor, inpName);
        diameter = inpDiameter;
    }
    
    @Override
    public void draw (Graphics g){
        int startX, startY,radius;
        radius = diameter/2;
        startX = getX() - radius;
        startY = getY() - radius;
        g.setColor(getColor());
        g.fillOval(startX, startY, diameter, diameter);
    }
    
    @Override
    public void move(){
        int newX;
        newX = getX() + getSpeed();
        setX(newX);
    }
}
