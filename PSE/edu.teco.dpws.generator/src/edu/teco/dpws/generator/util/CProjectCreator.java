package edu.teco.dpws.generator.util;

import java.io.File;
import java.io.IOException;
import java.net.URL;
import java.util.HashSet;
import java.util.LinkedHashSet;
import java.util.Set;

import org.eclipse.cdt.core.CCorePlugin;
import org.eclipse.cdt.core.ICDescriptor;
import org.eclipse.cdt.managedbuilder.core.BuildException;
import org.eclipse.cdt.managedbuilder.core.IConfiguration;
import org.eclipse.cdt.managedbuilder.core.IManagedBuildInfo;
import org.eclipse.cdt.managedbuilder.core.IManagedProject;
import org.eclipse.cdt.managedbuilder.core.IOption;
import org.eclipse.cdt.managedbuilder.core.IProjectType;
import org.eclipse.cdt.managedbuilder.core.ITool;
import org.eclipse.cdt.managedbuilder.core.ManagedBuildManager;
import org.eclipse.cdt.managedbuilder.core.ManagedBuilderCorePlugin;
import org.eclipse.cdt.managedbuilder.core.ManagedCProjectNature;
import org.eclipse.core.resources.IFolder;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IWorkspace;
import org.eclipse.core.resources.IWorkspaceDescription;
import org.eclipse.core.resources.IWorkspaceRoot;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.FileLocator;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.Path;
import org.eclipse.emf.mwe.core.resources.ResourceLoaderFactory;
import org.eclipse.emf.mwe.utils.StandaloneSetup;

