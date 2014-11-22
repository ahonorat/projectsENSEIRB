package pg306.utils;

import java.lang.reflect.Constructor;
import java.lang.reflect.InvocationTargetException;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Iterator;
import java.util.List;

import com.couchbase.client.protocol.views.ViewResponse;
import com.couchbase.client.protocol.views.ViewRow;
import com.google.gson.Gson;
import com.google.gson.JsonArray;
import com.google.gson.JsonElement;
import com.google.gson.JsonObject;
import com.google.gson.JsonParser;

public class DBResult {
	
	private String metric = null;
	
	public DBResult(String metric){
		this.metric = metric;
	}
	
		
	private List<JsonObject> retrieveJson(Calendar start, Calendar end){
		List<JsonObject> list = new ArrayList<JsonObject>();
		JsonParser parser = new JsonParser();
		ViewResponse res = DBConnexion.retrieveViewResult(metric, start, end);
		if (res != null){
			if (res.size() == 0)
				System.out.println("\u001B[31mNo row found !\u001B[0m");
			for (ViewRow row : res) {
				String json = (String) row.getDocument();
				if (json != null){
					JsonElement tmp = parser.parse(json);
					list.add(tmp.getAsJsonObject());
				}
			}
		}
		return list;
	}
	
	private List<JsonObject> retrieveJson(Integer count){
		List<JsonObject> list = new ArrayList<JsonObject>();
		JsonParser parser = new JsonParser();
		ViewResponse res = DBConnexion.retrieveViewResult(metric, count);
		if (res != null){
			for (ViewRow row : res) {
				String json = (String) row.getDocument();
				if (json != null){
					JsonElement tmp = parser.parse(json);
					list.add(tmp.getAsJsonObject());
				}
			}
		}
		return list;
	}

	
	private List<DataValue> getDataPointsFromJson(List<JsonObject> listJson){
		List<DataValue> listDP = new ArrayList<DataValue>();
		Iterator<JsonObject> it = listJson.iterator();
		Gson gson = new Gson();
		
		while(it.hasNext()){
			JsonObject obj = it.next();
			JsonObject dateObj = obj.getAsJsonObject("date");
			Calendar date = gson.fromJson(dateObj, Calendar.class);
			//Calendar may need a specific serializer/deserializer
			
			JsonElement nameEl = obj.get("metric");
			JsonElement unitEl = obj.get("unit");
			String name = gson.fromJson(nameEl, String.class);
			String unit = gson.fromJson(unitEl, String.class);
			JsonArray valuesArray  = obj.getAsJsonArray("values");
			
			Iterator<JsonElement> itVal = valuesArray.iterator();
			
			while (itVal.hasNext()){
				JsonObject objVal = itVal.next().getAsJsonObject();
				JsonElement valueEl = objVal.get("value");
				DataValue dp = new DataValue(name,unit,date,
						gson.fromJson(valueEl, Integer.class));
				listDP.add(dp);
			}
		}
		return listDP;
	}

	public List<DataValue> getLatestDataPoints(Calendar start){
		return getDataPointsFromJson(retrieveJson(start, Calendar.getInstance()));
	}
	public List<DataValue> getLatestDataPoints(Integer count){
		return getDataPointsFromJson(retrieveJson(count));
	}
	public List<DataValue> getDataPointsBetween(Calendar start, Calendar end){
		return getDataPointsFromJson(retrieveJson(start, end));		
	}

}
