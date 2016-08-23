
/**
 * Write a description of class BatManFactory here.
 * 
 * @author (Prasanth Bhagavatula) 
 * @version (12/02/2015 - 0.1)
 */
public class BatManFactory implements HeroFactory
{
    private static BatManFactory bmf;

    /**
     * Overriding the Default Constructor
     */
    private BatManFactory(){
    }
    
    /**
     * Method to create an instance of BatManFactory
     * @param  void
     * @return BatManFactory object
     */
    public static synchronized BatManFactory getBatManFactory(){
        if (bmf == null){
            bmf = new BatManFactory();
        }
        return bmf;
    }
    
    /**
     * Overriding the getHero Method
     * @param  void
     * @return BatMan object pointed by Hero reference variable
     */
    public Hero getHero()
    {
        return BatMan.getBatMan();
    }
    
    /**
     * Overriding the getHero Method
     * @param  inpLevel - Level of BatMan; inpPowerLevel - Power Level of BatMan
     * @return BatMan object pointed by Hero reference variable
     */
    public Hero getHero(int inpLevel, int inpPowerLevel)
    {
        return BatMan.getBatMan(inpLevel, inpPowerLevel);
    }
    
    /**
     * Method to override the clone() method
     * @param void
     * @return Object
     * @throws CloneNotSupportedException
     */
    public Object clone() throws CloneNotSupportedException{
        throw new CloneNotSupportedException();
    }
}
