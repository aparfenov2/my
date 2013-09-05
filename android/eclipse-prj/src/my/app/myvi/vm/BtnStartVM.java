package my.app.myvi.vm;

import gueei.binding.Command;
import gueei.binding.observables.StringObservable;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import android.view.View;

public class BtnStartVM {	

	private final Logger log = LoggerFactory.getLogger(BtnStartVM.class);
	
	public BtnStartVM() {
		
	}
	
	private enum State {
		STARTED,
		STOPPED
	}
	
	private State state = State.STOPPED;	
	
	public Command Click = new Command(){
		@Override
		public void Invoke(View arg0, Object... arg1) {
			log.debug("start btn clicked !");

			if (state == State.STOPPED) {
				state = State.STARTED;
				
			} else if (state == State.STARTED) {
				state = State.STOPPED;
			}
			
			
			if (state == State.STOPPED) {
				midText.set("C");
				botText.set("СТАРТ");
				
			} else if (state == State.STARTED) {
				midText.set("D");
				botText.set("СТОП");
				
			}

		}
	};
	
	public StringObservable midText = new StringObservable("C");
	public StringObservable botText = new StringObservable("СТАРТ");
}
