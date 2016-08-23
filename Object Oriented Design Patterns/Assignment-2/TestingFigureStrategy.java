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

public class TestingFigureStrategy {
    public static void main(String args[]) throws InterruptedException{
        int raceComplete, strategyIndicator, averageSpeed, midPointX, endX, loop;
        String winnerName = "Dummy";
        ArrayList<Figure> figuresList = new ArrayList<>();
        MovingStrategy m1 = new LowSpeedMovingStrategy();
        MovingStrategy m2 = new HighSpeedMovingStrategy();
        loop = 0;
        //Add figures to Array List
        figuresList.add(new FigureWithHat(new Square(100,100,50,Color.GRAY,"Square with Hat")));
        Thread.sleep(500);
        figuresList.add(new FigureWithUmb(new Circle(100,275,50,Color.RED, "Circle with Umbrella")));
        Thread.sleep(500);
        figuresList.add(new FigureWithHat(new Triangle(100,450,50,Color.BLUE, "Triangle with Hat")));
        Thread.sleep(500);
        figuresList.add(new FigureWithUmb(new Hexagon(100,625,50,Color.GREEN, "Hexagon with Umbrella")));
        Thread.sleep(500);

        //JFrame to display the objects
        JFrame frame = new JFrame();
        frame.setTitle("Testing Figures Strategy");
        frame.setBackground(Color.BLACK);
        frame.setSize(1200, 700);
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.setResizable(false);
        frame.setVisible(true);
        Graphics g = frame.getGraphics();
        
        endX = frame.getWidth()-100;
        midPointX = ((100 + endX) / 2);
        g.setColor(Color.RED);
        g.drawLine(100, 0, 100, frame.getHeight());
        g.drawLine(endX, 0, endX, frame.getHeight());

        //Draw the figures for the first time
        for (Figure fig : figuresList) {
            fig.draw(g);
            g.setColor(Color.WHITE);
            g.drawLine(0, fig.getY() + 70, frame.getWidth(), fig.getY() + 70);
        }
        frame.setVisible(true);
        //Start the race
        raceComplete = 0;
        // Indicates the strategy to be implemented
        // 1 - Low Speed Moving Strategy
        // 2 - High Speed Moving Strategy
        strategyIndicator = 2;
        do{
            loop++;
            System.out.println("Loop : " + loop);
            Thread.sleep(500);
            
            averageSpeed = 0;
            for(Figure fig : figuresList){
                System.out.println(fig.getName() + "'s Speed is " + fig.getSpeed());
                averageSpeed += fig.getSpeed();
            }
            averageSpeed /= figuresList.size();
            System.out.println("The average speed of figures is : " + averageSpeed);
            
            g.clearRect( 0, 0, frame.getWidth(), frame.getHeight());
            g.setColor(Color.RED);
            g.drawLine(100, 0, 100, frame.getHeight());
            g.drawLine(endX, 0, endX, frame.getHeight());
            
            if ((averageSpeed >= 20) || (strategyIndicator == 1)){
                System.out.println("Low Speed Moving Strategy");
                g.setColor(Color.RED);
                g.fillOval(midPointX, 25, 50, 50);
                g.setColor(Color.WHITE);
                strategyIndicator = 1;
            }
            //else{
                if ((averageSpeed <= 10) || (strategyIndicator == 2)){
                    System.out.println("High Speed Moving Strategy");
                    g.setColor(Color.GREEN);
                    g.fillOval(midPointX, 25, 50, 50);
                    g.setColor(Color.WHITE);
                    strategyIndicator = 2;
                }
            //}
            for (Figure fig : figuresList) {
                if(strategyIndicator == 1){
                    fig.setMovingStrategy(m1);
                }
                else{
                    fig.setMovingStrategy(m2);
                    }
                fig.move();
                if ((raceComplete == 0) && (fig.getX() >= (frame.getWidth() - 100))){
                    raceComplete = 1;
                    winnerName = fig.getName();
                }
                fig.draw(g);
                g.setColor(Color.WHITE);
                g.drawLine(0, fig.getY() + 70, frame.getWidth(), fig.getY() + 70);
            }
            frame.setVisible(true);
        }while(raceComplete == 0);
        frame.dispose();
        System.out.println("The winner is : "+ winnerName);
    }
}
