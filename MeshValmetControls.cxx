/* Author: Christine Xu, University of North Carolina at Chapel Hill*/

#include "MeshValmetControls.h"


/* Constructor and destructor */
MeshValmetControls::MeshValmetControls( QWidget * parent , const char* name ) : MeshValmetGUI( parent , name )
{

  m_CommonPath = "";
  m_FileName1 = "";
  m_FileName2 = "";

  m_Model1Line = 0;
  m_Model2Line = 0;
  
  m_Renderer1 = vtkSmartPointer <vtkRenderer> ::New();
  m_Renderer2 = vtkSmartPointer <vtkRenderer> ::New();
  m_Renderer = vtkSmartPointer <vtkRenderer> ::New();

  m_Iren = vtkSmartPointer <vtkRenderWindowInteractor> ::New();
  m_Iren1 = vtkSmartPointer <vtkRenderWindowInteractor> ::New();
  m_Iren2 = vtkSmartPointer <vtkRenderWindowInteractor> ::New();

  vtkSmartPointer <vtkInteractorStyleSwitch> trackBall1 = vtkSmartPointer <vtkInteractorStyleSwitch> ::New();
  trackBall1 -> SetCurrentStyleToTrackballCamera();
  m_Iren1 -> SetInteractorStyle( trackBall1 );

  vtkSmartPointer <vtkInteractorStyleSwitch> trackBall2 = vtkSmartPointer <vtkInteractorStyleSwitch> ::New();
  trackBall2 -> SetCurrentStyleToTrackballCamera();
  m_Iren2 -> SetInteractorStyle( trackBall2 );

  m_Iren1 -> SetRenderWindow( renWin1 );
  m_Iren2 -> SetRenderWindow( renWin2 );
  m_Iren -> SetRenderWindow( renWin );

  m_ModelActor1 = vtkSmartPointer <vtkActor> ::New();
  m_ModelActor11 = vtkSmartPointer <vtkActor>::New();
  m_ModelActor2 = vtkSmartPointer <vtkActor>::New();
  m_ModelActor22 = vtkSmartPointer <vtkActor>::New();
        
  m_Downsampling = 1;

  m_BgColor[ 0 ] = 1;
  m_BgColor[ 1 ] = 1;
  m_BgColor[ 2 ] = 1;

  m_Renderer1 -> SetBackground( 1 , 1 , 1 );
  m_Renderer2 -> SetBackground( 1 , 1 , 1 );
  m_Renderer -> SetBackground( 1 , 1 , 1 );
  m_Renderer -> SetViewport( 0 , 0 , 1 , 1 );
        
  m_Histogram1 = NULL;
  m_Histogram2 = NULL;
  m_Hist = NULL;

  m_CmapLen = 256;

  m_MeanError = 0;
  m_SumError = 0;
  m_SumSquareDeviations = 0;
  m_AbsSumSquareDeviations = 0;
  m_SigmaError = 0;
  m_ErrorMax = 0;

  m_FaceMeanError = 0;
  m_FaceRmsError = 0;

  m_Median = 0;
  m_Percentile95 = 0;
  m_Percentile75 = 0;
  m_Percentile68 = 0;
  m_Percentile25 = 0;

  m_DiceCoefficient[0] = 0.0;
  m_IntUnionRatio[0] = 0.0;
  
  m_Computed = false;

  m_Pargs.do_symmetric = 0;

  // desable gui button
  UpdateColorButton -> setEnabled(false);
  UpdateHistButton -> setEnabled(false);
  middleSlider -> setEnabled(false);
  IntervalSpinBox -> setEnabled(false);
  ColormapCheckbox1 -> setEnabled(false);
  ColormapCheckbox2 -> setEnabled(false);

    
  m_Middle = 0;

  for ( float i = 0 ; i < 256 ; i++ )
  {
      m_Lookuptable[ (int)i ].R = 255*( (-(i*i)/(float)4096) + (i/32) );
      if ( m_Lookuptable[ (int)i ].R < 0 )
      {
          m_Lookuptable[(int)i].R =0;
      }

      m_Lookuptable[ (int)i ].G = 255*( (-((i-64)*(i-64))/(float)4096) + ((i-64)/(float)32) );
      if ( m_Lookuptable[ (int)i ].G < 0 )
      {
          m_Lookuptable[ (int)i ].G = 0;
      }

      m_Lookuptable[ (int)i ].B = 255*( (-((i-128)*(i-128))/(float)4096) + ((i-128)/(float)32) );
      if ( m_Lookuptable[ (int)i ].B < 0 )
      {
          m_Lookuptable[ (int)i ].B = 0;
      }
  }
  
  m_Menubar = new QMenuBar( this , "menu" );
  m_Output = new QPopupMenu( this );
  m_View = new QPopupMenu( this );
  m_Comp = new QPopupMenu( this );

  QPopupMenu * file = new QPopupMenu( this );  
  QPopupMenu * help = new QPopupMenu( this );

  m_Menubar -> insertItem( "File" , file );
  m_Menubar -> insertItem( "Output" , output );
  m_Menubar -> insertItem( "View" , view );
  m_Menubar -> insertItem( "Help" , help );
  m_Menubar -> insertItem( "Compute", comp );

  m_Output -> setCheckable( true );
  m_View -> setCheckable( true );

  // Connections
  file -> insertItem( "Load Model A" , this , SLOT( Model1Open() ) );
  file -> insertItem( "Load Model B" , this , SLOT( Model2Open() ) );
  file -> insertSeparator();
  file -> insertItem( "Exit" , this , SLOT( close() ) );
  
  m_Output -> insertItem( "Save Model 1" , this , SLOT( SaveModel1() ) );
  m_Output -> insertItem( "Save Model 2" , this , SLOT( SaveModel2() ) );
  m_Output -> insertSeparator();

  m_Idtext = m_Output -> insertItem( "External TextWindow" , this , SLOT( CheckTextWindow() ) );
  m_Output -> setItemChecked( m_Idtext , false );
  m_Output -> insertSeparator();

  m_Idhist = m_Output -> insertItem( "Output Histogram" , this , SLOT( OutputHistogram() ) );
  m_Output -> setItemEnabled( m_Idhist , false );
  m_Output->insertSeparator();

  m_Iderror = m_Output -> insertItem( "Output Sample Errors" , this , SLOT( OutputRawErrors() ) );
  m_Output -> setItemEnabled( m_Iderror , false );
  m_Output -> insertSeparator();

  m_Idverror = m_Output -> insertItem( "Output Vertex Errors" , this , SLOT( OutputVertexErrors() ) );
  m_Output -> setItemEnabled( m_Idverror , false );
  m_Output -> insertSeparator();

  m_Idstat = m_Output -> insertItem( "Output Statistical Infos" , this , SLOT( OutputStats() ) );
  m_Output -> setItemEnabled( m_Idstat , false );
  
  m_Comp -> insertItem( "Compute Error" , this , SLOT( Compute() ) );
  m_Comp -> insertSeparator();

  m_Idcomphist = m_Comp -> insertItem( "Update Histogram" , this , SLOT( ComputeHistogram() ) );
  m_Comp -> setItemEnabled( m_Idcomphist , false );
  
  m_Idbg = m_View -> insertItem( "Switch BLK/WHT Background" , this , SLOT( SwitchBackground() ) );
  m_View -> insertSeparator();

  m_Idreset = m_View -> insertItem( "Reset Cameras" , this , SLOT( ResetCameras() ) );
  m_View -> insertSeparator();

  m_Idsyn = m_View -> insertItem( "Synchronize Viewport" , this , SLOT( SwitchSync() ) );
  m_View -> setItemEnabled( m_Idsyn , false );
  m_View -> insertSeparator();

  m_Idoverlay = m_View -> insertItem( "Overlay Two Views" , this , SLOT( SwitchOverlay() ) );
  m_View -> setItemEnabled( m_Idoverlay , false );
  m_View -> insertSeparator();

  m_View -> insertItem( "Fill/Line/Point for Model 1" , this , SLOT( Model1Switch() ) );
  m_View -> insertItem( "Fill/Line/Point for Model 2" , this , SLOT( Model2Switch() ) );
  
  help -> insertItem( "About MeshValmet" , this , SLOT( AboutMeshValmet() ) );

}

MeshValmetControls::~MeshValmetControls()
{

}


/* Slots */
void MeshValmetControls::Model1Open()
{
  QString Model1File;
  if( m_CommonPath == "" )
  {
    Model1File = QFileDialog::getOpenFileName( "." , "3D Models (*.vtk)" , this , "open file dialog" , "Choose a file to open" );
  }
  else
  {
    Model1File = QFileDialog::getOpenFileName( m_CommonPath ,"3D Models (*.vtk)" , this , "open file dialog" , "Choose a file to open" );
  }
  if( Model1File != "" && Model1File != NULL )
  {
    QFileInfo fi( Model1File.latin1() );
    m_CommonPath = fi.dirPath();
    model1Name -> setText( fi.fileName() );

    /* ------------------------------> create the model from a vtk file and then the poly data <------------------------------------*/
    if( Model1File.right(4).lower() == ".vtk" )
    {
        std::cout << " vtk file need to be read " << std::endl;

        //struct model* Model1 = read_model_vtk( Model1File.latin1() );
        //m_Mesh1 = BuildMeshFromModel( Model1 );

    }
    /* ------------------------------> create the model from a vtk file and then the poly data <------------------------------------*/


    SetupWindow( m_Mesh1 , m_Renderer1 , m_ModelActor1 );
    m_ModelActor1 -> GetProperty() -> SetColor( 0 , 0 , 1 );
    renWin1 -> Render();

    m_FileName1 = Model1File;

    ColormapCheckbox1 -> setEnabled(false);
    ColormapCheckbox2 -> setEnabled(false);
    ColormapCheckbox1 -> setChecked(false);

    m_Computed = false;

    m_Comp -> setItemEnabled( m_Idcomphist , false );
    m_Output -> setItemEnabled( m_Idhist , false );
    m_Output -> setItemEnabled( m_Iderror , false );
    m_Output -> setItemEnabled( m_Idverror , false );
    m_Output -> setItemEnabled( m_Idstat , false );

    if( model1Name -> text() == ""  || model2Name -> text() == "" )
    {
        m_View -> setItemEnabled( m_Idsyn , false );
        m_View -> setItemEnabled( m_Idoverlay , false );
    }
    else
    {
        m_View -> setItemEnabled( m_Idsyn , true );
        m_View -> setItemEnabled( m_Idoverlay , true );
    }

    UpdateColorButton -> setEnabled( false );
    UpdateHistButton -> setEnabled( false );
    middleSlider -> setEnabled( false );
    IntervalSpinBox -> setEnabled( false );
  }
}

