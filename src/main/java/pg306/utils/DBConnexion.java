package pg306.utils;

import java.io.IOException;
import java.net.URI;
import java.net.URISyntaxException;
import java.util.Arrays;
import java.util.Calendar;
import java.util.List;
import java.util.UUID;
import java.util.concurrent.ExecutionException;

import net.spy.memcached.PersistTo;

import com.couchbase.client.CouchbaseClient;
import com.couchbase.client.protocol.views.ComplexKey;
import com.couchbase.client.protocol.views.DesignDocument;
import com.couchbase.client.protocol.views.InvalidViewException;
import com.couchbase.client.protocol.views.Query;
import com.couchbase.client.protocol.views.View;
import com.couchbase.client.protocol.views.ViewDesign;
import com.couchbase.client.protocol.views.ViewResponse;
import com.couchbase.client.protocol.views.ViewRow;

public class DBConnexion {

	private static String node = "http://127.0.0.1:8091/pools";
	private static String bucket = "pg306";
	private static String password = "";
	private static String docDesign = "monitoring";
	private static String mainViewName = "findDates";
	private static String allDocsViewName = "allDocs";
	private static String viewMode = "development"; // change to 'production'
													// when ready

	private static String mainView = "function (doc, meta) {\n"
			+ "  if (doc.name){\n"
			+ "    emit([doc.name, doc.date.year,doc.date.month, doc.date.dayOfMonth,\n"
			+ "          doc.date.hourOfDay, doc.date.minute, doc.date.second],\n"
			+ "          null); \n  } \n} ";

	private static String allDocsView = "function (doc, meta) {\n"
			+ " if (doc.name){\n"
			+ "   emit(meta.id,null); \n } \n }";

	private static CouchbaseClient establishConnexion() {
		List<URI> hosts;
		CouchbaseClient client = null;
		try {
			hosts = Arrays.asList(new URI(node));
			client = new CouchbaseClient(hosts, bucket, password);

		} catch (URISyntaxException | IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return client;
	}

	private static void endConnexion(CouchbaseClient client) {
		client.shutdown();
	}

	private static ComplexKey computeComplexKey(Calendar date, String metric) {
		return ComplexKey.of(metric, date.get(Calendar.YEAR),
				date.get(Calendar.MONTH), date.get(Calendar.DAY_OF_MONTH),
				date.get(Calendar.HOUR_OF_DAY), date.get(Calendar.MINUTE),
				date.get(Calendar.SECOND));
	}

	/*
	 * If the views doesn't exist, it install them should not be called in
	 * production mode.
	 */
	private static boolean checkView() {
		CouchbaseClient client = establishConnexion();
		DesignDocument designDoc = null;
		Boolean res = false;
		if (client != null) {
			try {
				client.getView(docDesign, mainViewName);
				client.getView(docDesign, allDocsViewName);
			} catch (InvalidViewException e) {
				client.deleteDesignDoc(docDesign);
			}
			try {
				// create the mainView and the design document
				designDoc = client.getDesignDoc(docDesign);
			} catch (InvalidViewException f) {
				designDoc = new DesignDocument(docDesign);
				ViewDesign viewDesign1 = new ViewDesign(mainViewName, mainView);
				ViewDesign viewDesign2 = new ViewDesign(allDocsViewName,
						allDocsView);
				designDoc.setView(viewDesign1);
				designDoc.setView(viewDesign2);
				// be aware that createDesignDoc is certainly launched in a
				// separate thread
				// so either you store the result, either you add .wait().
				res = client.createDesignDoc(designDoc);

			}
		}
		endConnexion(client);
		return res;
	}

	public static boolean sendDocument(DBDocument doc) {
		Boolean res = false;
		CouchbaseClient client = establishConnexion();
		if (client != null) {
			// Store the Document
			try {
				res = client.set(UUID.randomUUID().toString(), doc.getJson())
						.get();
			} catch (InterruptedException | ExecutionException e) {
				e.printStackTrace();
				return false;
			}
			endConnexion(client);
		}
		return res;
	}

	public static ViewResponse retrieveViewResult(String metric, Integer count) {
		System.setProperty("viewmode", viewMode);
		checkView();
		ViewResponse response = null;
		CouchbaseClient client = establishConnexion();
		View view = null;
		if (client != null) {
			view = client.getView(docDesign, mainViewName);
			Query query = new Query();
			query.setReduce(false);
			query.setLimit(count);
			query.setIncludeDocs(true);
			query.setDescending(true);
			response = client.query(view, query);
			endConnexion(client);
		}
		return response;
	}

	public static ViewResponse retrieveViewResult(String metric,
			Calendar start, Calendar end) {
		System.setProperty("viewmode", viewMode);
		checkView();
		ViewResponse response = null;
		CouchbaseClient client = establishConnexion();
		View view = null;
		if (client != null) {
			view = client.getView(docDesign, mainViewName);
			Query query = new Query();
			ComplexKey k1 = computeComplexKey(start, metric);
			ComplexKey k2 = computeComplexKey(end, metric);
			query.setReduce(false);
			query.setRange(k1, k2);
			query.setIncludeDocs(true);
			response = client.query(view, query);
			endConnexion(client);
		}
		return response;
	}

	protected static void flushDB() {
		System.setProperty("viewmode", viewMode);
		checkView();
		CouchbaseClient client = establishConnexion();
		ViewResponse response = null;
		View view = null;
		if (client != null) {
			view = client.getView(docDesign, allDocsViewName);
			Query query = new Query();
			query.setReduce(false);
			query.setIncludeDocs(false);
			response = client.query(view, query);
			for (ViewRow row : response) {
				try {
					client.delete(row.getId(), PersistTo.ONE).get();
				} catch (InterruptedException | ExecutionException e) {
					e.printStackTrace();
				}
			}
			endConnexion(client);
		}
	}
}