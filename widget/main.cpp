#include <iostream>
#include <memory>
#include <QApplication>
#include "db_widget.h"

#include "qvariant_cvt.hpp"

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);

	DBWidget w;
	w.show();
	//if new need do : w->setAttribute(Qt::WA_DeleteOnClose); 

    app.exec();

	return 0;
}