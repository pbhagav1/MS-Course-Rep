
/**
 * Write a description of class Canyon here.
 * 
 * @author (Prasanth Bhagavatula) 
 * @version (12/02/2015 - 0.1)
 */

import java.awt.*;

public class Canyon implements Glyph
{
    /**
     * Overriding the abstract draw method
     * @parm Graphics - The Graphics object using which the Glyph is drawn
     * @return void 
     */
    public void draw(Graphics g){
        System.out.println("This is a Canyon");
    }
}
