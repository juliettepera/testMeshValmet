#include "testMeshValmet.h"

int main( int argc , char* argv[] )
{

      QApplication app( argc , argv );

      QWidget * parent = 0;
      Qt::WFlags f = 0;

      testMeshValmet mytestMeshValmet( parent , f );
      mytestMeshValmet.show();

      return app.exec();

}
