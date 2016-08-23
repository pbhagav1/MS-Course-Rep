
/**
 * Write a description of class Arena here.
 * 
 * @author (Prasanth Bhagavatula) 
 * @version (12/02/2015 - 0.1)
 */

import java.awt.*;
import java.util.*;

public class Arena implements Glyph
{
    private ArrayList<Glyph> glyphsList;

    /**
     * Constructor for objects of class Arena
     */
    public Arena()
    {
        glyphsList = new ArrayList<>();
    }

    /**
     * Add a Glyph to the Arean
     * @param  Glyph to be added to the Arena
     * @return void
     */
    public void AddGlyph(Glyph g)
    {
        glyphsList.add(g);
    }
    
    /**
     * Overriding the abstract draw method
     * @parm Graphics - The Graphics object using which the Glyph is drawn
     * @return void 
     */
    public void draw(Graphics g){
        System.out.println("This is the Arena");
        for (Glyph glyph : glyphsList)
        {
            glyph.draw(g);
        }
    }

}