void MeshValmetControls::Model2Open()
{
    QString Model2File;
    if( m_CommonPath == "" )
    {
      Model2File = QFileDialog::getOpenFileName( "." , "3D Models (*.vtk)" , this , "open file dialog" , "Choose a file to open" );
    }
    else
    {
      Model2File = QFileDialog::getOpenFileName( m_CommonPath ,"3D Models (*.vtk)" , this , "open file dialog" , "Choose a file to open" );
    }
    if( Model2File != "" && Model2File != NULL )
    {
      QFileInfo fi( Model2File.latin1() );
      m_CommonPath = fi.dirPath();
      model2Name -> setText( fi.fileName() );

      /* ------------------------------> create the model from a vtk file and then the poly data <------------------------------------*/
      if( Model2File.right(4).lower() == ".vtk" )
      {
          std::cout << " vtk file need to be read " << std::endl;
          /*
          struct model* Model2 = read_model_vtk( Model1File.latin1() );
          m_Mesh2 = BuildMeshFromModel( Model2 );
          */
      }
      /* ------------------------------> create the model from a vtk file and then the poly data <------------------------------------*/


      SetupWindow( m_Mesh2 , m_Renderer2 , m_ModelActor2 );
      m_ModelActor2 -> GetProperty() -> SetColor( 1 , 0 , 0 );
      renWin1 -> Render();

      m_FileName2 = Model2File;

      ColormapCheckbox1 -> setEnabled(false);
      ColormapCheckbox2 -> setEnabled(false);
      ColormapCheckbox1 -> setChecked(false);

      m_Computed = false;

      m_Comp -> setItemEnabled( m_Idcomphist , false );
      m_Output -> setItemEnabled( m_Idhist , false );
      m_Output -> setItemEnabled( m_Iderror , false );
      m_Output -> setItemEnabled( m_Idverror , false );
      m_Output -> setItemEnabled( m_Idstat , false );

      if( model1Name -> text() == ""  || model2Name -> text() == "" )
      {
          m_View -> setItemEnabled( m_Idsyn , false );
          m_View -> setItemEnabled( m_Idoverlay , false );
      }
      else
      {
          m_View -> setItemEnabled( m_Idsyn , true );
          m_View -> setItemEnabled( m_Idoverlay , true );
      }

      UpdateColorButton -> setEnabled( false );
      UpdateHistButton -> setEnabled( false );
      middleSlider -> setEnabled( false );
      IntervalSpinBox -> setEnabled( false );
    }
}

void MeshValmetControls::SaveModel1()
{
  if( m_Renderer1 -> GetNumberOfPropsRendered() != 0 )
  {
    QString OutFile;

    if( m_CommonPath == "" )
    {
      OutFile = QFileDialog::getSaveFileName( "." , "Mesh Info (*.vtk)" , this , "save file dialog" , "Choose a filename to save under" );
    }
    else
    {
      OutFile = QFileDialog::getSaveFileName( m_CommonPath , "Mesh Info (*.vtk)" , this , "save file dialog" , "Choose a filename to save under" );
    }

    if( OutFile != 0 )
    {
      if( OutFile.right(4) != ".vtk")
      {
        OutFile = OutFile + ".vtk";
      }

      if ( QFile::exists( OutFile ) )
      {
            QMessageBox mb( "MeshValmet", "Saving the file will overwrite the old file on disk.\n Do you really want to save?",
                            QMessageBox::Information,
                            QMessageBox::Yes | QMessageBox::Default,
                            QMessageBox::No,
                            QMessageBox::Cancel | QMessageBox::Escape );

            mb.setButtonText( QMessageBox::Yes, "Save" );
            mb.setButtonText( QMessageBox::No, "Don't Save" );

            switch( mb.exec() )
            {
              case QMessageBox::Yes:
                 m_Renderer1 -> SetBackground( 0 , 0 , 0 );
                 /*-----------------------------------------> Save Model to VTK format <----------------------------------------- */
                 //SaveMeshToVTK( renWin1, OutFile );
                 /*-----------------------------------------> Save Model to VTK format <----------------------------------------- */
                m_Renderer1 -> SetBackground( m_BgColor[0] , m_BgColor[1] , m_BgColor[2] );
              break;

              case QMessageBox::No:
              break;

              case QMessageBox::Cancel:
              break;
           }
      }
      else
      {
        m_Renderer1 -> SetBackground( 0 , 0 , 0 );
        /*-----------------------------------------> Save Model to VTK format <----------------------------------------- */
        //SaveMeshToVTK( renWin1, OutFile );
        /*-----------------------------------------> Save Model to VTK format <----------------------------------------- */
        m_Renderer1 -> SetBackground( m_BgColor[0] , m_BgColor[1] , m_BgColor[2] );
      }

      QFileInfo fi( OutFile );
      m_CommonPath = fi.dirPath();

    }
  }
}

void MeshValmetControls::SaveModel2()
{
    if( m_Renderer2 -> GetNumberOfPropsRendered() != 0 )
    {
      QString OutFile;

      if( m_CommonPath == "" )
      {
        OutFile = QFileDialog::getSaveFileName( "." , "Mesh Info (*.vtk)" , this , "save file dialog" , "Choose a filename to save under" );
      }
      else
      {
        OutFile = QFileDialog::getSaveFileName( m_CommonPath , "Mesh Info (*.vtk)" , this , "save file dialog" , "Choose a filename to save under" );
      }

      if( OutFile != 0 )
      {
        if( OutFile.right(4) != ".vtk")
        {
          OutFile = OutFile + ".vtk";
        }

        if ( QFile::exists( OutFile ) )
        {
              QMessageBox mb( "MeshValmet", "Saving the file will overwrite the old file on disk.\n Do you really want to save?",
                              QMessageBox::Information,
                              QMessageBox::Yes | QMessageBox::Default,
                              QMessageBox::No,
                              QMessageBox::Cancel | QMessageBox::Escape );

              mb.setButtonText( QMessageBox::Yes, "Save" );
              mb.setButtonText( QMessageBox::No, "Don't Save" );

              switch( mb.exec() )
              {
                case QMessageBox::Yes:
                   m_Renderer2 -> SetBackground( 0 , 0 , 0 );
                   /*-----------------------------------------> Save Model to VTK format <----------------------------------------- */
                   //SaveMeshToVTK( renWin1, OutFile );
                   /*-----------------------------------------> Save Model to VTK format <----------------------------------------- */
                  m_Renderer2 -> SetBackground( m_BgColor[0] , m_BgColor[1] , m_BgColor[2] );
                break;

                case QMessageBox::No:
                break;

                case QMessageBox::Cancel:
                break;
             }
        }
        else
        {
          m_Renderer2 -> SetBackground( 0 , 0 , 0 );
          /*-----------------------------------------> Save Model to VTK format <----------------------------------------- */
          //SaveMeshToVTK( renWin1, OutFile );
          /*-----------------------------------------> Save Model to VTK format <----------------------------------------- */
          m_Renderer2 -> SetBackground( m_BgColor[0] , m_BgColor[1] , m_BgColor[2] );
        }

        QFileInfo fi( OutFile );
        m_CommonPath = fi.dirPath();

      }
    }
}

void MeshValmetControls::CheckTextWindow()
{
  if( m_Output -> isItemChecked( m_Idtext ) )
  {
    m_Output -> setItemChecked( m_Idtext , false );
  }
  else
  {
    m_Output -> setItemChecked( m_Idtext , true );
  }
}

void MeshValmetControls::OutputHistogram()
{
  if( ( m_Pargs.do_symmetric == 1 || m_Pargs.do_symmetric == 3 ) && m_Histogram1 == NULL
          || (m_Pargs.do_symmetric == 2 || m_Pargs.do_symmetric == 3 ) && m_Histogram2 == NULL
                || m_Pargs.do_symmetric == 0 )
  {
    return;
  }

  QString OutFile;
  if( m_CommonPath == "" )
  {
    OutFile = QFileDialog::getSaveFileName( "." , "Histogram Info (*.hist)" , this , "save file dialog" , "Choose a filename to save under" );
  }
  else
  {
          OutFile = QFileDialog::getSaveFileName( m_CommonPath , "Histogram Info (*.hist)" , this , "save file dialog" , "Choose a filename to save under" );
  }

  if( OutFile != 0 )
  {
    if( OutFile.right(5) != ".hist")
    {
      OutFile = OutFile + ".hist";

      if ( QFile::exists( outFile ) )
      {
        QMessageBox mb( "MeshValmet", "Saving the file will overwrite the old file on disk.\n Do you really want to save?",
                        QMessageBox::Information,
                        QMessageBox::Yes | QMessageBox::Default,
                        QMessageBox::No,
                        QMessageBox::Cancel | QMessageBox::Escape );

        mb.setButtonText( QMessageBox::Yes, "Save" );
        mb.setButtonText( QMessageBox::No, "Don't Save" );

        switch( mb.exec() )
          {
              case QMessageBox::Yes:
                    SaveHist( OutFile );
              break;

              case QMessageBox::No:
              break;

              case QMessageBox::Cancel:
              break;
          }
      }
      else
      {
        SaveHist(outFile);
      }

      QFileInfo fi( OutFile);
      m_CommonPath = fi.dirPath();

    }
   }
}

void MeshValmetControls::OutputRawErrors()
{
  QString OutFile;

  if( m_CommonPath == "" )
  {
    OutFile = QFileDialog::getSaveFileName( "." , "Raw Errors Info (*.errs)" , this , "save file dialog" , "Choose a filename to save under" );
  }
  else
  {
    OutFile = QFileDialog::getSaveFileName( m_CommonPath , "Raw Errors Info (*.errs)" , this , "save file dialog" , "Choose a filename to save under" );
  }

  if( OutFile != 0 )
  {
    if( OutFile.right(5) != ".errs" )
    {
       OutFile = OutFile + ".errs";
    }

    if( QFile::exists( OutFile ) )
    {
      QMessageBox mb( "MeshValmet" , "Saving the file will overwrite the old file on disk.\n Do you really want to save?",
                        QMessageBox::Information,
                        QMessageBox::Yes | QMessageBox::Default,
                        QMessageBox::No,
                        QMessageBox::Cancel | QMessageBox::Escape );

      mb.setButtonText( QMessageBox::Yes, "Save" );
      mb.setButtonText( QMessageBox::No, "Don't Save" );

      switch( mb.exec() )
      {
         case QMessageBox::Yes:
              SaveRawErrors( OutFile );
         break;

         case QMessageBox::No:
         break;

         case QMessageBox::Cancel:
         break;
      }
    }
    else
    {
      SaveRawErrors( OutFile );
    }

    QFileInfo fi( OutFile );
    m_CommonPath = fi.dirPath();

  }
}

