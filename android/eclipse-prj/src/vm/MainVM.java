package vm;

import android.app.Activity;

public class MainVM {
	private Activity mContext;
	
	public MainVM(Activity mContext) {
		this.mContext = mContext;
	}
	
	public BtnStartVM startBtnVM = new BtnStartVM();
	public BtnDMEVM dmeBtnVM = new BtnDMEVM();
	public BtnAntVM antBtnVM = new BtnAntVM();
	public BtnLvlVM lvlBtnVM = new BtnLvlVM();
	public BtnHipVM hipBtnVM = new BtnHipVM();

	public TopCh50VM topCh50VM = new TopCh50VM();
	public TopFSendVM topFSendVM = new TopFSendVM();
	public TopFRecVM topFRecVM = new TopFRecVM();
	public TopDlySendVM topDlySendVM = new TopDlySendVM();
	public TopDlyRecVM topDlyRecVM = new TopDlyRecVM();
	
}
