package vm;

import gueei.binding.Command;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import android.view.View;

public class BtnAntVM {

	private final Logger log = LoggerFactory.getLogger(BtnAntVM.class);
	
	public Command Click = new Command(){
		@Override
		public void Invoke(View arg0, Object... arg1) {
			log.debug("Ant btn clicked !");
		}
	};
}
