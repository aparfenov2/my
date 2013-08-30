package my.app.myvi;

import android.content.Context;
import android.content.res.TypedArray;
import android.util.AttributeSet;
import android.util.TypedValue;
import android.widget.LinearLayout;
import android.widget.TextView;

public class StyleableWidget extends LinearLayout {

	public StyleableWidget(Context context) {
		super(context);
	}

	public StyleableWidget(Context context, AttributeSet attrs) {
		super(context, attrs);
	}

	public StyleableWidget(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
	}

	private void applyTextStyle(TextView tv, int styleId) {
	     
	    tv.setTextAppearance(getContext(), styleId);
	    
	    TypedArray a = getContext().getTheme().obtainStyledAttributes(styleId, R.styleable.CustTextView ); //new int[] {R.attr.font}
	    String font = a.getString(R.styleable.CustTextView_font);
	    if (font != null)
	        FontManager.setFont(tv, font);
	    a.recycle();
	}

	protected void applyAttrs(TextView tv, TypedArray a, int textId,
			int styleId, int sizeId) {
			    if (tv == null) return;
			    
			    String s = a.getString(textId);
			    if (s != null && !s.isEmpty()) {
			        tv.setText(s);
			    } else {
			        tv.setVisibility(GONE);
			        return;
			    }
			    
			    int res = a.getResourceId(styleId, -1);
			    if (res != -1) {
			        applyTextStyle(tv, res);
			    }
			    
			    int size = a.getDimensionPixelSize(sizeId, -1);			    
			    if (size != -1) {
			        tv.setTextSize(TypedValue.COMPLEX_UNIT_PX, size);
			    }
			}

}