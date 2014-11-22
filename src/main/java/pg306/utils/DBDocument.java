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
		JsonObject data = new JsonObject();
		data.addProperty("value", (Number) datapoint.getValue());
		values.add(data);
		return true;
	}
	
	protected String getJson(){
		if (!merge){
			obj.add("values", values);
			merge = true;
		}
		return gson.toJson(obj);
	}

}
