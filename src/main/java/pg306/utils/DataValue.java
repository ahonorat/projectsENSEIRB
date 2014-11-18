package pg306.utils;

import java.util.Calendar;

public class DataValue {
	private String name = null;
	private String unit = null;
	private Calendar calendar = null;
	private Class<?> type = null;
	private Integer v1 = null;
	
	
	public DataValue(String name, String unit, Calendar calendar, Class<?> type, Integer v1) {
		this.name = name;
		this.unit = unit;
		this.calendar = calendar;
		this.type = type;
		this.v1 = v1;
	}

	public String getName() {
		return name;
	}

	public String getUnit() {
		return unit;
	}

	public Calendar getCalendar() {
		return calendar;
	}

	public Class<?> getType() {
		return type;
	}

	public String toString(){
		return ("Name : "+getName()+
				"\nUnit : "+getUnit()+
				"\nCalendar : "+getCalendar().getTime()+
				"\nValue : "+getValue().toString());
	}
	
	public Integer getValue() {
		return v1;
	}

}
