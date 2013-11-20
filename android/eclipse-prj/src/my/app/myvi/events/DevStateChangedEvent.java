package my.app.myvi.events;

import my.app.myvi.appmodel.DevState;


public class DevStateChangedEvent {
	
	private DevState state;

	public DevStateChangedEvent(DevState state) {
		this.setState(state);
	}

	public DevState getState() {
		return state;
	}

	public void setState(DevState state) {
		this.state = state;
	}

	
}