@SuppressWarnings("deprecation")
public class CProjectCreator extends
		org.eclipse.emf.mwe.core.lib.AbstractWorkflowComponent2 {

	String prjName;

	public void setPrjName(final String name) {
		this.prjName = name;
	}

	@Override
	public String getLogMessage() {
		return "creating C Project '" + prjName + "'";
	}

	@Override
	protected void invokeInternal(
			final org.eclipse.emf.mwe.core.WorkflowContext model,
			final org.eclipse.emf.mwe.core.monitor.ProgressMonitor monitor,
			final org.eclipse.emf.mwe.core.issues.Issues issues) {
		if (ResourcesPlugin.getPlugin() != null)
			try {
				createManagedProject(prjName);
			} catch (BuildException e) {
				issues.addError(this, e.getMessage(), this, e, null);
			}
	}

	@Override
	protected void checkConfigurationInternal(
			final org.eclipse.emf.mwe.core.issues.Issues issues) {
		if (prjName == null) {
			issues.addError(this, "No project specified!");
			return;
		}
		if (ResourcesPlugin.getPlugin() == null) {
			issues.addWarning(this, "Cannot access eclipse Workspace");
			if (!(new File( StandaloneSetup.getPlatformRootPath() +File.separator+ prjName + File.separator+ ".cproject").exists()))
				issues.addError(this, "CDT project "+ StandaloneSetup.getPlatformRootPath() +File.separator+ prjName + " not found");
		}
		
		for(IOException e: pathErrors)
		{
			issues.addWarning(this, e.toString(),null, e, null);
		}
	}
	
	public static String getFilePath(String path) throws  IOException
	{
		URL url=ResourceLoaderFactory.createResourceLoader().getResource(path);
		if(url==null) return path;
		try{
			URL newURL=FileLocator.toFileURL(url);
			url=newURL;
		}
		catch(NullPointerException e)
		{
		}
		String pathStr=new File(url.getPath()).getCanonicalPath();
		pathStr=new Path(pathStr).toOSString();
		if(!pathStr.startsWith("\"") && pathStr.contains(" "))
			pathStr="\""+pathStr+"\"";		
//		pathStr=new Path(pathStr).toPortableString().replaceAll(" ", "\\\\ "); //This effectively replaces backspaces on windows (works with gcc) and protects spaces
		return pathStr;
	}
	
	Set<IOException> pathErrors=new LinkedHashSet<IOException> ();
	
	private String projTypeStr="cdt.managedbuild.target.gnu.lib";
	
	public void setProjType(String projType) {
		this.projTypeStr = projType;
	}

	private Set<String> includeDirs=new LinkedHashSet<String>();
	public void addIncludeDir(String path)
	{    
		try {
			includeDirs.add(getFilePath(path));
		} catch (IOException e) {
			pathErrors.add(e);
		}
	}
    
	private Set<String> libDirs=new LinkedHashSet<String>();
	public void addLibDir(String path)
	{
		try {
			libDirs.add(getFilePath(path));
		} catch (IOException e) {
			pathErrors.add(e);
		}
	}
	
	private Set<String> libs=new LinkedHashSet<String>();
	
	public void addLib(String path)
	{
		libs.add(path);
	}
	
	private Set<String> defines=new LinkedHashSet<String>();
	
	public void addDefine(String symbol)
	{
		defines.add(symbol);
	}
	
	private Set<String> undefs=new LinkedHashSet<String>();
	
	public void addUndef(String symbol)
	{
		undefs.add(symbol);
	}
	
	public static class Link 
	{
		
		private String name=null;
		private String target=null;
	
		public String getName() {
			return name;
		}
		public void setName(String name) {
			this.name = name;
		}
		public String getTarget() {
			return target;
		}
		public void setTarget(String target){
			this.target = target;
		}
		

	}

	private Set<Link> links=new HashSet<Link>();
	
	public void addLink(Link link) 
	{
		links.add(link);
	}
	
	IProject proj = null;
	IManagedProject mproj = null;

	@SuppressWarnings("deprecation")
	void createManagedProject(String name) throws BuildException {

		IWorkspaceRoot root = ResourcesPlugin.getWorkspace().getRoot();
		proj = root.getProject(name);

		if (proj.exists() && null!=ManagedBuildManager.getBuildInfo(proj)) {
			mproj = ManagedBuildManager.getBuildInfo(proj).getManagedProject();
		} else {
			IWorkspace workspace = ResourcesPlugin.getWorkspace();
			IWorkspaceDescription workspaceDesc = workspace.getDescription();
			workspaceDesc.setAutoBuilding(false);
			try {
				workspace.setDescription(workspaceDesc);
				proj = CCorePlugin.getDefault().createCProject(
						workspace.newProjectDescription(proj.getName()), proj,
						new org.eclipse.core.runtime.NullProgressMonitor(),
						ManagedBuilderCorePlugin.MANAGED_MAKE_PROJECT_ID);

				// add ManagedBuildNature
				IManagedBuildInfo info = ManagedBuildManager
						.createBuildInfo(proj);
				info.setValid(true);
				
				
				ManagedCProjectNature.addManagedNature(proj, null);
				ManagedCProjectNature.addManagedBuilder(proj, null);

				for(Link l:links)
				{
				   IPath target = new Path(l.getTarget());
				   IFolder link = proj.getFolder(l.getName());
				   
				   try{
				  
					if (workspace.validateLinkLocation(link,target).isOK()) {
					      link.createLink(target, IResource.NONE, null);
					} else {
				      throw new BuildException(workspace.validateLinkLocation(link,target).getMessage());			      
					   }
				   }catch (BuildException e) {
					   e.printStackTrace();
					   	//invalid location, throw an exception or warn user??
				}
				}

				
			 ICDescriptor desc =
				  CCorePlugin.getDefault().getCProjectDescription(proj, true);
				  desc.remove(CCorePlugin.BUILD_SCANNER_INFO_UNIQ_ID);
				  desc.create(CCorePlugin.BUILD_SCANNER_INFO_UNIQ_ID,
				  ManagedBuildManager.INTERFACE_IDENTITY);
		     desc.saveProjectData();
				 				
					
			} catch (CoreException e) {
				throw new BuildException(e.getMessage());
			} 

			if (ManagedBuildManager.getDefinedProjectTypes() == null)
				; // Call this function just to avoid init problems in
					// getProjectType();
			IProjectType projType = ManagedBuildManager
					.getProjectType(projTypeStr); //$NON-NLS-1$
			
			if (projType != null) {		

			 {
				mproj = ManagedBuildManager
						.createManagedProject(proj, projType);
				
				IConfiguration activeConfig = ManagedBuildManager.getBuildInfo(
						proj).getDefaultConfiguration();
				
				if(activeConfig!=null)
				{
				  activeConfig.setManagedBuildOn(true);
				}
				
				ManagedBuildManager.saveBuildInfo(proj, true); //TODO: unable to apply the invalid Project description;
			} 
			
			}
			
			ManagedBuildManager.setNewProjectVersion(proj);

			IConfiguration[] cfgs = projType.getConfigurations();
			IConfiguration defcfg = cfgs.length > 0 ? mproj
					.createConfiguration(cfgs[0], projType.getId() + ".0") : null; //$NON-NLS-1$

			for (int i = 1; i < cfgs.length; ++i) { // sic ! from 1
				mproj.createConfiguration(cfgs[i], projType.getId() + "." + i); //$NON-NLS-1$
			}
		
			
			addOptionValues(defcfg, defcfg.getToolFromInputExtension("c"), "gnu.c.compiler.option.include.paths", includeDirs);
			addOptionValues(defcfg, defcfg.getToolFromInputExtension("c"), "gnu.c.compiler.option.preprocessor.def.symbols", defines);
			addOptionValues(defcfg, defcfg.getToolFromInputExtension("c"), "gnu.c.compiler.option.preprocessor.undef.symbols", undefs);
			
			addOptionValues(defcfg, defcfg.getToolFromInputExtension("o"), "gnu.c.link.option.paths", libDirs);
			addOptionValues(defcfg, defcfg.getToolFromInputExtension("o"), "gnu.c.link.option.libs", libs);
			
			
			ManagedBuildManager.setDefaultConfiguration(proj, defcfg);
		}
		// open project w/o progress monitor; no action performed if it's opened
		try {
			proj.open(null);
		} catch (CoreException e) {
		}
	}
	
	
	void addOptionValues(IConfiguration cf, ITool cfTool, String optID,  Set<String> values)
	{
		if(values==null || values.size()==0) return;
		
		IOption option = cfTool.getOptionById(optID); 
		String[] oldVals=option.getApplicableValues();
		String[] newVals=new String[oldVals.length+values.size()];
		
		System.arraycopy(oldVals, 0, newVals, 0, oldVals.length);
		System.arraycopy(values.toArray(), 0, newVals, oldVals.length, values.size());
		
		ManagedBuildManager.setOption(cf, cfTool, option, newVals);
	}

	
	
}
