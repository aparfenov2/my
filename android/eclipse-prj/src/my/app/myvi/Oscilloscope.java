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

public class Oscilloscope extends View {
    
    private Handler hdl = new Handler();

    private final int FRAME_RATE = 30;
    
    
    public Oscilloscope(Context context, AttributeSet attrs) {
        super(context, attrs);        
    }
    
    private Runnable r = new Runnable() {
        @Override
        public void run() {
                invalidate();
        }
    };
    
    Paint p = new Paint();
    Path ph = new Path();
    
    float fi0;
    
    @Override
    protected void onDraw(Canvas c) {
        super.onDraw(c);
        
        c.drawColor(Color.parseColor("#292929"));

        p.setColor(Color.WHITE); 
        p.setStyle(Style.STROKE);
        
        float h = c.getHeight();
        float w = c.getWidth();
        
// рисуем сетку
        float cw = 50;
        float ch = cw;
        
        ph.reset();        
        for (int col=1; col*cw < w; col++) {
            ph.moveTo(col*cw, 0);
            ph.lineTo(col*cw, h);
        }
        for (int r=1; r*ch < h; r++) {
            ph.moveTo(0, r*ch);
            ph.lineTo(w, r*ch);
        }
        c.drawPath(ph, p);
// рисуем синусоиду        
        
        
        float x1 = 0;
        float y1 = h/2;
        
        float l = 150;
        float step = 10;
        
        fi0 += 10*Math.PI/180.;
        float fi = fi0;
        
        ph.reset();
        ph.moveTo(x1,y1);
        float y2 = y1;
        
        while (x1 < w) {
            y2 = y1 + l * FloatMath.sin(fi);
            ph.lineTo(x1, y2);
            x1 += step;
            fi += (4*Math.PI * step)/w;
        }
        c.drawPath(ph, p);
        
        hdl.postDelayed(r, FRAME_RATE);
    }
}
