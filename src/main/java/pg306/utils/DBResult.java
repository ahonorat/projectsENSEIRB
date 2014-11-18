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

public class DBResult<T> {
	
	/*
	 * So it's a bad magic tabular yeah ! What's the reason for ?
	 * It's because we use generics. So when you want to retrieve results,
	 * in runtime you cannot know the type of these generics. That's why a class
	 * is needed in the constructor. And that's why a magic number is needed, it's
	 * just to ensure that we will instantiate the generic object (a data value)
	 * using the right constructor. It's up to the developer to have the same type
	 * of constructors in all the implementations of DataValue abstract class. 
	 */
	private static final Class<?>[] magicTabular = 
		{String.class, String.class, Calendar.class};
	private Class<T> genericType = null;
	private String metric = null;
	
	public DBResult(String metric, Class<T> clsOfDataValue){
		this.metric = metric;
		this.genericType = clsOfDataValue;
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

	
	private List<T> getDataPointsFromJson(List<JsonObject> listJson){
		List<T> listDP = new ArrayList<T>();
		Iterator<JsonObject> it = listJson.iterator();
		Gson gson = new Gson();

		Constructor<?>[] dpConstructors = genericType.getConstructors();
		Constructor<?> constr = null;
		for(Constructor<?> cst : dpConstructors){
			if (cst.getParameterTypes().length==5){
				Boolean match = true;
				for (int i = 0; i< 3; i++){
					match = cst.getParameterTypes()[i].equals(magicTabular[i]) && match;
				}
				if (match){
					constr = cst;
					break;
				} else {
					System.out.println("\u001B[31mNo constructor found !\u001B[0m");
				}
			}
		}
		
		while(it.hasNext() && constr != null){
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
				try {
					T dp = (T) constr.newInstance(name,unit,date,
							constr.getParameterTypes()[4],
							gson.fromJson(valueEl, constr.getParameterTypes()[4]));
					listDP.add(dp);
				} catch (InstantiationException | IllegalAccessException
						| IllegalArgumentException | InvocationTargetException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}
		}
		return listDP;
	}

	public List<T> getLatestDataPoints(Calendar start){
		return getDataPointsFromJson(retrieveJson(start, Calendar.getInstance()));
	}
	public List<T> getLatestDataPoints(Integer count){
		return getDataPointsFromJson(retrieveJson(count));
	}
	public List<T> getDataPointsBetween(Calendar start, Calendar end){
		return getDataPointsFromJson(retrieveJson(start, end));		
	}

}
