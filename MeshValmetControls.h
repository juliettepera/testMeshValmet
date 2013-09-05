/* Author: Christine Xu, University of North Carolina at Chapel Hill*/

#ifndef MESHVALMETCONTROLS_H
#define MESHVALEMTCONTROLS_H

// other libraries
#include <iostream>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

// qt libraries
#include <qdir.h>
#include <qfiledialog.h>
#include <qstring.h>
#include <qfileinfo.h>
#include <qmessagebox.h>
#include <qcheckbox.h>
#include <qslider.h>
#include <qspinbox.h>
#include <qradiobutton.h>
#include <qtextstream.h>
#include <qlineedit.h>
#include <qfile.h>
#include <qtoolbutton.h>
#include <qlistbox.h>
#include <qdialog.h>
#include <qstringlist.h>
#include <qprogressdialog.h>
#include <qmenubar.h>
#include <qpopupmenu.h>

// vtk libraries
#include <vtkTextProperty.h>
#include <vtkAxisActor2D.h>
#include <vtkInteractorStyleSwitch.h>
#include <vtkImplicitBoolean.h>
#include <vtkClipPolyData.h>
#include "vtkActor.h"
#include "vtkBYUReader.h"
#include "vtkCamera.h"
#include "vtkCellArray.h"
#include "vtkColorTransferFunction.h"
#include "vtkCoordinate.h"
#include "vtkDataSet.h"
#include "vtkDoubleArray.h"
#include "vtkFloatArray.h"
#include "vtkLookupTable.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkPoints.h"
#include "vtkProperty.h"
#include "vtkProperty2D.h"
#include "vtkRenderer.h"
#include "vtkVRMLExporter.h"
#include "vtkXYPlotActor.h"
#include "vtkSmartPointer.h"


// meshvalmet libraries
#include "3dmodel.h"
#include "mesh_run.h"
#include "reporting.h"
#include "compute_volume_overlap.h"
#include "MeshValmetGUI.h"
#include "TextWidget.h"
#include "geomutils.h"

//#include <assert.h>
//#include <ColormapWidget.h>
//#include "vtkQtRenderWindow.h"
//#include "vtkQtRenderWindowInteractor.h"
//#include <read_model.h>




class MeshValmetControls : public MeshValmetGUI
{


public:

  QMenuBar* m_Menubar;
  QPopupMenu *m_Output;
  QPopupMenu *m_View;
  QPopupMenu *m_Comp;

  int m_Idtext;
  int m_Idhist;
  int m_Idcomphist;
  int m_Idsyn;
  int m_Idoverlay;
  int m_Iderror;
  int m_Idverror;
  int m_Idstat;
  int m_Idreset;
  int m_Idbg;

  QString m_CommonPath;
  QString m_FileName1;
  QString m_FileName2;

  int m_Model1Line;
  int m_Model2Line;
  
  vtkSmartPointer <vtkRenderer> m_Renderer1;
  vtkSmartPointer <vtkRenderer> m_Renderer2;
  vtkSmartPointer <vtkRenderer> m_Renderer;

  vtkSmartPointer <vtkRenderWindowInteractor> m_Iren1;
  vtkSmartPointer <vtkRenderWindowInteractor> m_Iren2;
  vtkSmartPointer <vtkRenderWindowInteractor> m_Iren;
  
  vtkSmartPointer <vtkActor> m_ModelActor1;
  vtkSmartPointer <vtkActor> m_ModelActor11;
  vtkSmartPointer <vtkActor> m_ModelActor2;
  vtkSmartPointer <vtkActor> m_ModelActor22;

  int m_Downsampling;

  int m_BgColor[3];

  int *m_Histogram1;
  int *m_Histogram2;
  int *m_Hist;

  int m_CmapLen;

  double m_MeanError;
  double m_SumError;
  double m_SumSquareDeviations;
  double m_AbsSumSquareDeviations;
  double m_SigmaError;
  double m_AbsSigmaError;
  double m_ErrorMax;

  double m_FaceMeanError;
  double m_FaceRmsError;

  double m_Median;
  double m_Percentile95;
  double m_Percentile75;
  double m_Percentile68;
  double m_Percentile25;

  double m_DiceCoefficient[1];
  double m_IntUnionRatio[1];

  bool m_Computed;

  double m_Middle;

  struct args m_Pargs;

  vtkSmartPointer <vtkPolyData> m_Mesh1;
  vtkSmartPointer <vtkPolyData> m_Mesh2;

  double m_Dmax;
  double m_Dmin;

  double m_HausdorffError;

  double m_AbsSumError;
  double m_AbsSumSqrError;
  double m_MadError;
  double m_MsdError;
  double m_DistVolume;

  struct model_error m_Model1;
  struct model_error m_Model2;

  struct outbuf *m_Log;

  struct dist_surf_surf_stats m_Stats;
  struct dist_surf_surf_stats m_StatsRev;

  double m_AbsSamplingStep;
  double m_AbsSamplingDens;

  TextWidget* m_TextOut;

  vtkSmartPointer <vtkColorTransferFunction> m_Lut;

  vtkSmartPointer <vtkXYPlotActor> xyplot;

private:

  struct look
      {
          float R;
          float G;
          float B;
      };

  look m_Lookuptable[256];

public:

  // constructor and destructor
  MeshValmetControls( QWidget * parent = 0, const char* name = 0);
  ~MeshValmetControls();

  //slots
  void Model1Open();
  void Model2Open();
  void SaveModel1();
  void SaveModel2();
  void CheckTextWindow();
  void OutputHistogram();
  void OutputRawErrors();
  void OutputVertexErrors();
  void OutputStats();
  void Compute();
  void ComputeHistogram();
  void SwitchBackground();
  void ResetCameras();
  void SwitchSync();
  void SwitchOverlay();
  void Model1Switch();
  void Model2Switch();
  void AboutMeshValmet();

  //functions
  vtkSmartPointer <vtkPolyData> BuildMeshFromModel( model* CurrModel );
  void SetupWindow( vtkSmartPointer <vtkPolyData> Mesh , vtkSmartPointer <vtkRenderer> Renderer , vtkSmartPointer <vtkActor> Actor );
  void SaveMeshToIV( vtkSmartPointer<vtkRenderWindow> Win, QString Name );
  void SaveHist( QString FileName );
  void SaveRawErrors( QString OutFile );
  void SaveVertexErrors( QString OutFile );
  void SaveStats( QString OutFile );
  void MeshSetup(struct args* Pargs);
  void drawVertexErrorT( vtkSmartPointer <vtkPolyData> Mesh , model_error* Model , vtkSmartPointer <vtkActor> Actor , vtkSmartPointer <vtkRenderWindow> Win , int Tag = 0 );//0: colormap 1: no colormap
  void DoStat();
  void OutputStatInfo();
  void DoHistogram();


  void UpdateHistogram();
  void SelectStep(const QString& step);
  void ChangeSamplingSlider();
  void ColormapCheckbox1Changed(bool checked);
  void ColormapCheckbox2Changed(bool checked);
  void UpdateColor();
  void middleSliderMoved(int pos);

};

extern "C"
{
  void QT_prog( void *out , int p );
}

#endif
