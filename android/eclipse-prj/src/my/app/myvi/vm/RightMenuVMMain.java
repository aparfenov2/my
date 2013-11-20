package my.app.myvi.vm;

import my.app.myvi.R;

import com.google.inject.Inject;

public class RightMenuVMMain extends RightMenuVMBase {
	
	@Inject
	public BtnDMEVM dmeBtnVM;

	public RightMenuVMMain() {
		super("ГЛАВНОЕ", R.layout.right_menu_main);
	}
	
	@Inject
	public BtnStartVM startBtnVM;
	@Inject
	public BtnAntVM antBtnVM;
	@Inject
	public BtnLvlVM lvlBtnVM;
	@Inject
	public BtnHipVM hipBtnVM;
	
}
