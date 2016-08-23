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

public class FigureWithHat extends FigureDecorator {
    
    public FigureWithHat(Figure s){
        super(s);
        s.setName(s.getName() + " with Hat");
    }
    
    @Override
    public void draw(Graphics g){
        super.draw(g);
        int startX, startY;
        startX = super.getX() - 25;
        startY = super.getY() - 25;
        g.setColor(Color.YELLOW);
        g.fillRect(startX, startY, 60, 5);
        startY = startY - 20;
        g.fillArc(startX, startY, 50, 50, 0, 180);
    }
}
