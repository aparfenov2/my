package my.app.myvi.vm;

public class RightMenuVMBase {
	
	public String name;
	public int layout;
	

	public RightMenuVMBase(String name, int layout) {
		this.name = name;
		this.layout = layout;
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
