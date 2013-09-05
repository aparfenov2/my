package my.app.myvi.vm;

import my.app.myvi.R;

public class RightMenuVMMain extends RightMenuVMBase {

	public RightMenuVMMain() {
		super("ГЛАВНОЕ", R.layout.right_menu_main);
	}
	public BtnStartVM startBtnVM = new BtnStartVM();
	public BtnDMEVM dmeBtnVM = new BtnDMEVM();
	public BtnAntVM antBtnVM = new BtnAntVM();
	public BtnLvlVM lvlBtnVM = new BtnLvlVM();
	public BtnHipVM hipBtnVM = new BtnHipVM();
	
}
