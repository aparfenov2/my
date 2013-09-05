package my.app.myvi.vm;

import java.util.Collection;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import gueei.binding.Command;
import gueei.binding.IObservable;
import gueei.binding.Observable;
import gueei.binding.Observer;
import gueei.binding.observables.IntegerObservable;
import gueei.binding.observables.StringObservable;
import my.app.myvi.MenuButton;
import my.app.myvi.R;
import android.app.Activity;
import android.os.Handler;
import android.view.View;

public class MainVM {
	
	private final Logger log = LoggerFactory.getLogger(MainVM.class);
	
	private Activity mContext;
	
	public MainVM(Activity mContext) {
		this.mContext = mContext;
		
		rightMenuVM.subscribe(new Observer() {

			@Override
			public void onPropertyChanged(IObservable<?> arg0,
					Collection<Object> arg1) {
				
				new Handler().postDelayed(new Runnable() {

					@Override
					public void run() {
						rightMenuName.set(rightMenuVM.get().getName());
						rightMenuId.set(rightMenuVM.get().getLayout());
						
					}

				}, 1);
			}
			
		});
		rightMenuVM.set(rightMenuVMMain);
	}
// Right Menu	
	public Observable<RightMenuVMBase> rightMenuVM = new Observable<RightMenuVMBase>(RightMenuVMBase.class);
	public IntegerObservable rightMenuId = new IntegerObservable(R.layout.right_menu_main);
	public StringObservable rightMenuName = new StringObservable("xxx");
	
	public RightMenuVMMain rightMenuVMMain = new RightMenuVMMain();
	public RightMenuVMParams rightMenuVMParams = new RightMenuVMParams();
// Top Menu	
	public TopCh50VM topCh50VM = new TopCh50VM();
	public TopFSendVM topFSendVM = new TopFSendVM();
	public TopFRecVM topFRecVM = new TopFRecVM();
	public TopDlySendVM topDlySendVM = new TopDlySendVM();
	public TopDlyRecVM topDlyRecVM = new TopDlyRecVM();
	
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
