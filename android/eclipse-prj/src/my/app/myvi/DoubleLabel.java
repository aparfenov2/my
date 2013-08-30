package my.app.myvi;

import android.content.Context;
import android.content.res.TypedArray;
import android.util.AttributeSet;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

public class DoubleLabel extends StyleableWidget {
 
    public DoubleLabel(final Context context, AttributeSet attrs) {
        super(context, attrs);
        
        LayoutInflater inflater = LayoutInflater.from(context);
        View inflated = inflater.inflate(R.layout.double_label, this,true);
        
        if (inflated == null) {
            Button bt = new Button(context);
            bt.setText("Dummy");
            this.addView(bt);
            return;
        }
 
        TypedArray a = getContext().obtainStyledAttributes(attrs,  R.styleable.MenuButton);
        
        TextView ltop = (TextView) this.findViewById(R.id.ltop);
        TextView lbot = (TextView) this.findViewById(R.id.lbot);
        
        applyAttrs(ltop, a, R.styleable.MenuButton_topText, R.styleable.MenuButton_topStyle, R.styleable.MenuButton_topSize );
        applyAttrs(lbot, a, R.styleable.MenuButton_botText, R.styleable.MenuButton_botStyle, R.styleable.MenuButton_botSize );
        
        a.recycle();
        
       
        
    }
    
}
