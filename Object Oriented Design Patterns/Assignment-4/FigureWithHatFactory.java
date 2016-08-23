/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 *
 * @author Prasanth_B
 */
public class FigureWithHatFactory implements FigureDecoratorFactory{
    
    private static FigureWithHatFactory fwh;
    private FigureWithHatFactory(){
    }
    
    public static synchronized FigureWithHatFactory getFigureWithHatFactory(){
        if (fwh == null){
            fwh = new FigureWithHatFactory();
        }
        return fwh;
    }
    
    @Override
    public Figure getFigure(String figureType){
        Figure f;
        switch (figureType){
            case "SQUARE":
                f = new FigureWithHat(Square.getSquare());
                break;
            case "CIRCLE":
                f = new FigureWithHat(Circle.getCircle());
                break;
            case "HEXAGON":
                f = new FigureWithHat(Hexagon.getHexagon());
                break;
            default:
                f = new FigureWithHat(Triangle.getTriangle());
        }
        return f;
    }
    
    @Override
    public Object clone() throws CloneNotSupportedException{
        throw new CloneNotSupportedException();
    }
}
