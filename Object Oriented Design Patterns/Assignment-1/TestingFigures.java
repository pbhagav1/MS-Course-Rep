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
import java.util.ArrayList;
import javax.swing.*;

public class TestingFigures extends JPanel{
    /*private ArrayList<Figure> figuresList;*/
    public static void main(String args[]) throws InterruptedException{
        int raceComplete, endX;
        String winnerName = "Dummy";
        ArrayList<Figure> figuresList = new ArrayList<>();
        
        //Add figures to Array List
        figuresList.add(new Square(100,100,50,Color.GRAY,"Square-1"));
        Thread.sleep(500);
        figuresList.add(new Circle(100,200,50,Color.RED, "Circle-1"));
        Thread.sleep(500);
        figuresList.add(new Triangle(100,300,50,Color.BLUE, "Triangle-1"));
        Thread.sleep(500);
        figuresList.add(new Hexagon(100,400,50,Color.GREEN, "Hexagon-1"));
        Thread.sleep(500);

        //JFrame to display the objects
        JFrame frame = new JFrame();
        frame.setTitle("Figure Racing");
        frame.setBackground(Color.BLACK);
        frame.setSize(1000, 600);
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.setResizable(false);
        frame.setVisible(true);
        Graphics g = frame.getGraphics();
        
        endX = frame.getWidth()-100;
        g.drawLine(100, 0, 100, 600);
        g.drawLine(endX, 0, endX, 600);
        //Draw the figures for the first time
        for (Figure fig : figuresList) {
            fig.draw(g);
        }
        frame.setVisible(true);
        
        //Start the race
        raceComplete = 0;
        do{
            Thread.sleep(500);
            g.clearRect( 0, 0, frame.getWidth(), frame.getHeight());
            g.drawLine(100, 0, 100, 600);
            g.drawLine(endX, 0, endX, 600);
            for (Figure fig : figuresList) {
                fig.move();
                if ((raceComplete == 0) && (fig.getX() >= (frame.getWidth() - 100))){
                    raceComplete = 1;
                    winnerName = fig.getName();
                }    
                fig.draw(g);
            }
            frame.setVisible(true);
        }while(raceComplete == 0);
        frame.dispose();
        System.out.println("The winner is : "+ winnerName);
        
        for (Figure fig : figuresList) {
            System.out.println("The Attributes of "+ fig.getName() + " are: ");
            System.out.println("Color    : " + fig.getColor());
            System.out.println("Speed    : " + fig.getSpeed());
        }
    }
}