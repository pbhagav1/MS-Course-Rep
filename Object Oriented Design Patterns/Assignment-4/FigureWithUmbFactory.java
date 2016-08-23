/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 *
 * @author Prasanth_B
 */
public class FigureWithUmbFactory implements FigureDecoratorFactory{
    
    private static FigureWithUmbFactory fwu;
    
    private FigureWithUmbFactory(){
    }
    
    public static synchronized FigureWithUmbFactory getFigureWithUmbFactory(){
        if (fwu == null){
            fwu = new FigureWithUmbFactory();
        }
        return fwu;
    }
    
    @Override
    public Figure getFigure(String figureType){
        Figure f;
        switch (figureType){
            case "SQUARE":
                f = new FigureWithUmb(Square.getSquare());
                break;
            case "CIRCLE":
                f = new FigureWithUmb(Circle.getCircle());
                break;
            case "HEXAGON":
                f = new FigureWithUmb(Hexagon.getHexagon());
                break;
            default:
                f = new FigureWithUmb(Triangle.getTriangle());
        }
        return f;
    }
    
    @Override
    public Object clone() throws CloneNotSupportedException{
        throw new CloneNotSupportedException();
    }
}
