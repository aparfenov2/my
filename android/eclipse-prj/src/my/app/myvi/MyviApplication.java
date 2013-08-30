package my.app.myvi;

import gueei.binding.Binder;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import android.app.Application;

public class MyviApplication extends Application {
	
	private final Logger log = LoggerFactory.getLogger(MyviApplication.class);
	
	@Override
	public void onCreate() {
		super.onCreate();
		
        ConfigureLog4J.configure();
        log.debug("sl4j operation ok");
		
		Binder.init(this);
	}

}
