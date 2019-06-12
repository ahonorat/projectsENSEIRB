package pg306.utils;

import java.util.Calendar;

import com.google.gson.Gson;
import com.google.gson.GsonBuilder;
import com.google.gson.JsonElement;
import com.google.gson.JsonObject;

public class DBDocument {

	private JsonObject obj = null;
	private Gson gson = null;

	public DBDocument(DataValue dv) {
		gson = new GsonBuilder().serializeNulls().create();
		obj = new JsonObject();

		JsonElement element = gson.toJsonTree(dv.getCalendar() , Calendar.class);
		obj.add("date", element);
		obj.addProperty("name", dv.getName());
		obj.addProperty("value", (Number) dv.getValue());
	}

	protected String getJson() {
		return gson.toJson(obj);
	}

}
