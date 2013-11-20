package my.app.myvi.vm;

import gueei.binding.Command;
import gueei.binding.observables.StringObservable;
import my.app.myvi.events.InputDialogResultEvent;
import android.app.Dialog;
import android.view.View;
import de.greenrobot.event.EventBus;

public class InputDialogVM {
	
	public StringObservable valueText = new StringObservable("!33");
	
	private Object caller;
	private Dialog dialog;
	
	public Command ApplyClick = new Command(){

		@Override
		public void Invoke(View view, Object... args) {
			InputDialogVM.this.dialog.dismiss();
			EventBus.getDefault().post(new InputDialogResultEvent(InputDialogVM.this));
		}
		
	};
	
	public Command CancelClick = new Command(){

		@Override
		public void Invoke(View view, Object... args) {
			InputDialogVM.this.dialog.dismiss();
		}
		
	};


	public void setDialog(Dialog dialog) {
		this.dialog = dialog;
	}
	
	public void setCaller(Object caller) {
		this.caller = caller;
	}

	
	public Object getCaller() {
		return caller;
	}

	public void setValue(String value) {		
		this.valueText.set(value);
	}
	
	public String getValue() {
		return this.valueText.get();
	}
	
	
}