void MeshValmetControls::OutputVertexErrors()
{
  QString OutFile;

  if( m_CommonPath == "" )
  {
      OutFile = QFileDialog::getSaveFileName( "." , "Raw Errors Info (*.verrs)" , this , "save file dialog" , "Choose a filename to save under" );
  }
  else
  {
      OutFile = QFileDialog::getSaveFileName( m_CommonPath , "Raw Errors Info (*.verrs)" , this , "save file dialog" , "Choose a filename to save under" );
  }

  if( OutFile != 0 )
  {
     if( OutFile.right(6) != ".verrs")
     {
       OutFile = OutFile + ".verrs";
     }

    if( QFile::exists( OutFile ) )
    {
      QMessageBox mb( "MeshValmet", "Saving the file will overwrite the old file on disk.\n Do you really want to save?" ,
                      QMessageBox::Information,
                      QMessageBox::Yes | QMessageBox::Default,
                      QMessageBox::No,
                      QMessageBox::Cancel | QMessageBox::Escape );

      mb.setButtonText( QMessageBox::Yes, "Save" );
      mb.setButtonText( QMessageBox::No, "Don't Save" );

      switch( mb.exec() )
      {
         case QMessageBox::Yes:
              SaveVertexErrors( OutFile );
         break;

         case QMessageBox::No:
         break;

         case QMessageBox::Cancel:
         break;
      }
    }
    else
    {
      SaveVertexErrors( OutFile );
    }

    QFileInfo fi( OutFile );
    m_CommonPath = fi.dirPath();

  }
}

void MeshValmetControls::OutputStats()
{
  QString OutFile;

  if( m_CommonPath == "" )
  {
     OutFile = QFileDialog::getSaveFileName( "." , "Statistics Info (*.stat)" , this , "save file dialog" , "Choose a filename to save under" );
  }
  else
  {
     OutFile = QFileDialog::getSaveFileName( m_CommonPath , "Statistics Info (*.stat)" , this , "save file dialog" , "Choose a filename to save under" );
  }

  if( OutFile != 0 )
  {
    if( OutFile.right(5) != ".stat" )
    {
       OutFile = OutFile + ".stat";
    }

    if ( QFile::exists( OutFile ) )
    {
      QMessageBox mb( "MeshValmet", "Saving the file will overwrite the old file on disk.\n Do you really want to save?",
                        QMessageBox::Information,
                        QMessageBox::Yes | QMessageBox::Default,
                        QMessageBox::No,
                        QMessageBox::Cancel | QMessageBox::Escape );

      mb.setButtonText( QMessageBox::Yes, "Save" );
      mb.setButtonText( QMessageBox::No, "Don't Save" );

      switch( mb.exec() )
      {
         case QMessageBox::Yes:
               SaveStats( OutFile );
         break;

         case QMessageBox::No:
         break;

         case QMessageBox::Cancel:
         break;
      }
    }
    else
    {
      SaveStats( OutFile );
    }

    QFileInfo fi( OutFile );
    m_CommonPath = fi.dirPath();

  }
}

void MeshValmetControls::Compute()
{
  QString File1 = model1Name -> text();
  QString File2 = model2Name -> text();

  if( File1 == "" || File2 == "" )
  {
    QMessageBox::warning( this , "Warning" , "Please input two model files!" , QMessageBox::Ok , QMessageBox::NoButton , QMessageBox::NoButton );
    return;
  }

  QProgressDialog *qProg;
  struct prog_reporter pr;

  qProg = NULL;

  memset( &m_Model1 , 0 , sizeof( m_Model1 ) );
  memset( &m_Model2 , 0 , sizeof( m_Model2 ) );

  memset( &pr , 0 , sizeof( pr ) );
  m_Log = NULL;

  qProg = new QProgressDialog( "Calculating distance" , 0 , 100 );
  qProg -> setMinimumDuration( 1500 );

  pr.prog = QT_prog;
  pr.cb_out = qProg;

  MeshSetup( &m_Pargs );

  mesh_run( &m_Pargs , &m_Model1 , &m_Model2 , m_Log , &pr , &m_Stats , &m_StatsRev , &m_AbsSamplingStep , &m_AbsSamplingDens );

  int num_vert1 = m_Model1.mesh -> num_vert;
  int num_vert2 = m_Model2.mesh -> num_vert;
  int num_faces1 = m_Model1.mesh -> num_faces;
  int num_faces2 = m_Model2.mesh -> num_faces;

  double * L1 = new double[ 3*num_vert1 ];
  double * L2 = new double[ 3*num_vert2 ];
  int * T1 = new int[ 3*num_faces1 ];
  int * T2 = new int[ 3*num_faces2 ];

  vertex_t * vert_list = m_Model1.mesh->vertices;

  for( int i = 0 ; i < num_vert1 ; i++ )
  {
      L1[ 3*i + 0 ] = vert_list[ i ].x;
      L1[ 3*i + 1 ] = vert_list[ i ].y;
      L1[ 3*i + 2 ] = vert_list[ i ].z;
  }

  vert_list = m_Model2.mesh -> vertices;

  for( int i = 0 ; i < num_vert2 ; i++ )
  {
      L2[ 3*i + 0 ] = vert_list[ i ].x;
      L2[ 3*i + 1 ] = vert_list[ i ].y;
      L2[ 3*i + 2 ] = vert_list[ i ].z;
  }

  face_t * face_list = m_Model1.mesh -> faces;

  for( int i = 0 ; i < num_faces1 ; i++ )
  {
      T1[ 3*i + 0 ] = face_list[ i ].f0 + 1;
      T1[ 3*i + 1 ] = face_list[ i ].f1 + 1;
      T1[ 3*i + 2 ] = face_list[ i ].f2 + 1;
  }

  face_list = m_Model2.mesh -> faces;

  for( int i = 0 ; i < num_faces2 ; i++ )
  {
      T2[ 3*i + 0 ] = face_list[ i ].f0 + 1;
      T2[ 3*i + 1 ] = face_list[ i ].f1 + 1;
      T2[ 3*i + 2 ] = face_list[ i ].f2 + 1;
  }

  ComputeRobustVolumeOverlap( L1 , L2 , num_vert1 , num_vert2 , T1 , T2 , num_faces1 , num_faces2 , m_DiceCoefficient , m_IntUnionRatio );

  delete [] L1;
  delete [] L2;
  delete [] T1;
  delete [] T2;

  int t = m_Pargs.do_symmetric;
  bool Signedd = m_Pargs.signeddist;

  if( t == 1 )
  {
    if( Signedd )
    {
      m_Dmax = m_Model1.max_error;
      m_Dmin = m_Model1.min_error;
    }
    else
    {
      m_Dmax = m_Model1.abs_max_error;
      m_Dmin = m_Model1.abs_min_error;
    }
  }
  else if( t == 2 )
  {
    if( Signedd )
    {
      m_Dmax = m_Model2.max_error;
      m_Dmin = m_Model2.min_error;
    }
    else
    {
      m_Dmax = m_Model2.abs_max_error;
      m_Dmin = m_Model2.abs_min_error;
    }
  }
  else if( t == 3 )
  {
    if( Signedd )
    {
      m_Dmax = ( m_Model1.max_error > m_Model2.max_error ) ? m_Model1.max_error : m_Model2.max_error;
      m_Dmin = ( m_Model1.min_error > m_Model2.min_error ) ? m_Model2.min_error : m_Model1.min_error;
    }
    else
    {
      m_Dmax = ( m_Model1.abs_max_error > m_Model2.abs_max_error ) ? m_Model1.abs_max_error : m_Model2.abs_max_error;
      m_Dmin = ( m_Model1.abs_min_error > m_Model2.abs_min_error ) ? m_Model2.abs_min_error : m_Model1.abs_min_error;
    }
  }

  m_Downsampling = DownsamplingSlider -> value();

  if( m_Pargs.do_symmetric == 1 || m_Pargs.do_symmetric == 3 )
  {
      drawVertexErrorT( m_Mesh1 , &m_Model1 , m_ModelActor1 , renWin1 );

      ColormapCheckbox1 -> setEnabled( true );
      ColormapCheckbox1 -> setChecked( TRUE );
  }

  if( m_Pargs.do_symmetric == 2 || m_Pargs.do_symmetric == 3 )
  {
      drawVertexErrorT( m_Mesh2 , &m_Model2 , m_ModelActor2 , renWin2 );

      ColormapCheckbox2 -> setEnabled( true );
      ColormapCheckbox2 -> setChecked( TRUE );
  }

  ColormapMin -> setText( QString::number( m_Dmin ) );
  ColormapMax -> setText( QString::number( m_Dmax ) );

  DoStat();

  OutputStatInfo();

  m_Computed = true;
  m_Comp -> setItemEnabled( m_Idcomphist , true );
  m_Output -> setItemEnabled( m_Idhist , true );
  m_Output -> setItemEnabled( m_Iderror , true );
  m_Output -> setItemEnabled( m_Idverror , true );
  m_Output -> setItemEnabled( m_Idstat , true );
  m_View -> setItemEnabled( m_Idsyn , true );

  UpdateColorButton -> setEnabled( true );
  UpdateHistButton -> setEnabled( true );
  middleSlider -> setEnabled( true );
  IntervalSpinBox -> setEnabled( true );

  ComputeHistogram();

  fprintf( stdout , "\n\nStatistical results:\n" );
  fprintf( stdout , "\t Min:\t\t\t%f\n", m_Dmin );
  fprintf( stdout , "\t Max:\t\t\t%f\n", m_Dmax );
  fprintf( stdout , "\t VertexMean:\t\t%f\n", m_MeanError );
  fprintf( stdout , "\t VertexStd:\t\t%f\n", m_SigmaError );
  fprintf( stdout , "\t FaceMean:\t\t%f\n", m_FaceMeanError );
  fprintf( stdout , "\t FaceRMS:\t\t%f\n", m_FaceRmsError );
  fprintf( stdout , "\t Hausdorff:\t\t%f\n" , m_HausdorffError );
  fprintf( stdout , "\t MSD:\t\t\t%f\n" , m_MsdError );
  fprintf( stdout , "\t MAD:\t\t\t%f\n" , m_MadError );
  fprintf( stdout , "\t Median:\t\t%f\n" , m_Median );
  fprintf( stdout , "\t 25percentile:\t\t%f\n" , m_Percentile25 );
  fprintf( stdout , "\t 68percentile:\t\t%f\n" , m_Percentile68 );
  fprintf( stdout , "\t 75percentile:\t\t%f\n" , m_Percentile75 );
  fprintf( stdout , "\t 95percentile:\t\t%f\n" , m_Percentile95 );
}

