package edu.teco.dpws.generator.ui;

import java.io.Serializable;

import org.eclipse.core.runtime.ILog;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;

public class Log implements org.apache.commons.logging.Log, Serializable
{
	private static final long serialVersionUID = 9181448189753075665L;
	
	
	private String sClassName;

	public Log(String sName) { sClassName = sName; }
	
	public Log() { }
		
	public String getClassName() { return sClassName; }
	
	private static ILog Logger=null;
	
	static void setLog(ILog l)
	{
		Logger=l;
	}

	public void error(Object obj) {
		Logger.log(new Status(IStatus.ERROR,     Plugin.PLUGIN_ID, 0, obj.toString(),null));
	}

	public void error(Object obj, Throwable throwable) {
		Logger.log(new Status(IStatus.ERROR,     Plugin.PLUGIN_ID, 0, (obj!=null?obj.toString():(throwable!=null?throwable.getLocalizedMessage():"null")),throwable));
		
	}

	public void fatal(Object obj) {
		Logger.log(new Status(IStatus.ERROR,     Plugin.PLUGIN_ID, 0, (obj!=null?obj.toString():"null"),null));
		
	}

	public void fatal(Object obj, Throwable throwable) {
		Logger.log(new Status(IStatus.ERROR,     Plugin.PLUGIN_ID, 0,(obj!=null?obj.toString():(throwable!=null?throwable.getLocalizedMessage():"null")),throwable));
		
	}

	public void info(Object obj) {
		Logger.log(new Status(IStatus.INFO,     Plugin.PLUGIN_ID, 0, (obj!=null?obj.toString():"null"),null));
		
	}
	
	public void info(Object obj, Throwable throwable) {
		Logger.log(new Status(IStatus.INFO,     Plugin.PLUGIN_ID, 0,(obj!=null?obj.toString():(throwable!=null?throwable.getLocalizedMessage():"null")),throwable));
		
	}
	

	public void warn(Object obj) {
		Logger.log(new Status(IStatus.WARNING,     Plugin.PLUGIN_ID, 0, (obj!=null?obj.toString():"null"),null));
		
	}

	public void warn(Object obj, Throwable throwable) {
		Logger.log(new Status(IStatus.WARNING,     Plugin.PLUGIN_ID, 0, (obj!=null?obj.toString():(throwable!=null?throwable.getLocalizedMessage():"null")),throwable));
		
	}

	public boolean isDebugEnabled() {
		// TODO Auto-generated method stub
		return false;
	}

	public boolean isErrorEnabled() {
		// TODO Auto-generated method stub
		return true;
	}

	public boolean isFatalEnabled() {
		// TODO Auto-generated method stub
		return true;
	}

	public boolean isInfoEnabled() {
		// TODO Auto-generated method stub
		return true;
	}

	public boolean isTraceEnabled() {
		// TODO Auto-generated method stub
		return false;
	}

	public boolean isWarnEnabled() {
		// TODO Auto-generated method stub
		return true;
	}

	public void trace(Object obj) {
		// TODO Auto-generated method stub
		
	}

	public void trace(Object obj, Throwable throwable) {
		// TODO Auto-generated method stub
		
	}
	

	public void debug(Object obj) {
		// TODO Auto-generated method stub
		
	}

	public void debug(Object obj, Throwable throwable) {
		// TODO Auto-generated method stub
		
	}

		




}
