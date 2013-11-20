package my.app.myvi;

import my.app.myvi.vm.MainVM;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import android.os.Bundle;

import com.google.inject.Inject;

public class MyviMainActivity extends MyRoboActivity {
	
	private final Logger log = LoggerFactory.getLogger(MyviMainActivity.class);
	
	@Inject
	private MainVM mainVM;
	
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        this.inflateAndBind(R.xml.main_meta, mainVM);
    }


}