void MeshValmetControls::ComputeHistogram()
{
  if( m_Computed == false )
  {
    return;
  }

  m_CmapLen = BinsSpinBox -> value();

  DoHistogram();

  xyplot = vtkSmartPointer <vtkXYPlotActor> ::New();
  xyplot->PlotPointsOn();
  xyplot->GetProperty()->SetPointSize(3);
  xyplot->GetProperty()->SetColor(0,0,1);
  xyplot->GetTitleTextProperty()->SetColor(0,0,1);
  xyplot->SetTitle("Histograms of Mesh Errors");
  xyplot->SetNumberOfXLabels(6);
  xyplot->GetAxisTitleTextProperty()->SetColor(0,0,1);
  xyplot->SetXTitle("Distance");
  xyplot->SetYTitle("# of sample points");
  xyplot->GetPositionCoordinate()->SetValue(0, 0, 0);
  xyplot->GetPosition2Coordinate()->SetValue(1, 1, 0);//relative to Position
  xyplot->SetXValuesToValue();
  xyplot->LegendOn();
  xyplot->GetAxisLabelTextProperty()->SetColor(0,0,1);


  vtkSmartPointer <vtkPolyData> mesh1 = vtkSmartPointer <vtkPolyData> ::New();
  vtkSmartPointer <vtkPolyData> mesh2 = vtkSmartPointer <vtkPolyData> ::New();
  vtkSmartPointer <vtkPolyData> mesh3 = vtkSmartPointer <vtkPolyData> ::New();

  if( m_Pargs.do_symmetric == 1 || m_Pargs.do_symmetric == 3)
  {
    vtkSmartPointer <vtkPoints> points = vtkSmartPointer <vtkPoints> ::New();
    vtkSmartPointer <vtkDoubleArray> scalars = vtkSmartPointer <vtkDoubleArray> ::New();

    int i;
    double v[3];

    for ( i = 0 ; i < m_CmapLen ; i++ )
    {
      v[0] = m_Dmin + (double)( m_Dmax - m_Dmin )*i / (double)m_CmapLen;
      v[1] = m_Dmin + (double)( m_Dmax - m_Dmin )*i / (double)m_CmapLen;
      v[2] = m_Dmin + (double)( m_Dmax - m_Dmin )*i / (double)m_CmapLen;

      points->InsertPoint(i,v);
      scalars->InsertTuple1(i,(double)m_Histogram1[i]);
    }

    mesh1->SetPoints(points);
    mesh1->GetPointData()->SetScalars(scalars);
    xyplot->AddInput( ( vtkSmartPointer <vtkDataSet> )mesh1 );

    xyplot->SetPlotColor(0,0,0,1);
    xyplot->SetPlotLabel(0, "A 2 B");

  }

  if( m_Pargs.do_symmetric == 2 || m_Pargs.do_symmetric == 3 )
  {
      vtkSmartPointer <vtkPoints> points = vtkSmartPointer <vtkPoints> ::New();
      vtkSmartPointer <vtkDoubleArray> scalars = vtkSmartPointer <vtkDoubleArray> ::New();

      int i;
      double v[3];

      for( i = 0 ; i < m_CmapLen ; i++ )
      {
        v[0] = m_Dmin + (double)( m_Dmax - m_Dmin )*i / (double)m_CmapLen;
        v[1] = m_Dmin + (double)( m_Dmax - m_Dmin )*i / (double)m_CmapLen;
        v[2] = m_Dmin + (double)( m_Dmax - m_Dmin )*i / (double)m_CmapLen;

        points->InsertPoint(i,v);
        scalars->InsertTuple1(i,(double)m_Histogram2[i]);
      }

      mesh2->SetPoints(points);
      mesh2->GetPointData()->SetScalars(scalars);
      xyplot->AddInput( ( vtkSmartPointer <vtkDataSet> )mesh2 );

    if( m_Pargs.do_symmetric == 2)
    {
      xyplot->SetPlotColor(0,1,0,0);
      xyplot->SetPlotLabel(0, "B 2 A");
    }
    else
    {
      xyplot->SetPlotColor(1,1,0,0);
      xyplot->SetPlotLabel(1, "B 2 A");
    }
  }

  if( m_Pargs.do_symmetric == 3)
  {
      vtkSmartPointer <vtkPoints> points = vtkSmartPointer <vtkPoints> ::New();
      vtkSmartPointer <vtkDoubleArray> scalars = vtkSmartPointer <vtkDoubleArray> ::New();

    int i;
    double v[3];
    for ( i = 0 ; i < m_CmapLen ; i++ )
    {
        v[0] = m_Dmin + (double)( m_Dmax - m_Dmin )*i / (double)m_CmapLen;
        v[1] = m_Dmin + (double)( m_Dmax - m_Dmin )*i / (double)m_CmapLen;
        v[2] = m_Dmin + (double)( m_Dmax - m_Dmin )*i / (double)m_CmapLen;

        points->InsertPoint(i,v);
        scalars->InsertTuple1(i,(double)m_Histogram1[i]+m_Histogram2[i]);\
    }
    mesh3->SetPoints(points);
    mesh3->GetPointData()->SetScalars(scalars);

    xyplot->AddInput( ( vtkSmartPointer <vtkDataSet> )mesh3 );

    xyplot->SetPlotColor(2,0,1,0);
    xyplot->SetPlotLabel(2, "A and B");
  }

  m_Renderer->RemoveAllViewProps();
  m_Renderer->AddActor2D(xyplot);
  renWin->update();
}

void MeshValmetControls::SwitchBackground()
{
  if( m_BgColor[0] == 0 )
  {
    m_BgColor[0] = 1;
    m_BgColor[1] = 1;
    m_BgColor[2] = 1;
  }
  else
  {
    m_BgColor[0] = 0;
    m_BgColor[1] = 0;
    m_BgColor[2] = 0;
  }
  m_Renderer1->SetBackground(m_BgColor[0],m_BgColor[1],m_BgColor[2]);
  m_Renderer2->SetBackground(m_BgColor[0],m_BgColor[1],m_BgColor[2]);
  renWin1->Render();
  renWin2->Render();
}

void MeshValmetControls::ResetCameras()
{
  vtkSmartPointer <vtkCamera> camera1 = vtkSmartPointer <vtkCamera> ::New();
  camera1->SetPosition(1,1,1);
  camera1->SetFocalPoint(0,0,0);
vtkSmartPointer <vtkCamera> camera2 = vtkSmartPointer <vtkCamera> ::New();
  camera2->SetPosition(1,1,1);
  camera2->SetFocalPoint(0,0,0);

  m_Renderer1->SetActiveCamera(camera1);
  m_Renderer2->SetActiveCamera(camera2);
  m_Renderer1->ResetCamera();
  m_Renderer2->ResetCamera();

  renWin1->Render();
  renWin2->Render();
}

void MeshValmetControls::SwitchSync()
{
  if(m_View->isItemEnabled( m_Idsyn))
  {
    if(m_View->isItemChecked(m_Idsyn))
    {
      m_View->setItemChecked(m_Idsyn,false);
      renWin1->SetInteractorSync(NULL);
      renWin2->SetInteractorSync(NULL);
    }
    else
    {
      m_View->setItemChecked( m_Idsyn,true);

      vtkSmartPointer <vtkCamera> camera1 = vtkSmartPointer <vtkCamera> ::New();
      camera1->SetPosition(1,1,1);
      camera1->SetFocalPoint(0,0,0);
      vtkSmartPointer <vtkCamera> camera2 = vtkSmartPointer <vtkCamera> ::New();
      camera2->SetPosition(1,1,1);
      camera2->SetFocalPoint(0,0,0);

      m_Renderer1->SetActiveCamera(camera1);
      m_Renderer2->SetActiveCamera(camera2);
      m_Renderer1->ResetCamera();
      m_Renderer2->ResetCamera();

      renWin1->SetInteractorSync(m_Iren1);
      renWin2->SetInteractorSync(m_Iren2);

      renWin1->Render();
      renWin2->Render();

    }
  }
}

void MeshValmetControls::SwitchOverlay()
{
  if(m_View->isItemEnabled(m_Idoverlay))
  {
    if(m_View->isItemChecked(m_Idoverlay))
    {
      m_View->setItemChecked(m_Idoverlay,false);
      m_Renderer1->RemoveActor(m_ModelActor22);
      m_Renderer2->RemoveActor(m_ModelActor11);

      renWin1->Render();
      renWin2->Render();
    }
    else
    {
      m_View->setItemChecked(m_Idoverlay,true);
      SetupWindow(m_Mesh1,m_Renderer2,m_ModelActor11);
        m_ModelActor11->GetProperty()->SetColor(1,1,0);
        m_ModelActor11->GetProperty()->SetRepresentationToPoints();

        SetupWindow(m_Mesh2,m_Renderer1,m_ModelActor22);
        m_ModelActor22->GetProperty()->SetColor(1,1,0);
        m_ModelActor22->GetProperty()->SetRepresentationToPoints();

        renWin1->Render();
        renWin2->Render();
    }
  }

}

void MeshValmetControls::Model1Switch()
{
  m_Model1Line++;
  m_Model1Line = m_Model1Line % 3;
  if(m_Model1Line == 0)
    m_ModelActor1->GetProperty()->SetRepresentationToSurface();
  else if(m_Model1Line == 1)
    m_ModelActor1->GetProperty()->SetRepresentationToWireframe();
  else if(m_Model1Line == 2)
  {
    m_ModelActor1->GetProperty()->SetRepresentationToPoints();
    m_ModelActor1->GetProperty()->SetPointSize(3.0);
  }

  renWin1->Render();
}

void MeshValmetControls::Model2Switch()
{
  m_Model2Line++;
  m_Model2Line = m_Model2Line % 3;
  if(m_Model2Line == 0)
    m_ModelActor2->GetProperty()->SetRepresentationToSurface();
  else if(m_Model2Line == 1)
    m_ModelActor2->GetProperty()->SetRepresentationToWireframe();
  else if(m_Model2Line == 2)
  {
    m_ModelActor2->GetProperty()->SetRepresentationToPoints();
    m_ModelActor2->GetProperty()->SetPointSize(3.0);
  }

  renWin2->Render();
}

void MeshValmetControls::AboutMeshValmet()
{
  QMessageBox::information(this,"About MeshValmet", "MeshValmet 3.0 \n Author: Christine Xu \n Contact: xushun@gmail.com \n Funded by: Guido Gerig and Martin Styner \n Copyright (c) 2010");
}




/* Functions */
vtkSmartPointer <vtkPolyData> MeshValmetControls::BuildMeshFromModel(model* CurrModel)
{

  vtkSmartPointer <vtkPolyData> Mesh = vtkSmartPointer <vtkPolyData> ::New();
  vtkSmartPointer <vtkPoints> Points = vtkSmartPointer <vtkPoints> ::New();
  vtkSmartPointer <vtkCellArray> Polys = vtkSmartPointer <vtkCellArray> ::New();

  vtkIdType i;
  float v[3];
  vtkIdType f[3];

  for ( i=0 ; i < CurrModel -> num_vert ; i++ )
  {
      vertex_t t = CurrModel -> vertices[i];
      v[0] = t.x;
      v[1] = t.y;
      v[2] = t.z;
      Points -> InsertPoint( i , v );
  }

  for ( i=0 ; i < CurrModel -> num_faces ; i++ )
  {
      face_t t = CurrModel -> faces[i];
      f[0] = t.f0;
      f[1] = t.f1;
      f[2] = t.f2;
      Polys -> InsertNextCell( 3 , f );
  }

  Mesh -> SetPoints( Points );
  Mesh -> SetPolys( Polys );

  return Mesh;
}

void MeshValmetControls::SetupWindow( vtkSmartPointer<vtkPolyData> Mesh, vtkSmartPointer<vtkRenderer> Renderer, vtkSmartPointer<vtkActor> Actor)
{
  vtkSmartPointer <vtkPolyDataMapper> Mapper = vtkSmartPointer <vtkPolyDataMapper> ::New();

  Mapper -> SetInput( Mesh );
  Actor -> SetMapper( Mapper );
  Renderer -> AddActor( Actor );

  vtkSmartPointer <vtkCamera> Camera = vtkSmartPointer <vtkCamera> ::New();

  Camera -> SetPosition( 1 , 1 , 1 );
  Camera -> SetFocalPoint( 0 , 0 , 0 );
  Renderer -> SetActiveCamera( Camera );
  Renderer -> ResetCamera();
}

