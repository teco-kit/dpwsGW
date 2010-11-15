package edu.teco.dpws.generator.ui;

import java.io.File;
import java.io.IOException;
import java.net.URL;
import java.util.HashMap;
import java.util.Map;

import org.eclipse.core.commands.AbstractHandler;
import org.eclipse.core.commands.ExecutionEvent;
import org.eclipse.core.commands.ExecutionException;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IPathVariableManager;
import org.eclipse.core.resources.IWorkspace;
import org.eclipse.core.resources.ResourcesPlugin;
//import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.FileLocator;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.jobs.Job;

import org.eclipse.emf.mwe.core.WorkflowRunner;
import org.eclipse.emf.mwe.core.monitor.ProgressMonitorAdapter;
import org.eclipse.emf.mwe.core.resources.ResourceLoaderFactory;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.ui.IActionDelegate;
import org.eclipse.ui.IObjectActionDelegate;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.handlers.HandlerUtil;

public class WRunner extends AbstractHandler implements IActionDelegate {

	IFile selectedFile = null;
	IWorkbenchPart part;
	String baseName;
	String dirName;
	String workspaceURL;

	String wfFile = "wsdl.oaw";

	/**
	 * Constructor for Action1.
	 */
	public WRunner() {
		super();
	}

	/**
	 * @see IObjectActionDelegate#setActivePart(IAction, IWorkbenchPart)
	 */
	public void setActivePart(IAction action, IWorkbenchPart targetPart) {
		targetPart.getSite().getShell();
		this.part = targetPart;
	}

	/**
	 * @see IActionDelegate#run(IAction)
	 */
	public void run(IAction action) {
		run();
	}

	@SuppressWarnings("unchecked")
	public void run() {

		final Map propertiesSchema = new HashMap();

		propertiesSchema.put("dirName", dirName);
		propertiesSchema.put("baseName", baseName);
		propertiesSchema.put("platformURI", workspaceURL);
		Plugin.getDefault().getLog().log(
				new Status(IStatus.INFO, Plugin.PLUGIN_ID, 0, "Generating "
						+ baseName, null));

		Log.setLog(Plugin.getDefault().getLog());

		Job job = new Job("Generating code for " + baseName) {
			protected IStatus run(IProgressMonitor monitor) {

				Thread.currentThread().setContextClassLoader(
						WRunner.class.getClassLoader());
				System.setProperty("org.apache.commons.logging.Log", Log.class
						.getName());
				return new WorkflowRunner().run(wfFile,
						new ProgressMonitorAdapter(monitor), propertiesSchema,
						null) ? Status.OK_STATUS : Status.CANCEL_STATUS;

			}
		};
		job.setPriority(Job.BUILD);
		job.schedule();
	}

	/**
	 * @see IActionDelegate#selectionChanged(IAction, ISelection)
	 */

	private void setVars(IStructuredSelection selection) {
		IFile f = (IFile) selection.getFirstElement();
		baseName = f.getLocation().removeFileExtension().lastSegment();
		wfFile = f.getLocation().getFileExtension() + ".oaw";
		dirName = f.getLocation().removeLastSegments(1).toPortableString();
		workspaceURL = ResourcesPlugin.getWorkspace().getRoot().getLocation()
				.toPortableString();
		IWorkspace workspace = ResourcesPlugin.getWorkspace();
		IPathVariableManager pathMan = workspace.getPathVariableManager();

		URL url = ResourceLoaderFactory.createResourceLoader().getResource(
				"shared");
		try {
			try {
				URL newURL = FileLocator.toFileURL(url);
				url = newURL;
			} catch (NullPointerException e) {
			}

			String pathStr = new File(url.getPath()).getAbsolutePath();
			IPath value = new Path(pathStr);

			String name = Plugin.PLUGIN_ID.replaceAll("\\.", "_").toUpperCase()
					+ "_SHARED";

			if (pathMan.validateName(name).isOK()
					&& pathMan.validateValue(value).isOK()) {
				pathMan.setValue(name, value);
			}
			else
			{
				throw new IOException("\""+name + "\":" +pathMan.validateName(name).getMessage()+"\n"+"\""+value + "\":" +pathMan.validateValue(value).getMessage());
			}
		} catch (Exception e) {
			Plugin.getDefault().getLog().log(new Status(IStatus.WARNING, Plugin.PLUGIN_ID, 0, "Failed to set shared source folder", e));
		} 
	}

	public void selectionChanged(IAction action, ISelection selection) {

		if (selection instanceof IStructuredSelection) {
			setVars((IStructuredSelection) selection);
		}
	}

	public Object execute(ExecutionEvent event) throws ExecutionException {
		ISelection selection = HandlerUtil.getCurrentSelection(event);
		if (selection instanceof IStructuredSelection) {
			setVars((IStructuredSelection) selection);
			run();
		}
		return null;
	}
}
