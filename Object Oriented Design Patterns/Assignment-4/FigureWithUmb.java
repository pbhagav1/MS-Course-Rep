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

public class FigureWithUmb extends FigureDecorator{
    
    public FigureWithUmb (Figure s){
        super(s);
        s.setName(s.getName() + " with Umbrella");
    }
    
    @Override
    public void draw(Graphics g){
        super.draw(g);
        int startX, startY;
        startX = super.getX() - 50;
        startY = super.getY() - 80;
        g.setColor(Color.YELLOW);
        /*g.drawString(super.getName(), startX, startY);*/
        g.drawLine(super.getX(), super.getY(), super.getX(), startY);
        g.fillArc(startX, startY, 100, 100, 0, 180);
    }
}
