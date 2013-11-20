package my.app.myvi.appmodel;

import my.app.myvi.events.DevStateChangedEvent;
import my.app.myvi.events.DevStateToggleEvent;
import android.os.Handler;
import de.greenrobot.event.EventBus;

public class AppModel {
	
	private DevState devState = DevState.STOPPED;
	
	public AppModel() {
		EventBus.getDefault().register(this, DevStateChangedEvent.class);
		EventBus.getDefault().register(this, DevStateToggleEvent.class);
		
		new Handler().postDelayed(new Runnable() {

			@Override
			public void run() {
				EventBus.getDefault().post(new DevStateChangedEvent(devState));
			}
			
		}, 1);
		
	}
	
	public void onEvent(DevStateChangedEvent event) {
		devState = event.getState();
	}

	public void onEvent(DevStateToggleEvent event) {
		if (devState == DevState.STOPPED) {
			devState = DevState.STARTED;
			
		} else if (devState == DevState.STARTED) {
			devState = DevState.STOPPED;
		}
		EventBus.getDefault().post(new DevStateChangedEvent(devState));
	}
	

}