void MeshValmetControls::SaveMeshToIV( vtkSmartPointer <vtkRenderWindow> Win, QString Name )
{
  vtkSmartPointer <vtkVRMLExporter> Exporter = vtkSmartPointer <vtkVRMLExporter> ::New();
  Exporter -> SetRenderWindow( Win );
  Exporter -> SetFileName( Name.latin1() );
  Exporter -> Write();
  Exporter -> Delete();
}

void MeshValmetControls::SaveHist( QString FileName )
{
  QFile File( FileName);

  if( File.open( IO_WriteOnly ) )
  {
    QTextStream Stream( &File );

    if( m_Pargs.do_symmetric == 1 )
    {
        Stream << "#Histogram from Model1 \"" << m_Pargs.m1_fname << "\" to Model2 \"" << m_Pargs.m2_fname << "\"\n";
    }
    else if( m_Pargs.do_symmetric == 2 )
    {
        Stream << "#Histogram from Model2 \"" << m_Pargs.m2_fname << "\" to Model1 \"" << m_Pargs.m1_fname << "\"\n";
    }
    else if( m_Pargs.do_symmetric == 3)
    {
        Stream << "#Histgram of symmetric errors between Model1 \"" << m_Pargs.m1_fname << "\" and Model2 \"" << m_Pargs.m2_fname << "\"\n";
    }

    Stream << "#MAX: " << m_Dmax << "\n";
    Stream << "#MIN: " << m_Dmin << "\n";
    Stream << "#BINS: " << m_CmapLen << "\n";
    Stream << "#STEP: " << (double)( m_Dmax - m_Dmin ) / (double)m_CmapLen << "\n";
    Stream << "#VERTEXMEAN: " << m_MeanError << "\n";
    Stream << "#SIGMA: " << m_SigmaError << "\n";
    Stream << "#MEADIAN: " << m_Median << "\n";
    Stream << "#25PERCENTILE: " << m_Percentile25 <<  "\n";
    Stream << "#68PERCENTILE: " << m_Percentile68 <<  "\n";
    Stream << "#75PERCENTILE: " << m_Percentile75 <<  "\n";
    Stream << "#95PERCENTILE: " << m_Percentile95 <<  "\n";
    Stream << "#FACEMEAN: " << m_FaceMeanError << "\n";
    Stream << "#RMS: " << m_FaceRmsError << "\n";

    for( int i = 0 ; i < m_CmapLen ; i++ )
    {
      if( m_Pargs.do_symmetric == 1 )
      {
        Stream << m_Histogram1[ i ] << "\n";
      }
      else if( m_Pargs.do_symmetric == 2 )
      {
        Stream << m_Histogram2[ i ] << "\n";
      }
      else if( m_Pargs.do_symmetric == 3 )
      {
        Stream << m_Histogram1[ i ] + m_Histogram2[ i ] << "\n";
      }
     }

     File.close();
   }
   else
   {
     cout << "Cant open file for writing!" << endl;
   }
}

void MeshValmetControls::SaveRawErrors( QString OutFile )
{
  int i;
  int n;
  double *Serror;
  FILE* Fp;

  Fp = fopen( OutFile.latin1() , "w" );

  if( m_Pargs.do_symmetric == 1 || m_Pargs.do_symmetric == 3 )
  {
    fprintf( Fp ,"%s->%s\n", m_FileName1.latin1(), m_FileName2.latin1() );

    n = model1.n_samples;
    fprintf( Fp ,"%d\n" , n );

    Serror = model1.fe[0].serror;

    if( m_Pargs.signeddist == true )
    {
        for( i = 0 ; i < n ; i++ )
        {
          fprintf( Fp , "%f\n" , Serror[ i ] );
        }

        fprintf( Fp , "\n\n" );
    }
    else
    {
        for( i = 0 ; i < n ; i++ )
        {
            fprintf( Fp , "%f\n" , fabs( Serror[ i ] ) );
        }

        fprintf( Fp , "\n\n" );
    }
  }

  if( m_Pargs.do_symmetric == 2 || m_Pargs.do_symmetric == 3 )
  {
    fprintf( Fp ,"%s->%s\n", m_FileName2.latin1(), m_FileName1.latin1() );

    n = model2.n_samples;
    fprintf( Fp ,"%d\n", n );

    Serror = model2.fe[0].serror;

    if( m_Pargs.signeddist == true )
    {
        for ( i = 0 ; i < n ; i++ )
        {
           fprintf( Fp , "%f\n" , Serror[ i ] );
        }

        fprintf( Fp , "\n\n" );
    }
    else
    {
        for( i = 0 ; i < n ; i++ )
        {
          fprintf( Fp , "%f\n" , fabs( Serror[ i ] ) );
        }

        fprintf( Fp , "\n\n" );
    }
  }

  fclose( Fp );
}

void MeshValmetControls::SaveVertexErrors( QString OutFile )
{
  int i;
  int n;

  float *Verror;
  FILE* Fp;

  Fp = fopen( OutFile.latin1() , "w" );

  if( m_Pargs.do_symmetric == 1 || m_Pargs.do_symmetric == 3 )
  {
    fprintf( Fp , "%s->%s\n" , m_FileName1.latin1() , m_FileName2.latin1() );

    n = model1.mesh->num_vert;
    fprintf( Fp , "%d\n" , n );
    Verror = model1.verror;

    if( m_Pargs.signeddist == true )
    {
        for( i = 0 ; i < n ; i++ )
        {
          fprintf( Fp , "%f\n" , Verror[ i ] );
        }

        fprintf( Fp , "\n\n" );
    }
    else
    {
        for( i = 0 ; i < n ; i++ )
        {
          fprintf( Fp , "%f\n" , fabs( Verror[ i ] ) );
        }

        fprintf( Fp , "\n\n" );
    }
  }

  if( m_Pargs.do_symmetric == 2 || m_Pargs.do_symmetric == 3 )
  {
    fprintf( Fp , "%s->%s\n" , m_FileName2.latin1() , m_FileName1.latin1() );

    n = model2.mesh->num_vert;
    fprintf( Fp , "%d\n" , n );

    Verror = model2.verror;

    if( m_Pargs.signeddist == true )
    {
        for( i = 0 ; i < n ; i++ )
        {
          fprintf( Fp , "%f\n" , Verror[ i ] );
        }

        fprintf( Fp , "\n\n" );
    }
    else
    {
      for( i = 0 ; i < n ; i++ )
      {
          fprintf( Fp , "%f\n" , fabs( Verror[ i ] ) );
      }

      fprintf( Fp , "\n\n" );
    }
  }

  fclose( Fp );

}

void MeshValmetControls::SaveStats( QString OutFile )
{
  FILE* Fp = fopen( OutFile.latin1() , "w" );

  fprintf( Fp , "Min\tMax\tVertexMean\tVertexSigma\tFaceMean\tFaceRMS\tHausdorff\tMSD\tMAD\tMedian\t25percentile\t68percentile\t75percentile\t95percentile\n");
  fprintf( Fp , "%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\n", m_Dmin , m_Dmax , m_MeanError, m_SigmaError , m_FaceMeanError , m_FaceRmsError ,
           m_HausdorffError, m_MadError , m_MadError , m_Median , m_Percentile25 , m_Percentile68 , m_Percentile75 , m_Percentile95 );

  fclose( Fp );
}

void QT_prog( void *out , int p )
{
  QProgressDialog *qpd;
  qpd = (QProgressDialog*)out;
  qpd -> setProgress( p<0 ? qpd -> totalSteps() : p );
  qApp -> processEvents();
}

void MeshValmetControls::MeshSetup( struct args* Pargs )
{
  Pargs -> m1_fname =  (char *)m_FileName1.latin1();
  Pargs -> m2_fname =  (char *)m_FileName2.latin1();
  Pargs -> sampling_step = atof( (char*)samplingStep->text().latin1() )/100;
  Pargs -> min_sample_freq = atoi((char*)minSamplingFq->text().latin1());
  Pargs -> do_wlog = m_Output -> isItemChecked( m_Idtext );
  Pargs -> verb_analysis = false;
  Pargs -> do_texture = false;
  Pargs -> signeddist = true;

  if( a2bRadio -> isChecked() )
  {
    Pargs -> do_symmetric = 1;
  }
  else if( b2aRadio -> isChecked() )
  {
    Pargs -> do_symmetric = 2;
  }
  else if( symmetricRadio -> isChecked() )
  {
    Pargs -> do_symmetric = 3;
  }

  if( SignedRadio -> isChecked() )
  {
    Pargs -> signeddist = true;
  }
  else if( AbsRadio -> isChecked() )
  {
    Pargs -> signeddist = false;
  }

  if( ! Pargs -> do_wlog )
  {
    m_Log = outbuf_new( stdio_puts , stdout );
  }
  else
  {
    m_TextOut = new TextWidget();
    m_Log = outbuf_new( TextWidget_puts , m_TextOut );
    m_TextOut->show();
  }
}

