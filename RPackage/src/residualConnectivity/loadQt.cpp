#include <QApplication>
#include <Rcpp.h>
extern "C"
{
	Q_DECL_EXPORT SEXP loadQT()
	{
		QCoreApplication::addLibraryPath(QString("."));
		//only needs to be called once
		static bool called = false;
		if(called) return R_NilValue;

		char* empty = new char[1];
		*empty = 0;
		char* argv[] = {empty};

		int argc = 0;
		//We make an attempt here to tripper the QT runtime, at which point it will load up all the required plugins, while the 
		//working directory is correctly set. 
		QApplication app(argc, argv);
		delete[] empty;
		return R_NilValue;
	}
}