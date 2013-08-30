package my.app.myvi;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Paint.Style;
import android.graphics.Path;
import android.os.Handler;
import android.util.AttributeSet;
import android.util.FloatMath;
import android.view.View;

public class BatteryMeter extends View {
    
    public BatteryMeter(Context context, AttributeSet attrs) {
        super(context, attrs);        
    }
    
    
    Paint p = new Paint();
    
    
    @Override
    protected void onDraw(Canvas c) {
        super.onDraw(c);
        
        c.drawColor(Color.parseColor("#292929"));

        p.setColor(Color.WHITE); 
        p.setStyle(Style.FILL);
        
        float h = c.getHeight();
        float w = c.getWidth();
        
        w =  0.7f * w;
        
        c.drawRect(0,0,w,h, p);
    }
}
