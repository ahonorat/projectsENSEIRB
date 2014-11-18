package pg306.utils;

import java.util.Calendar;

import com.google.gson.Gson;
import com.google.gson.GsonBuilder;
import com.google.gson.JsonArray;
import com.google.gson.JsonElement;
import com.google.gson.JsonObject;

public class DBDocument {

	private boolean merge = false;
	private JsonObject obj = null;
	private JsonArray values = null;
	private Calendar date = null;
	private String metric = null;
	private String unit = null;
	private Gson gson = null;
	
	public DBDocument(String documentType, Calendar date, String metric, String unit){
		this.date = date;
		this.metric = metric;
		this.unit = unit;
		
		gson = new GsonBuilder().serializeNulls().create();
		obj = new JsonObject();
		values = new JsonArray();

		JsonElement element = gson.toJsonTree(date, Calendar.class);
		obj.add("date", element);
		obj.addProperty("documentType", documentType);
		obj.addProperty("metric", metric);
		obj.addProperty("unit", unit);
	}
	
	public boolean addDataValue(DataValue datapoint){
		Class<?> valueType = datapoint.getType();
		if (datapoint.getCalendar().equals(date) && datapoint.getName().equals(metric)
				&& datapoint.getUnit().equals(unit) && !merge){
			JsonObject data = new JsonObject();
			if (Number.class.isAssignableFrom(valueType))
				data.addProperty("value", (Number) datapoint.getValue());
			else {
				//!!! the Json may fails the serialization/deserialization in this case, 
				//maybe we should throw an exception
				JsonElement element = gson.toJsonTree(datapoint.getValue(), valueType);
				data.add("unknown", element);
			}
			values.add(data);
			return true;
		}
		return false;
	}
	
	protected String getJson(){
		if (!merge){
			obj.add("values", values);
			merge = true;
		}
		return gson.toJson(obj);
	}

}
