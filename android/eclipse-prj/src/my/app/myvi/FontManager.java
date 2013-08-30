package my.app.myvi;

import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.Typeface;
import android.util.AttributeSet;
import android.widget.TextView;

public class FontManager {
    // ** This enum is tightly coupled with the enum in res/values/attrs.xml! **
    // ** Make sure their orders stay the same **

    public static void setFont(TextView tv, String font) {
        if (font.equals("default") || tv.isInEditMode()) {
            tv.setTypeface(Typeface.DEFAULT);
        } else {
            Typeface typeface = Typeface.createFromAsset(tv.getContext().getAssets(), "fonts/" + font);
            tv.setTypeface(typeface);
        }
    }

    public static void setFontFromAttributeSet(Context context, AttributeSet attrs, TextView tv) {
        
        TypedArray a = context.obtainStyledAttributes(attrs, R.styleable.CustTextView);
        
        String fontId = null;
        for (int i = 0; i < a.getIndexCount(); i++) {
            int attr = a.getIndex(i);
            if (attr == R.styleable.CustTextView_font) {
                fontId = a.getString(attr);
            }
        }

        if (fontId != null) {
            FontManager.setFont(tv, fontId);
        }
        
        a.recycle();
    }
}