void MeshValmetControls::drawVertexErrorT( vtkSmartPointer <vtkPolyData> Mesh , model_error* Model , vtkSmartPointer <vtkActor> Actor , vtkSmartPointer <vtkRenderWindow> Win , int Tag )
{

  Mesh = vtkSmartPointer <vtkPolyData> ::New();
  vtkSmartPointer <vtkPoints> Points = vtkSmartPointer <vtkPoints> ::New();
  vtkSmartPointer <vtkCellArray> Polys = vtkSmartPointer <vtkCellArray> ::New();
  vtkSmartPointer <vtkDoubleArray> Scalars = vtkSmartPointer <vtkDoubleArray> ::New();

  int k;
  int i;
  int j;
  int jmax;
  int n;

  vertex_t u;
  vertex_t v;
  vertex_t a;
  vertex_t b;
  vertex_t c;

  face_t *cur_face;

  int i0;
  int i1;
  int i2;
  int i3;
  int j0;
  int j1;
  int j2;
  int j3;
  int l0;
  int l1;
  int l2;
  int l3;

  vertex_t v0;
  vertex_t v1;
  vertex_t v2;
  vertex_t v3;

  vtkIdType index = 0;
  double vertex[3];
  vtkIdType f[3];

  for ( k=0 ; k < Model->mesh->num_faces ; k++ )
  {
     n = Model->fe[ k ].sample_freq;
     cur_face = &( Model->mesh->faces[ k ] );

     if( n == 1 && m_Downsampling == 1)
     {
          a = Model->mesh->vertices[ cur_face->f0 ];
          b = Model->mesh->vertices[ cur_face->f1 ];
          c = Model->mesh->vertices[ cur_face->f2 ];

          v3.x = 1/3.0*( a.x + b.x + c.x );
          v3.y = 1/3.0*( a.y + b.y + c.y );
          v3.z = 1/3.0*( a.z + b.z + c.z );

          vertex[ 0 ] = a.x;
          vertex[ 1 ] = a.y;
          vertex[ 2 ] = a.z;

          Points -> InsertPoint( index , vertex );
          Scalars -> InsertTuple1( index++ , Model->verror[ cur_face->f0 ] );

          vertex[ 0 ] = b.x;
          vertex[ 1 ] = b.y;
          vertex[ 2 ] = b.z;

          Points -> InsertPoint( index , vertex );
          Scalars -> InsertTuple1( index++ , Model->verror[ cur_face->f1 ] );

          vertex[ 0 ] = v3.x;
          vertex[ 1 ] = v3.y;
          vertex[ 2 ] = v3.z;

          Points -> InsertPoint( index , vertex );
          Scalars -> InsertTuple1( index++ , Model->fe[k].serror[0] );

          f[ 0 ] = index - 3;
          f[ 1 ] = index - 2;
          f[ 2 ] = index - 1;

          Polys->InsertNextCell(3,f);

          vertex[ 0 ] = a.x;
          vertex[ 1 ] = a.y;
          vertex[ 2 ] = a.z;

          Points -> InsertPoint( index , vertex );
          Scalars -> InsertTuple1( index++ , Model->verror[ cur_face->f0 ] );

          vertex[ 0 ] = v3.x;
          vertex[ 1 ] = v3.y;
          vertex[ 2 ] = v3.z;

          Points -> InsertPoint( index , vertex);
          Scalars -> InsertTuple1( index++ , Model->fe[k].serror[0] );

          vertex[ 0 ] = c.x;
          vertex[ 1 ] = c.y;
          vertex[ 2 ] = c.z;

          Points -> InsertPoint( index , vertex);
          Scalars -> InsertTuple1( index++ , Model->verror[ cur_face->f2 ] );

          f[ 0 ] = index - 3;
          f[ 1 ] = index - 2;
          f[ 2 ] = index - 1;

          Polys->InsertNextCell(3,f);

          vertex[ 0 ] = b.x;
          vertex[ 1 ] = b.y;
          vertex[ 2 ] = b.z;

          Points -> InsertPoint( index , vertex);
          Scalars -> InsertTuple1( index++ , Model->verror[ cur_face->f1 ] );

          vertex[ 0 ] = c.x;
          vertex[ 1 ] = c.y;
          vertex[ 2 ] = c.z;

          Points -> InsertPoint( index , vertex);
          Scalars -> InsertTuple1( index++ , Model->verror[ cur_face->f2 ] );

          vertex[ 0 ] = v3.x;
          vertex[ 1 ] = v3.y;
          vertex[ 2 ] = v3.z;

          Points -> InsertPoint( index , vertex);
          scalars->InsertTuple1(index++, Model->fe[k].serror[0]);//?

          f[ 0 ] = index - 3;
          f[ 1 ] = index - 2;
          f[ 2 ] = index - 1;

          Polys->InsertNextCell(3,f);

    }
    else if( m_Downsampling >= n )
    {
          vertex[ 0 ] = Model->mesh->vertices[ cur_face->f0 ].x;
          vertex[ 1 ] = Model->mesh->vertices[ cur_face->f0 ].y;
          vertex[ 2 ] = Model->mesh->vertices[ cur_face->f0 ].z;

          Points -> InsertPoint( index , vertex);
          Scalars -> InsertTuple1( index++ , Model->verror[ cur_face->f0 ] );

          vertex[ 0 ] = Model->mesh->vertices[ cur_face->f1 ].x;
          vertex[ 1 ] = Model->mesh->vertices[ cur_face->f1 ].y;
          vertex[ 2 ] = Model->mesh->vertices[ cur_face->f1 ].z;

          Points -> InsertPoint( index , vertex);
          Scalars -> InsertTuple1( index++ , Model->verror[ cur_face->f1 ] );

          vertex[ 0 ] = Model->mesh->vertices[ cur_face->f2 ].x;
          vertex[ 1 ] = Model->mesh->vertices[ cur_face->f2 ].y;
          vertex[ 2 ] = Model->mesh->vertices[ cur_face->f2 ].z;

          Points -> InsertPoint( index , vertex);
          Scalars -> InsertTuple1( index++ , Model->verror[ cur_face->f2 ] );

          f[ 0 ] = index - 3;
          f[ 1 ] = index - 2;
          f[ 2 ] = index - 1;

          Polys->InsertNextCell(3,f);
    }
    else
    {
          a = Model->mesh->vertices[ cur_face->f0 ];
          b = Model->mesh->vertices[ cur_face->f1 ];
          c = Model->mesh->vertices[ cur_face->f2 ];

          substract_v( &b , &a , &u );
          substract_v( &c , &a , &v );

          prod_v( 1/(float)(n-1), &u , &u );
          prod_v( 1/(float)(n-1), &v , &v );

          for( i = 0 ; i < n-1 ; i += m_Downsampling )
          {
             i2 = ( i + m_Downsampling < n) ? i + m_Downsampling : n-1;

             for( j = 0 , jmax = n - i - 1 ; j < jmax ; j += m_Downsampling )
             {
                  if( i + j + m_Downsampling < n )
                  {
                    i0 = i;
                    j0 = j;
                    i1 = i + m_Downsampling;
                    j1 = j;
                    i2 = i;
                    j2 = j + m_Downsampling;
                    i3 = i1;
                    j3 = j2;
                  }
                  else
                  {
                    i2 = i;
                    j2 = j;
                    i0 = ( i + m_Downsampling < n) ? i + m_Downsampling : n-1;
                    j0 = ( j > 0 ) ? j - m_Downsampling : j;
                    i1 = i0;
                    j1 = n - 1 - i1;
                    i3 = i;
                    j3 = n - 1 - i3;
                  }

                  l0 = j0 + i0 * ( 2*n - i0 + 1 )/2;
                  l1 = j1 + i1 * ( 2*n - i1 + 1 )/2;
                  l2 = j2 + i2 * ( 2*n - i2 + 1 )/2;

                  v0.x = a.x + i0*u.x + j0*v.x;
                  v0.y = a.y + i0*u.y + j0*v.y;
                  v0.z = a.z + i0*u.z + j0*v.z;
                  v1.x = a.x + i1*u.x + j1*v.x;
                  v1.y = a.y + i1*u.y + j1*v.y;
                  v1.z = a.z + i1*u.z + j1*v.z;
                  v2.x = a.x + i2*u.x + j2*v.x;
                  v2.y = a.y + i2*u.y + j2*v.y;
                  v2.z = a.z + i2*u.z + j2*v.z;

                  if( i0 != i1 || j0 != j1)
                  {
                    vertex[ 0 ] = v0.x;
                    vertex[ 1 ] = v0.y;
                    vertex[ 2 ] = v0.z;

                    Points -> InsertPoint( index , vertex );
                    Scalars -> InsertTuple1( index++ , Model->fe[k].serror[ l0 ] );

                    vertex[ 0 ] = v1.x;
                    vertex[ 1 ] = v1.y;
                    vertex[ 2 ] = v1.z;

                    Points -> InsertPoint( index , vertex );
                    Scalars -> InsertTuple1( index++ , Model->fe[k].serror[ l1 ] );

                    vertex[ 0 ] = v2.x;
                    vertex[ 1 ] = v2.y;
                    vertex[ 2 ] = v2.z;

                    Points -> InsertPoint( index , vertex );
                    Scalars -> InsertTuple1( index++ , Model->fe[k].serror[ l2 ] );

                    f[ 0 ] = index - 3;
                    f[ 1 ] = index - 2;
                    f[ 2 ] = index - 1;

                    Polys -> InsertNextCell(3,f);
                  }

                  if ( i3 + j3 < n )
                  {
                    l3 = j3 + i3 * ( 2*n - i3 + 1 )/2;
                    v3.x = a.x + i3*u.x + j3*v.x;
                    v3.y = a.y + i3*u.y + j3*v.y;
                    v3.z = a.z + i3*u.z + j3*v.z;

                    vertex[ 0 ] = v3.x;
                    vertex[ 1 ] = v3.y;
                    vertex[ 2 ] = v3.z;

                    Points -> InsertPoint( index , vertex );
                    Scalars -> InsertTuple1( index++ , Model->fe[k].serror[ l3 ] );

                    vertex[ 0 ] = v2.x;
                    vertex[ 1 ] = v2.y;
                    vertex[ 2 ] = v2.z;

                    Points -> InsertPoint( index , vertex );
                    Scalars -> InsertTuple1( index++ , Model->fe[k].serror[ l2 ] );

                    vertex[ 0 ] = v1.x;
                    vertex[ 1 ] = v1.y;
                    vertex[ 2 ] = v1.z;

                    Points -> InsertPoint( index , vertex );
                    Scalars -> InsertTuple1( index++ , Model->fe[k].serror[ l1 ] );

                    f[ 0 ] = index - 3;
                    f[ 1 ] = index - 2;
                    f[ 2 ] = index - 1;

                    Polys -> InsertNextCell(3,f);
                  }
              }
         }
    }
  }

  Mesh -> SetPoints( Points );
  Mesh -> SetPolys( Polys );

  if( Tag == 0 )
  {
      Mesh -> GetPointData()-> SetScalars( Scalars );
  }

  vtkSmartPointer <vtkPolyDataMapper> Mapper = vtkSmartPointer <vtkPolyDataMapper> ::New();
  Mapper -> SetInput( Mesh );

  double Mmax;
  double Mmin;
  int inter = IntervalSpinBox -> value();

  Mmax = m_Dmax;
  Mmin = m_Dmin;

  if( Tag == 0 )
  {
    m_Lut = vtkSmartPointer <vtkColorTransferFunction> ::New();

    m_Lut -> AddRGBPoint( m_Middle , 0 , 1 , 0 );

    if( Mmax - m_Middle > 0 && Mmin - m_Middle >= 0 )
    {
      int i;
      for( i = 1 ; i <= inter ; i++ )
      {
        m_Lut -> AddRGBPoint( (float)( m_Middle + ( Mmax - m_Middle )*i/(double)inter) , m_Lookuptable[128-i*22].R/(float)255 ,
                m_Lookuptable[128-i*22].G/(float)255 , m_Lookuptable[128-i*22].B/(float)255 );
      }
    }
    else if( Mmax - m_Middle <= 0 && Mmin - m_Middle < 0 )
    {
      int i;
      for(i = 1; i <=inter; i++)
      {
        m_Lut -> AddRGBPoint( (float)( m_Middle + ( Mmin - m_Middle )*i/(double)inter ) , m_Lookuptable[128+i*22].R/(float)255 ,
                m_Lookuptable[128+i*22].G/(float)255 , m_Lookuptable[128+i*22].B/(float)255);
      }
    }
    else if( Mmax - m_Middle > 0 && Mmin - m_Middle < 0 )
    {
      int i;
      for( i = 1 ; i <= inter ; i++ )
      {
        m_Lut -> AddRGBPoint( (float)( m_Middle + ( Mmax - m_Middle )*i/(double)inter ) , m_Lookuptable[128-i*22].R/(float)255 ,
                m_Lookuptable[128-i*22].G/(float)255 , m_Lookuptable[128-i*22].B/(float)255);
      }

      for( i = 1 ; i <= inter ; i++ )
      {
        m_Lut -> AddRGBPoint( (float)(m_Middle + ( Mmin - m_Middle )*i/(double)inter ), m_Lookuptable[128+i*22].R/(float)255 ,
                m_Lookuptable[128+i*22].G/(float)255 , m_Lookuptable[128+i*22].B/(float)255);
      }
    }

    Mapper -> SetLookupTable( m_Lut );
  }

  Actor -> SetMapper( Mapper );

  //Win -> updateGL();

}

