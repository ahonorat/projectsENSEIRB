package pg306.utils;

import java.util.Calendar;
import java.util.Random;


public class CreateSampleDocuments {
	
	private static Random rand = new Random();

	private static int randInt(int min, int max) {
	    int randomNum = rand.nextInt((max - min) + 1) + min;
	    return randomNum;
	}

	public static void storeFakeDocs(MonitorAbstract monit){
		int nbDocs = randInt(5,10);
		for (int i = 0; i<nbDocs; i++){
			Calendar cal = Calendar.getInstance();
			DBDocument doc = new DBDocument("dataCollection", cal, monit.getName(), "N/A");
			DataValue dv = new DataValue(monit.getName(), "N/A", cal, randInt(0,100));
			doc.addDataValue(dv);
			DBConnexion.sendDocument(doc);		
		}			
	}
	
	
	public static void flushDB(){
		DBConnexion.flushDB();
	}
			
	
}
