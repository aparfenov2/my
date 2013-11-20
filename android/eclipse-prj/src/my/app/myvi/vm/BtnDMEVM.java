package my.app.myvi.vm;

import gueei.binding.Command;
import gueei.binding.observables.StringObservable;
import gueei.binding.v30.app.BindingWidgetV30;
import my.app.myvi.R;
import my.app.myvi.events.InputDialogResultEvent;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import android.app.Dialog;
import android.content.Context;
import android.view.View;

import com.google.inject.Inject;

import de.greenrobot.event.EventBus;

public class BtnDMEVM  {
	
	private final Logger log = LoggerFactory.getLogger(BtnDMEVM.class);
	
	@Inject
	private Context context;
	
	public BtnDMEVM() {
		EventBus.getDefault().register(this, InputDialogResultEvent.class);
	}
	
	public Command Click = new Command(){
		@Override
		public void Invoke(View arg0, Object... arg1) {
			log.debug("DME btn clicked !");
			InputDialogVM vm = new InputDialogVM();
			final Dialog dialog = BindingWidgetV30.createAndBindDialog(
					context, R.layout.input_dialog, vm 
					);
			//dialog.setTitle("Title...");
			vm.setDialog(dialog);
			vm.setCaller(BtnDMEVM.this);
			vm.setValue(midText.get());
			dialog.show();
		}
	};
	
	public void onEvent(InputDialogResultEvent event) {
		if (event.getViewModel().getCaller() == this) {
			log.debug("input dialog result received !");
			this.midText.set(event.getViewModel().getValue());
		}
	}
	
	
	public StringObservable midText = new StringObservable("!50X");

}
