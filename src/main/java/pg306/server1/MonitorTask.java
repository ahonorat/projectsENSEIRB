package pg306.server1;

import java.util.Calendar;
import java.util.List;
import java.util.TimerTask;

import pg306.utils.DBResult;
import pg306.utils.DataValue;


public class MonitorTask extends TimerTask {

	@Override
	public void run() {
		System.out.println("coucou du moniteur1");
		Calendar previously = Calendar.getInstance();
		previously.set(Calendar.YEAR,2013);
		DBResult res = new DBResult("monitor1");
		List<DataValue> list = res.getDataPointsBetween(previously, Calendar.getInstance());
		if (list.isEmpty())
			System.out.println("\u001B[31mNo item found !\u001B[0m");
		for(DataValue dv : list){
			System.out.println(dv.toString());
		}
	}
}
