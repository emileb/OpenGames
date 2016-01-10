package com.beloko.quaketouch;

public class QuakeGameMod {
	String title;
	String directory;
	String args = "";
	

	public boolean selected; //Just used for listview
	
	QuakeGameMod(String title,String directory)
	{
		this.title = title;
		this.directory = directory;
	}


	public String getTitle() {
		return title;
	}


	public void setTitle(String title) {
		this.title = title;
	}


	public String getDirectory() {
		return directory;
	}


	public void setDirectory(String directory) {
		this.directory = directory;
	}


	public String getArgs() {
		return args;
	}


	public void setArgs(String args) {
		this.args = args;
	}
	
	
}
