package my.app.myvi;

import gueei.binding.v30.app.BindingActivityV30;
import vm.MainVM;
import android.os.Bundle;

public class MyviMainActivity extends BindingActivityV30 {
	


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        MainVM vm = new MainVM(this);
        this.inflateAndBind(R.xml.main_meta, vm);
        
//        setContentView(R.layout.myvi_main);
    }

//    @Override
//    public boolean onCreateOptionsMenu(Menu menu) {
//        // Inflate the menu; this adds items to the action bar if it is present.
//        getMenuInflater().inflate(R.menu.myvi_main, menu);
//        return true;
//    }

}
