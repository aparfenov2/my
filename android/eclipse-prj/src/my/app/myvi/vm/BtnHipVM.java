package my.app.myvi.vm;

import gueei.binding.Command;
import gueei.binding.observables.StringObservable;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import android.view.View;

public class BtnHipVM {

	private final Logger log = LoggerFactory.getLogger(BtnHipVM.class);
	
	// F = _||_, L = _|-
	
	private enum State {
		HIP,
		PACKET,
		EXT
	}
	
	
	private State state = State.HIP;
	
	public Command Click = new Command(){
		@Override
		public void Invoke(View arg0, Object... arg1) {
			log.debug("Hip btn clicked !");
			
			if (state == State.HIP) {
				state = State.PACKET;
				
			} else if (state == State.PACKET) {
				state = State.EXT;
				
			} else if (state == State.EXT) {
				state = State.HIP;
			}
			
			if (state == State.HIP) {
				midText.set("ХИП");
				botText.set("FFFF");
				
			} else if (state == State.PACKET) {
				midText.set("ПАЧКА");
				botText.set("FFFF");
				
			} else if (state == State.EXT) {
				midText.set("ВНЕШ");
				botText.set("L");
			}
			
		}
	};
	
	public StringObservable midText = new StringObservable("ХИП");
	public StringObservable botText = new StringObservable("FFFF");
	
}
