package com.beloko.idtech;

import java.io.Serializable;

public class QuakeQuickCommand implements Serializable{
	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;
	
	String title;
	String command;
	
	QuakeQuickCommand(String title, String command)
	{
		this.title = title;
		this.command = command;
	}

	public String getTitle() {
		return title;
	}

	public void setTitle(String title) {
		this.title = title;
	}

	public String getCommand() {
		return command;
	}

	public void setCommand(String command) {
		this.command = command;
	}
	
}
