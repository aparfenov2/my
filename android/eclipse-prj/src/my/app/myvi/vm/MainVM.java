package my.app.myvi.vm;

import gueei.binding.Command;
import gueei.binding.Observable;
import my.app.myvi.appmodel.AppModel;

import org.nnsoft.guice.lifegycle.AfterInjection;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import android.view.View;

import com.google.inject.Inject;

public class MainVM {
	
	private final Logger log = LoggerFactory.getLogger(MainVM.class);
	
	@Inject
	public RightMenuVMMain rightMenuVMMain;
	@Inject
	public RightMenuVMParams rightMenuVMParams;
	
	@AfterInjection
	public void init() {	
		rightMenuVM.set(rightMenuVMMain);
	}
	
	@Inject
	public AppModel appModel;
	
// Right Menu	
	public Observable<RightMenuVMBase> rightMenuVM = new Observable<RightMenuVMBase>(RightMenuVMBase.class);
//	public RightMenuVMBase rightMenuVM;
//	public IntegerObservable rightMenuId = new IntegerObservable(R.layout.right_menu_main);
//	public StringObservable rightMenuName = new StringObservable("xxx");
	
// Top Menu
	@Inject
	public TopCh50VM topCh50VM;
	@Inject
	public TopFSendVM topFSendVM;
	@Inject
	public TopFRecVM topFRecVM;
	@Inject
	public TopDlySendVM topDlySendVM;
	@Inject
	public TopDlyRecVM topDlyRecVM;
	@Inject
	public TopRdyVM topRdyVM;
	
// Bot Menu	
// Context Menu
	
	public final Command MenuSettingsClick = new Command(){
		@Override
		public void Invoke(View view, Object... args) {
			log.debug("ctxm: settings");
		}
	};
	
	public final Command MenuMainClick = new Command(){
		@Override
		public void Invoke(View view, Object... args) {
			rightMenuVM.set(rightMenuVMMain);
			log.debug("ctxm: main");
		}
	};

	public final Command MenuParamsClick = new Command(){
		@Override
		public void Invoke(View view, Object... args) {
			rightMenuVM.set(rightMenuVMParams);
			log.debug("ctxm: params");
		}
	};
	
	public final Command MenuGraphicClick = new Command(){
		@Override
		public void Invoke(View view, Object... args) {
			log.debug("ctxm: graphics");
		}
	};
	
	public final Command MenuScreenClick = new Command(){
		@Override
		public void Invoke(View view, Object... args) {
			log.debug("ctxm: screen");
		}
	};

}