void MeshValmetControls::DoStat()
{
  double *serror;
  double *allserror;
  int i;
  int n;
  
  m_AbsSumError = 0;
  m_AbsSumSqrError = 0;
  m_SumError = 0;
  m_ErrorMax = -100000;
  m_SumSquareDeviations = 0;
  m_AbsSumSquareDeviations = 0;

  double percent95= 0.95;
  double percent68 = 0.68;
  double percent75 = 0.75;
  double percent50 = 0.5;
  double percent25 = 0.25;

  m_Percentile95 = 0;
  m_Percentile75 = 0;
  m_Percentile68 = 0;
  m_Percentile25 = 0;
  m_Median = 0;
  
  if( m_Pargs.do_symmetric == 1)
  {
    n = m_Model1.n_samples;
    serror = m_Model1.fe[0].serror;

    for ( i = 0 ; i < n ; i++ )
    { 
       m_SumError += serror[i];
       m_AbsSumError += fabs( serror[i] );
       m_AbsSumSqrError += serror[i]*serror[i];

       if( m_ErrorMax < serror[i] )
       {
           m_ErrorMax = serror[i];
       }
    }

    if( m_Pargs.signeddist )
    {
        m_MeanError = m_SumError / n;
    }
    else
    {
      m_MeanError = m_AbsSumError / n;
    }

    m_MadError = m_AbsSumError / n;
    m_MsdError = m_AbsSumSqrError /n;
      
    for ( i = 0 ; i < n ; i++ )
    {
        m_SumSquareDeviations += ( serror[i] - m_MeanError )*( serror[i] - m_MeanError );
        m_AbsSumSquareDeviations += ( fabs( serror[i] ) - m_MadError )*( fabs( serror[i] ) - m_MadError );
    }

    if( m_Pargs.signeddist )
    {
      m_SigmaError = sqrt( m_SumSquareDeviations/(n-1) );
    }
    else
    {
      m_SigmaError = sqrt( m_AbsSumSquareDeviations/(n-1) );
    }

    m_AbsSigmaError = sqrt( m_AbsSumSquareDeviations/(n-1) );

    allserror = new double[n];

    if( m_Pargs.signeddist )
    {
      memcpy( allserror , serror , n*sizeof(double) );
    }
    else
    {
      for ( i = 0 ; i < n ; i++ )
      {
        allserror[i] = fabs( serror[i] );
      }
    }

    qsort( (void *)allserror , n , sizeof(double) , compare_doubles );

    m_Percentile25 = allserror[ (int)(n*percent25+1) ];
    m_Median = allserror[ (int)(n*percent50+1) ];
    m_Percentile68 = allserror[ (int)(n*percent68+1) ];
    m_Percentile75 = allserror[ (int)(n*percent75+1) ];
    m_Percentile95 = allserror[ (int)(n*percent95+1) ];

    delete [] allserror;   
  }
  else if( m_Pargs.do_symmetric == 2 )
  {
    n = m_Model2.n_samples;
    serror = m_Model2.fe[0].serror;

    for ( i = 0 ; i < n ; i++ )
    {
       m_SumError += serror[i];
       m_AbsSumError += fabs( serror[i] );
       m_AbsSumSqrError += serror[i]*serror[i];

       if( m_ErrorMax < serror[i] )
       {
           m_ErrorMax = serror[i];
       }
    }

    if( m_Pargs.signeddist )
    {
        m_MeanError = m_SumError / n;
    }
    else
    {
      m_MeanError = m_AbsSumError / n;
    }

    m_MadError = m_AbsSumError / n;
    m_MsdError = m_AbsSumSqrError /n;

    for ( i = 0 ; i < n ; i++ )
    {
        m_SumSquareDeviations += ( serror[i] - m_MeanError )*( serror[i] - m_MeanError );
        m_AbsSumSquareDeviations += ( fabs( serror[i] ) - m_MadError )*( fabs( serror[i] ) - m_MadError );
    }

    if( m_Pargs.signeddist )
    {
      m_SigmaError = sqrt( m_SumSquareDeviations/(n-1) );
    }
    else
    {
      m_SigmaError = sqrt( m_AbsSumSquareDeviations/(n-1) );
    }

    m_AbsSigmaError = sqrt( m_AbsSumSquareDeviations/(n-1) );

    allserror = new double[n];

    if( m_Pargs.signeddist )
    {
      memcpy( allserror , serror , n*sizeof(double) );
    }
    else
    {
      for ( i = 0 ; i < n ; i++ )
      {
        allserror[i] = fabs( serror[i] );
      }
    }

    qsort( (void *)allserror , n , sizeof(double) , compare_doubles );

    m_Percentile25 = allserror[ (int)(n*percent25+1) ];
    m_Median = allserror[ (int)(n*percent50+1) ];
    m_Percentile68 = allserror[ (int)(n*percent68+1) ];
    m_Percentile75 = allserror[ (int)(n*percent75+1) ];
    m_Percentile95 = allserror[ (int)(n*percent95+1) ];

    delete [] allserror;
  }
  else if( m_Pargs.do_symmetric == 3)
  {
    int n1;
    int n2;

    n1 = m_Model1.n_samples;
    serror = m_Model1.fe[0].serror;

    for (i = 0 ; i < n1 ; i++ )
    { 
        m_SumError += serror[i];
        m_AbsSumError += fabs( serror[i] );
        m_AbsSumSqrError += serror[i]*serror[i];

        if( m_ErrorMax < serror[i] )
        {
            m_ErrorMax = serror[i];
        }
    }

    n2 = m_Model2.n_samples;
    serror = m_Model2.fe[0].serror;
    
    for ( i = 0 ; i < n2 ; i++ )
    {
        m_SumError += serror[i];
        m_AbsSumError += fabs( serror[i] );
        m_AbsSumSqrError += serror[i]*serror[i];

        if( m_ErrorMax < serror[i] )
        {
            m_ErrorMax = serror[i];
        }
    }

    n = n1 + n2;

    if( m_Pargs.signeddist )
    {
      m_MeanError = m_SumError / n;
    }
    else
    {
      m_MeanError = m_AbsSumError / n;
    }

    m_MadError = m_AbsSumError / n;
    m_MsdError = m_AbsSumSqrError /n;
        
    serror = m_Model1.fe[0].serror;

    for( i = 0 ; i < n1 ; i++ )
    {
        m_SumSquareDeviations += ( serror[i]- m_MeanError )*( serror[i] - m_MeanError );
        m_AbsSumSquareDeviations += ( fabs( serror[i] ) - m_MadError )*( fabs( serror[i] ) - m_MadError );
    }

    serror = m_Model2.fe[0].serror;

    for( i = 0 ; i < n2 ; i++ )
    {
      m_SumSquareDeviations += ( serror[i] - m_MeanError )*( serror[i] - m_MeanError );
      m_AbsSumSquareDeviations += ( fabs( serror[i] ) - m_MadError )*( fabs( serror[i] ) - m_MadError );
    }

    if( m_Pargs.signeddist )
    {
      m_SigmaError = sqrt( m_SumSquareDeviations/(n-1) );
    }
    else
    {
      m_SigmaError = sqrt( m_AbsSumSquareDeviations/(n-1) );
    }

    m_AbsSigmaError = sqrt( m_AbsSumSquareDeviations/(n-1) );

    allserror = new double[n];

    if( m_Pargs.signeddist )
    {
      memcpy( allserror , m_Model1.fe[0].serror , n1*sizeof(double) );
      memcpy( &allserror[n1] , m_Model2.fe[0].serror , n2*sizeof(double) );
    }
    else
    {
      serror = m_Model1.fe[0].serror;

      for( i = 0 ; i < n1 ; i++ )
      {
        allserror[i] = fabs( serror[i] );
      }

      serror = m_Model2.fe[0].serror;

      for( i = n1 ; i < n1+n2 ;  i++ )
      {
        allserror[i] = fabs( serror[ i-n1 ] );
      }
    }

    qsort( (void *)allserror , n , sizeof(double) , compare_doubles );

    m_Percentile25 = allserror[ (int)(n*percent25+1) ];
    m_Median = allserror[ (int)(n*percent50+1) ];
    m_Percentile68 = allserror[ (int)(n*percent68+1) ];
    m_Percentile75 = allserror[ (int)(n*percent75+1) ];
    m_Percentile95 = allserror[ (int)(n*percent95+1) ];

    delete [] allserror;
  }
  
  if( m_Pargs.signeddist )
  {
    if( m_Pargs.do_symmetric == 1)
    {
      m_FaceMeanError = m_Stats.mean_dist;
      m_DistVolume = m_Stats.abs_mean_tot;
      m_FaceRmsError = m_Stats.rms_dist;
    }
    else if( m_Pargs.do_symmetric == 2)
    {
      m_FaceMeanError = m_StatsRev.mean_dist;
      m_DistVolume = m_StatsRev.abs_mean_tot;
      m_FaceRmsError = m_StatsRev.rms_dist;
    }
    else if( m_Pargs.do_symmetric == 3)
    {
      m_FaceMeanError = ( m_Stats.mean_tot + m_StatsRev.mean_tot ) / ( m_Stats.st_m1_area + m_StatsRev.st_m1_area );
      m_DistVolume = ( m_Stats.abs_mean_tot + m_StatsRev.abs_mean_tot ) / 2.0;
      m_FaceRmsError = sqrt( ( m_Stats.rms_tot + m_StatsRev.rms_tot ) / ( m_Stats.st_m1_area + m_StatsRev.st_m1_area ) );
    }
  }
  else
  {
    if( m_Pargs.do_symmetric == 1)
    {
      m_FaceMeanError = m_Stats.abs_mean_dist;
      m_DistVolume = m_Stats.abs_mean_tot;
      m_FaceRmsError = m_Stats.abs_rms_dist;
    }
    else if( m_Pargs.do_symmetric == 2)
    {
      m_FaceMeanError = m_StatsRev.abs_mean_dist;
      m_DistVolume = m_StatsRev.abs_mean_tot;
      m_FaceRmsError = m_StatsRev.abs_rms_dist;
    }
    else if( m_Pargs.do_symmetric == 3)
    {
      m_FaceMeanError = ( m_Stats.abs_mean_tot + m_StatsRev.abs_mean_tot ) / ( m_Stats.st_m1_area + m_StatsRev.st_m1_area );
      m_DistVolume = ( m_Stats.abs_mean_tot + m_StatsRev.abs_mean_tot ) / 2.0;
      m_FaceRmsError = sqrt( ( m_Stats.abs_rms_tot + m_StatsRev.abs_rms_tot ) / ( m_Stats.st_m1_area + m_StatsRev.st_m1_area ) );
    }
  }
  
}

