package pg306.utils;

import java.util.Calendar;

public class DataValue {
	private String name = null;
	private Calendar calendar = null;
	private Integer v1 = null;
	
	
	public DataValue(String name, Calendar calendar, Integer v1) {
		this.name = name;
		this.calendar = calendar;
		this.v1 = v1;
	}

	public String getName() {
		return name;
	}

	public Calendar getCalendar() {
		return calendar;
	}

	public String toString(){
		return ("Name : "+getName()+
				"\nCalendar : "+getCalendar().getTime()+
				"\nValue : "+getValue().toString());
	}
	
	public Integer getValue() {
		return v1;
	}

}
