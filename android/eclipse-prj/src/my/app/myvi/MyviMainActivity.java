package my.app.myvi;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import my.app.myvi.vm.MainVM;
import gueei.binding.Command;
import gueei.binding.v30.actionbar.BindableActionBar;
import gueei.binding.v30.app.BindingActivityV30;
import android.os.Bundle;
import android.view.View;

public class MyviMainActivity extends BindingActivityV30 {
	
	private final Logger log = LoggerFactory.getLogger(MyviMainActivity.class);


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        MainVM vm = new MainVM(this);
        this.inflateAndBind(R.xml.main_meta, vm);
    }


}
