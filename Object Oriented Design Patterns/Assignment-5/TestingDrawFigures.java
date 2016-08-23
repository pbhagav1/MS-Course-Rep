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
//import java.io.*;
import javax.swing.*;

public class TestingDrawFigures extends JPanel{
    private static FigureFactory figureFactory;
    public static void main(String args[]) throws InterruptedException{
        int choice, inpX, inpY, inpColor, otherLength;
        figureFactory = FigureFactory.getFigureFactory();
        //choice = 0;
        //inpColor = 0;
        otherLength = 50;
        
        //JFrame to display the objects
        JFrame frame = new JFrame();
        frame.setTitle("Draw Multiple Figures");
        frame.setSize(1300, 700);
        frame.setBackground(Color.BLACK);
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.setResizable(false);
        frame.setVisible(true);
        Graphics g = frame.getGraphics();
        
        for (inpX = 100; inpX < 1300; inpX+=75){
            for (inpY = 100; inpY < 700; inpY+=75){
                Thread.sleep(500);
                //inpColor = (inpColor + 1) % 12;
                //choice = (choice + 1) % 4;
                choice = (Math.abs((int) (Math.random() * 10))) % 4;
                inpColor = (Math.abs((int) (Math.random() * 10))) % 12;
                drawFigure (choice, inpX, inpY, otherLength, inpColor, g);
                 frame.setVisible(true);
            }
        }
    }
    
    public static void drawFigure(int choice, int inpX, int inpY, int otherLength, int inpColor, Graphics g){
        Figure f;
        f = figureFactory.getFigure(choice, inpX, inpY, otherLength, inpColor);
        f.draw(g);
    }
}