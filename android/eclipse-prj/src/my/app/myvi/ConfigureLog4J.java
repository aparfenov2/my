package my.app.myvi;

import java.io.File;

import org.apache.log4j.Level;

import android.os.Environment;
import de.mindpipe.android.logging.log4j.LogConfigurator;

public class ConfigureLog4J {
	
	public static void configure() {
		
		final LogConfigurator logConfigurator = new LogConfigurator();
		
		logConfigurator.setUseFileAppender(false);

		logConfigurator.setFileName(Environment.getExternalStorageDirectory() + File.separator + "myvi.log");
		// Set the root log level
		logConfigurator.setRootLevel(Level.DEBUG);
		// Set log level of a specific logger
		logConfigurator.setLevel("org.apache", Level.ERROR);
		logConfigurator.configure();
	}

}
