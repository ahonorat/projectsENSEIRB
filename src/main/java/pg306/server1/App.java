package pg306.server1;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;

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
			Monitor1 monit = new Monitor1();

			if (flush)
				CreateSampleDocuments.flushDB();
			if (datas)
				CreateSampleDocuments.storeFakeDocs(monit);
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
