package my.app.myvi.events;

import my.app.myvi.vm.InputDialogVM;

public class InputDialogResultEvent {
	
	public InputDialogResultEvent(InputDialogVM dialogVM) {
		this.dialogVM = dialogVM;
	}
	
	private InputDialogVM dialogVM;

	public InputDialogVM getViewModel() {
		return dialogVM;
	}

	public void setDialogVM(InputDialogVM dialogVM) {
		this.dialogVM = dialogVM;
	}
}
