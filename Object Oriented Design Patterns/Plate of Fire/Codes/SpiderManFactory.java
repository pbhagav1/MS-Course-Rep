
/**
 * Write a description of class SpiderManFactory here.
 * 
 * @author (Prasanth Bhagavatula) 
 * @version (12/02/2015 - 0.1)
 */
public class SpiderManFactory implements HeroFactory
{
    private static SpiderManFactory smf;

    /**
     * Overriding the Default Constructor
     */
    private SpiderManFactory(){
    }
    
    /**
     * Method to create an instance of SpiderManFactory
     * @param  void
     * @return SpiderManFactory object
     */
    public static synchronized SpiderManFactory getSpiderManFactory(){
        if (smf == null){
            smf = new SpiderManFactory();
        }
        return smf;
    }
    
    /**
     * Overriding the getHero Method
     * @param  void
     * @return SpiderMan object pointed by Hero reference variable
     */
    public Hero getHero()
    {
        return SpiderMan.getSpiderMan();
    }
    
    /**
     * Overriding the getHero Method
     * @param  inpLevel - Level of SpiderMan; inpPowerLevel - Power Level of SpiderMan
     * @return SpiderMan object pointed by Hero reference variable
     */
    public Hero getHero(int inpLevel, int inpPowerLevel)
    {
        return SpiderMan.getSpiderMan(inpLevel, inpPowerLevel);
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
