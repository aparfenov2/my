package my.app.myvi.vm;

import gueei.binding.Command;
import gueei.binding.observables.StringObservable;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import android.view.View;

public class BtnAntVM {

	private final Logger log = LoggerFactory.getLogger(BtnAntVM.class);
	
	private enum State {
		ANT,
		ATTEN,
		CALIB
	}
	
	
	private State state = State.ANT;
	
// chars E - ant, J - atten, K - calibr	
	public Command Click = new Command(){
		@Override
		public void Invoke(View arg0, Object... arg1) {
			log.debug("Ant btn clicked !");
			
			if (state == State.ANT) {
				state = State.ATTEN;
				
			} else if (state == State.ATTEN) {
				state = State.CALIB;
				
			} else if (state == State.CALIB) {
				state = State.ANT;
			}
			

			if (state == State.ANT) {
				midText.set("E");
				botText.set("АНТЕННА");
				
			} else if (state == State.ATTEN) {
				midText.set("J");
				botText.set("АТТЕН.");
				
			} else if (state == State.CALIB) {
				midText.set("K");
				botText.set("КАЛИБР.");
			}
			
		}
	};
	
	public StringObservable midText = new StringObservable("E");
	public StringObservable botText = new StringObservable("АНТЕННА");
	
}
