package my.app.myvi;

import gueei.binding.IBindableView;
import gueei.binding.ViewAttribute;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import android.content.Context;
import android.content.res.TypedArray;
import android.util.AttributeSet;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

public class MenuButton extends StyleableWidget implements
		IBindableView<MenuButton> {
	
	private final Logger log = LoggerFactory.getLogger(MenuButton.class);

	public MenuButton(final Context context, AttributeSet attrs) {
		super(context, attrs);

		LayoutInflater inflater = LayoutInflater.from(context);
		View inflated = inflater.inflate(R.layout.menu_btn, this, true);

		if (inflated == null) {
			Button bt = new Button(context);
			bt.setText("Dummy");
			this.addView(bt);
			return;
		}

		TypedArray a = getContext().obtainStyledAttributes(attrs,
				R.styleable.MenuButton);

		TextView ltop = (TextView) this.findViewById(R.id.ltop);
		TextView lmid = (TextView) this.findViewById(R.id.lmid);
		TextView lbot = (TextView) this.findViewById(R.id.lbot);

		applyAttrs(ltop, a, R.styleable.MenuButton_topText,
				R.styleable.MenuButton_topStyle, R.styleable.MenuButton_topSize);
		applyAttrs(lmid, a, R.styleable.MenuButton_midText,
				R.styleable.MenuButton_midStyle, R.styleable.MenuButton_midSize);
		applyAttrs(lbot, a, R.styleable.MenuButton_botText,
				R.styleable.MenuButton_botStyle, R.styleable.MenuButton_botSize);

		a.recycle();
		
		this.setFocusable(true);
		this.setFocusableInTouchMode(true);
		this.setClickable(true);
		

	}
	

	private ViewAttribute<MenuButton, CharSequence> topTextAttribute = new ViewAttribute<MenuButton, CharSequence>(
			CharSequence.class, MenuButton.this, "topText") {
		@Override
		protected void doSetAttributeValue(Object newValue) {
			TextView ltop = (TextView) MenuButton.this.findViewById(R.id.ltop);
			ltop.setText((CharSequence) newValue);
		}

		@Override
		public CharSequence get() {
			TextView ltop = (TextView) MenuButton.this.findViewById(R.id.ltop);
			return ltop.getText();
		}
	};
	
	private ViewAttribute<MenuButton, CharSequence> midTextAttribute = new ViewAttribute<MenuButton, CharSequence>(
			CharSequence.class, MenuButton.this, "midText") {
		@Override
		protected void doSetAttributeValue(Object newValue) {
			TextView lmid = (TextView) MenuButton.this.findViewById(R.id.lmid);
			lmid.setText((CharSequence) newValue);
		}

		@Override
		public CharSequence get() {
			TextView lmid = (TextView) MenuButton.this.findViewById(R.id.lmid);
			return lmid.getText();
		}
	};
	

	private ViewAttribute<MenuButton, CharSequence> botTextAttribute = new ViewAttribute<MenuButton, CharSequence>(
			CharSequence.class, MenuButton.this, "botText") {
		@Override
		protected void doSetAttributeValue(Object newValue) {
			TextView lbot = (TextView) MenuButton.this.findViewById(R.id.lbot);
			lbot.setText((CharSequence) newValue);
		}

		@Override
		public CharSequence get() {
			TextView lbot = (TextView) MenuButton.this.findViewById(R.id.lbot);
			return lbot.getText();
		}
	};
	
	
	
	@Override
	public ViewAttribute<? extends View, ?> createViewAttribute(
			String attributeName) {

		if (attributeName.equals("topText")) {
			return topTextAttribute;
		} else if (attributeName.equals("midText")) {
			return midTextAttribute;
		} else if (attributeName.equals("botText")) {
			return botTextAttribute;
		}
		return null;
	}

}
