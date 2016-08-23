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
    private static int diameter;
    private static Figure c;
    
    /*public Circle (){
        super();
        diameter = 50;
    }*/
    
    private Circle (int inpX, int inpY, int inpDiameter, int inpColor, String inpName){
        super(inpX, inpY, inpColor, inpName);
        diameter = inpDiameter;
    }
    
    /*private Circle (){
        super(100, 275, Color.RED, "Circle", new HighSpeedMovingStrategy());
        diameter = 50;
    }*/
    
    public static synchronized Figure getCircle(int inpX, int inpY, int inpDiameter, int inpColor, String inpName){
        if (c == null){
            c = new Circle(inpX, inpY, inpDiameter, inpColor, inpName);
        }else{
            c.setX(inpX);
            c.setY(inpY);
            c.setColor(inpColor);
            c.setName(inpName);
            diameter = inpDiameter;
        }
        return c;
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
    public Object clone() throws CloneNotSupportedException{
        throw new CloneNotSupportedException();
    }
}