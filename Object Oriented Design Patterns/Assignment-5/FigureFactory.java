/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 *
 * @author Prasanth_B
 */
public class FigureFactory {
    private static FigureFactory ff;
    private FigureFactory(){
    }
    
    public static synchronized FigureFactory getFigureFactory(){
        if (ff == null){
            ff = new FigureFactory();
        }
        return ff;
    }
    public Figure getFigure(int choice, int inpX, int inpY, int otherLength, int inpColor){
        Figure f;
        switch (choice){
            case 0:
                f = Circle.getCircle(inpX, inpY, otherLength, inpColor, "Circle");
                break;
            case 1:
                f = Square.getSquare(inpX, inpY, otherLength, inpColor, "Square");
                break;   
            case 2:
                f = Triangle.getTriangle(inpX, inpY, otherLength, inpColor, "Triangle");
                break;
            default:
                f = Hexagon.getHexagon(inpX, inpY, otherLength, inpColor, "Hexagon");
        }
        return f;
    }
    
    @Override
    public Object clone() throws CloneNotSupportedException{
        throw new CloneNotSupportedException();
    }
}