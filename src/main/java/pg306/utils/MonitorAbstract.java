package pg306.utils;


public abstract class MonitorAbstract {

	private String name = null;
	private Long period = (long) 0;
	
	protected MonitorAbstract(String name) {
		this.name = name;
	}
	
	public String getName() {
		return name;
	}

	public abstract void refreshData();
	
	public Long getPeriod(){return period;} // period in s, 0 is equivalent to never
	public void setPeriod(Long period){this.period = period; refreshData();} // period in s, 0 is equivalent to never
	
}
