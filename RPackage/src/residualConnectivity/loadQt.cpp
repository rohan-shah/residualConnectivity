#ifdef BUILD_VISUALISATION
	#include <QApplication>
#endif
#include <Rcpp.h>
RcppExport SEXP loadQt()
{
#ifdef BUILD_VISUALISATION
	QCoreApplication::addLibraryPath(QString("."));
	//only needs to be called once
	static bool called = false;
	if(called) return R_NilValue;

	char* empty = new char[1];
	*empty = 0;
	char* argv[] = {empty};

	int argc = 0;
	//We make an attempt here to load the QT runtime, at which point it will load up all the required plugins, while the 
	//working directory is correctly set. 
	QApplication app(argc, argv);
	delete[] empty;
#endif
	return R_NilValue;
}
