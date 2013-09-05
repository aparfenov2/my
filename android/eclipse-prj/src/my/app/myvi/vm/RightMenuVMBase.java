package my.app.myvi.vm;

public class RightMenuVMBase {
	private String name;
	private int layout;

	public RightMenuVMBase(String name, int layout) {
		this.setName(name);
		this.setLayout(layout);
	}

	public String getName() {
		return name;
	}

	public void setName(String name) {
		this.name = name;
	}

	public int getLayout() {
		return layout;
	}

	public void setLayout(int layout) {
		this.layout = layout;
	}
}
