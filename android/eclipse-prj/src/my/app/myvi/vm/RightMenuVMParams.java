package my.app.myvi.vm;

import my.app.myvi.R;

import com.google.inject.Inject;

public class RightMenuVMParams extends RightMenuVMBase {

	public RightMenuVMParams() {
		super("ПАРАМЕТРЫ", R.layout.right_menu_params);
	}

	@Inject
	public BtnFzaprVM fzaprBtnVM;
	@Inject
	public BtnFprdVM fprdBtnVM;
	
	
}
