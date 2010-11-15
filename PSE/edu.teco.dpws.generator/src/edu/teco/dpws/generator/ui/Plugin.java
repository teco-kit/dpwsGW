package edu.teco.dpws.generator.ui;

import org.eclipse.ui.plugin.AbstractUIPlugin;
import org.osgi.framework.BundleContext;


public class Plugin extends AbstractUIPlugin {
	 private static Plugin plugin;
	 public static final String PLUGIN_ID = "edu.teco.dpws.generator";

	 
	 public static Plugin getDefault() { return plugin;}
	 
	 public void start(BundleContext context) throws Exception {
          super.start(context);
          plugin = this;
  }

    public void stop(BundleContext context) throws Exception {
          plugin = null;
          super.stop(context);
  }

}
