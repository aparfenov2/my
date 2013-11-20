package my.app.myvi;

import android.content.Context;
import android.util.AttributeSet;
import android.widget.TextView;
 
public class CustTextView extends TextView {
 
    public CustTextView(Context context, AttributeSet attrs) {
        super(context, attrs);
        FontManager.setFontFromAttributeSet(context, attrs, this);        
    }
    
    
    
}