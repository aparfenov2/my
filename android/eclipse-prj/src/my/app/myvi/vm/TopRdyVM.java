package my.app.myvi.vm;

import gueei.binding.observables.StringObservable;
import my.app.myvi.appmodel.DevState;
import my.app.myvi.events.DevStateChangedEvent;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import de.greenrobot.event.EventBus;

public class TopRdyVM {
	
	private final Logger log = LoggerFactory.getLogger(TopRdyVM.class);
	
	public TopRdyVM() {
		EventBus.getDefault().register(this, DevStateChangedEvent.class);
	}
	
	public void onEvent(DevStateChangedEvent event) {
		log.debug("label got event");
		
		if (event.getState() == DevState.STOPPED) {
			txtg.set("C");
			txt.set("ГОТОВ");
			
		} else if (event.getState() == DevState.STARTED) {
			txtg.set("D");
			txt.set("ЗАНЯТ");
		}
		
    }
	
	public StringObservable txtg = new StringObservable("C");
	public StringObservable txt = new StringObservable("ГОТОВ");
}
