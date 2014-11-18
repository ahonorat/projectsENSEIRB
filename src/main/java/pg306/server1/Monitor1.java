package pg306.server1;

import java.util.Timer;


public class Monitor1 extends pg306.utils.MonitorAbstract  {

	private static String name = "monitor1";
	
	private MonitorTask monit;
	private final Timer time;

	
	public Monitor1() {
		super(name);
		time = new Timer(); // Instantiate Timer Object
		monit = new MonitorTask(); // Instantiate SheduledTask class
		time.schedule(monit, 0, 10000); // Create Repetitively task for every 10 secs	
	}


	@Override
	public void refreshData() {
		monit.cancel();
		if (! super.getPeriod().equals((long) 0)){
			monit = new MonitorTask();
			time.schedule(monit, 0, super.getPeriod());
		}
		// TODO Auto-generated method stub
	}
}
