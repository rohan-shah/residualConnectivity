.onLoad <- function(libname, pkgname)
{
	couldLoad <- try(library.dynam(package="residualConnectivity", chname="residualConnectivity", lib.loc = .libPaths()), silent=TRUE)
	wd <- getwd()
	if(class(couldLoad) != "try-error")
	{
		setwd(dirname(couldLoad[["path"]]))
		.Call("loadQT", PACKAGE="residualConnectivity")
		setwd(wd)
	}
}
