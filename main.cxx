#include "MeshValmetControls.h"

int main( int argc , char* argv[] )
{

      QApplication app( argc , argv );

      QWidget * parent = 0;
      const char* name = "essai";

      MeshValmetControls myMeshValmet( parent , name );

      //myMeshValmet.show();

      return app.exec();

}
