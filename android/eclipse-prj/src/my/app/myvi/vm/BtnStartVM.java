package my.app.myvi.vm;

import gueei.binding.Command;
import gueei.binding.observables.StringObservable;
import my.app.myvi.appmodel.DevState;
import my.app.myvi.events.DevStateChangedEvent;
import my.app.myvi.events.DevStateToggleEvent;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import android.view.View;
import de.greenrobot.event.EventBus;

public class BtnStartVM {	

	private final Logger log = LoggerFactory.getLogger(BtnStartVM.class);
	
	public BtnStartVM() {
		EventBus.getDefault().register(this, DevStateChangedEvent.class);
	}
	
	
	
	
	public void onEvent(DevStateChangedEvent event) {
		
		if (event.getState() == DevState.STOPPED) {
			midText.set("C");
			botText.set("СТАРТ");
			
		} else if (event.getState() == DevState.STARTED) {
			midText.set("D");
			botText.set("СТОП");
		}
	}
	
	
	public Command Click = new Command(){
		@Override
		public void Invoke(View arg0, Object... arg1) {
			log.debug("start btn clicked !");
			EventBus.getDefault().post(new DevStateToggleEvent());
			
		}
	};
	
	public StringObservable midText = new StringObservable("!C");
	public StringObservable botText = new StringObservable("!СТАРТ");
}