void MeshValmetControls::OutputStatInfo()
{
  if( m_Pargs.signeddist )
  {
    StdMeanEdit -> setText( QString::number( m_MeanError ) );
    SigmaEdit -> setText( QString::number( m_SigmaError ) );
  }
  else
  {
    StdMeanEdit -> setText( QString::number( m_MadError ) );
    SigmaEdit -> setText( QString::number( m_AbsSigmaError ) );
  }

  MeanEdit -> setText( QString::number( m_FaceMeanError ) );
  RMSEdit -> setText( QString::number( m_FaceRmsError ) );

  MedianEdit -> setText( QString::number( m_Median ) );
  Percentile95Edit -> setText( QString::number( m_Percentile95 ) );
  Percentile75Edit -> setText( QString::number( m_Percentile75 ) );
  Percentile68Edit -> setText( QString::number( m_Percentile68 ) );
  Percentile25Edit -> setText( QString::number( m_Percentile25 ) );

  DiceEdit -> setText( QString::number( m_DiceCoefficient[0] ) );
  IntUnionEdit -> setText( QString::number( m_IntUnionRatio[0] ) );

  m_HausdorffError = fabs( m_Dmax ) > fabs( m_Dmin ) ? fabs( m_Dmax ) : fabs( m_Dmin );
  HausdorffEdit -> setText( QString::number( m_HausdorffError ) );
  MSDEdit -> setText( QString::number( m_MsdError ) );
  MADEdit -> setText( QString::number( m_MadError ) );

  if( m_Pargs.do_symmetric == 1)
  {
    if( m_Pargs.signeddist)
    {
        MinEdit -> setText( QString::number( m_Model1.min_error ) );
        MaxEdit -> setText( QString::number( m_Model1.max_error ) );
    }
    else
    {
        MinEdit -> setText( QString::number( m_Model1.abs_min_error ) );
        MaxEdit -> setText( QString::number( m_Model1.abs_max_error ) );
    }
  }
  else if( m_Pargs.do_symmetric == 2 )
  {
    if( m_Pargs.signeddist )
    {
        MinEdit -> setText( QString::number( m_Model2.min_error ) );
        MaxEdit -> setText( QString::number( m_Model2.max_error ) );
    }
    else
    {
        MinEdit -> setText( QString::number( m_Model2.abs_min_error ) );
        MaxEdit -> setText( QString::number( m_Model2.abs_max_error ) );
    }
  }
  else if( m_Pargs.do_symmetric == 3)
  {
    MinEdit -> setText( QString::number( m_Dmin ) );
    MaxEdit -> setText( QString::number( m_Dmax ) );
  }
}

void MeshValmetControls::DoHistogram()
{
    double drange;
    double off;
    double *serror;
    int i;
    int bin_idx;
    int n;
    int len = m_CmapLen;

    QApplication::setOverrideCursor( Qt::waitCursor );

    delete [] m_Histogram1;
    delete [] m_Histogram2;
    delete [] m_Hist;

    m_Histogram1 = new int[len];
    m_Histogram2 = new int[len];
    hist = new int[len];

    memset(m_Histogram1, 0, sizeof(*m_Histogram1)*len);
    memset(m_Histogram2, 0, sizeof(*m_Histogram2)*len);
    memset(m_Hist, 0, sizeof(*hist)*len);

  if(m_Pargs.do_symmetric == 1 || m_Pargs.do_symmetric == 3)
  {
    n = m_Model1.n_samples;
    if(m_Pargs.signeddist == true)
    {
      drange = m_Model1.max_error-m_Model1.min_error;
      off = m_Model1.min_error;
      serror = m_Model1.fe[0].serror;

      for (i=0; i<n; i++)
      {
         bin_idx = (int) ((serror[i]-off)/drange*len);
          if (bin_idx >= len)
          {
              bin_idx = len-1;
          }
          m_Histogram1[bin_idx]++;
      }
    }
    else
    {
      drange = m_Model1.abs_max_error-m_Model1.abs_min_error;
      off = m_Model1.abs_min_error;
      serror = m_Model1.fe[0].serror;
      for (i=0; i<n; i++)
      {
         bin_idx = (int) ((fabs(serror[i])-off)/drange*len);
          if (bin_idx >= len)
          {
              bin_idx = len-1;
          }
          m_Histogram1[bin_idx]++;
      }
    }
  }

  if(m_Pargs.do_symmetric == 2 || m_Pargs.do_symmetric == 3)
  {
    n = m_Model2.n_samples;
    if( m_Pargs.signeddist == true)
    {
      drange = m_Model2.max_error-m_Model2.min_error;
      off = m_Model2.min_error;
      serror = m_Model2.fe[0].serror;
      for (i=0; i<n; i++)
      {
         bin_idx = (int) ((serror[i]-off)/drange*len);
          if (bin_idx >= len)
          {
              bin_idx = len-1;
          }
          m_Histogram2[bin_idx]++;
      }
    }
    else
    {
        drange = m_Model2.abs_max_error-m_Model2.abs_min_error;
      off = m_Model2.abs_min_error;
      serror = m_Model2.fe[0].serror;
      for (i=0; i<n; i++)
      {
         bin_idx = (int) ((fabs(serror[i])-off)/drange*len);
          if (bin_idx >= len)
          {
              bin_idx = len-1;
          }
          m_Histogram2[bin_idx]++;
      }
    }
  }

  if( m_Pargs.do_symmetric == 3)
  {
    for(i=0; i<len; i++)
    {
      m_Hist[i] = m_Histogram1[i] + m_Histogram2[i];
    }
  }

  QApplication::restoreOverrideCursor();
}

void MeshValmetControls::SelectStep(const QString& step)
{
  samplingStep->setText(step);  
}

void MeshValmetControls::ChangeSamplingSlider()
{
  if(!m_Computed)
    return;
  m_Downsampling = DownsamplingSlider->value();
  if(m_Pargs.do_symmetric == 1 || m_Pargs.do_symmetric == 3)
    if(ColormapCheckbox1->isChecked())
          drawVertexErrorT(m_Mesh1, &m_Model1, m_ModelActor1, renWin1);
        else
          drawVertexErrorT(m_Mesh1, &m_Model1, m_ModelActor1, renWin1, 1);
    if(m_Pargs.do_symmetric == 2 || m_Pargs.do_symmetric == 3)
      if(ColormapCheckbox2->isChecked())
          drawVertexErrorT(m_Mesh2, &m_Model2, m_ModelActor2, renWin2);
        else
          drawVertexErrorT(m_Mesh2, &m_Model2, m_ModelActor2, renWin2, 1);
}

void MeshValmetControls::ColormapCheckbox1Changed(bool checked)
{
  if(!m_Computed)
    return;
  if(checked)
    drawVertexErrorT(m_Mesh1, &m_Model1, m_ModelActor1, renWin1, 0);
  else
    drawVertexErrorT(m_Mesh1, &m_Model1, m_ModelActor1, renWin1, 1);
}

void MeshValmetControls::ColormapCheckbox2Changed(bool checked)
{
  if(!m_Computed)
    return;
  if(checked)
    drawVertexErrorT(m_Mesh2, &m_Model2, m_ModelActor2, renWin2, 0);
  else
    drawVertexErrorT(m_Mesh2, &m_Model2, m_ModelActor2, renWin2, 1);
}

void MeshValmetControls::UpdateHistogram()
{
  xyplot->SetXRange(HistXMin->text().toDouble(),HistXMax->text().toDouble());
  xyplot->SetYRange(HistYMin->text().toDouble(),HistYMax->text().toDouble());
  renWin->update();
    
}

void MeshValmetControls::UpdateColor()
{
  double Mmax;
  double Mmin;
  if(ColormapMax->text() != "")
    Mmax = ColormapMax->text().toDouble();
  else
      Mmax = m_Dmax;
  if(ColormapMin->text() != "")
      Mmin = ColormapMin->text().toDouble();
  else
      Mmin = m_Dmin;
  int inter = IntervalSpinBox->value();
  
  m_Lut = vtkSmartPointer <vtkColorTransferFunction> ::New();
    
  m_Lut->AddRGBPoint(m_Middle,0,1,0);

    if(Mmax-m_Middle>0 && Mmin-m_Middle>=0)
    {
      int i;
      for(i = 1; i <=inter; i++)
        m_Lut->AddRGBPoint((float)(middle+(Mmax-m_Middle)*i/(double)inter),m_Lookuptable[128-i*22].R/(float)255,m_Lookuptable[128-i*22].G/(float)255,m_Lookuptable[128-i*22].B/(float)255);

    }
    else if(max-m_Middle<=0 && Mmin-m_Middle<0)
    {
      int i;
      for(i = 1; i <=inter; i++)
        m_Lut->AddRGBPoint((float)(middle+(Mmin-m_Middle)*i/(double)inter),m_Lookuptable[128+i*22].R/(float)255,m_Lookuptable[128+i*22].G/(float)255,m_Lookuptable[128+i*22].B/(float)255);
    }
    else if(max-m_Middle>0 && Mmin-m_Middle<0)
    {
      int i;
      for(i = 1; i <=inter; i++)
      {
        m_Lut->AddRGBPoint((float)(middle+(Mmax-m_Middle)*i/(double)inter),m_Lookuptable[128-i*22].R/(float)255,m_Lookuptable[128-i*22].G/(float)255,m_Lookuptable[128-i*22].B/(float)255);
      }
        
      for(i = 1; i <=inter; i++)
        m_Lut->AddRGBPoint((float)(middle+(Mmin-m_Middle)*i/(double)inter),m_Lookuptable[128+i*22].R/(float)255,m_Lookuptable[128+i*22].G/(float)255,m_Lookuptable[128+i*22].B/(float)255);
  
    }
    
    vtkSmartPointer <vtkPolyDataMapper> mapper;
    
    if(m_Pargs.do_symmetric == 1 || m_Pargs.do_symmetric == 3)
    {
      mapper = (vtkSmartPointer <vtkPolyDataMapper>)m_ModelActor1->GetMapper();
      mapper->SetLookupTable(m_Lut);
      renWin1->update();
    }
    if(m_Pargs.do_symmetric == 2 || m_Pargs.do_symmetric == 3)
    {
      mapper = (vtkSmartPointer <vtkPolyDataMapper>)m_ModelActor2->GetMapper();
      mapper->SetLookupTable(m_Lut);
      renWin2->update();
    }
    
}

void MeshValmetControls::middleSliderMoved(int pos)
{
  double range = fabs(m_Dmax)>fabs(m_Dmin)?fabs(m_Dmax):fabs(m_Dmin);
  m_Middle = range*(double)middleSlider->value()/110;
  TextLabel3->setText(QString::number(m_Middle));
}

int compare_doubles (const void * a, const void * b)
{
  double x = *((double *)a);
  double y = *((double *)b);

  if(x>y)
    return 1;
  else if(x<y)
    return -1;
  else
    return 0;
}
