package com.beloko.quake2touch;

public class QuakeGameMod {
	String title;
	String directory;
	String args = "";
	int gameDLL;
	int image;

	public boolean selected; //Just used for listview
	
	public int getGameDLL() {
		return gameDLL;
	}


	public void setGameDLL(int gameDLL) {
		this.gameDLL = gameDLL;
	}

	
	public int getImage() {
		return image;
	}


	public void setImage(int image) {
		this.image = image;
	}


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
