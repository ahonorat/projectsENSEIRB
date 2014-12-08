package pg306.client;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.Timer;

import org.kohsuke.args4j.CmdLineException;
import org.kohsuke.args4j.CmdLineParser;
import org.kohsuke.args4j.Option;

import pg306.utils.CreateSampleDocuments;

/**
 * Hello world!
 * 
 */
public class App {
		
	@Option(name = "-f", usage = "flush DataBase at starting")
	private boolean flush;

	@Option(name = "-d", usage = "create some random datas")
	private boolean datas;

	@Option(name = "-h", usage = "print this help")
	private boolean help;

	public static void main(String[] args) {
		System.out.println("Hello World!");
		new App().doMain(args);
	}

	public void doMain(String[] args) {
		CmdLineParser parser = new CmdLineParser(this);
		try {
			parser.parseArgument(args);
		} catch (CmdLineException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			parser.printUsage(System.out);
			return;
		}

		if (help)
			parser.printUsage(System.out);
		else {
			Timer time = new Timer(); // Instantiate Timer Object
			MonitorTask monit = new MonitorTask(); // Instantiate SheduledTask class
			time.schedule(monit, 0, 10000); // Create Repetitively task for every 10 secs	

			if (flush)
				CreateSampleDocuments.flushDB();
			if (datas)
				CreateSampleDocuments.storeFakeDocs();
			try {
				BufferedReader br = new BufferedReader(new InputStreamReader(
						System.in));

				String input;
				while ((input = br.readLine()) != null) {
					System.out.println(input);
				}

			} catch (IOException io) {
				io.printStackTrace();
			}

		}

	}
}
