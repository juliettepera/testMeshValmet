#ifndef TESTMESHVALMET_H
#define TESTMESHVALMET_H

// Vtk Libraries
#include <QVTKWidget.h>
#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkPolyDataReader.h>
#include <vtkCellArray.h>
#include <vtkPoints.h>
#include <vtkIdTypeArray.h>
#include <vtkPolyDataMapper.h>
#include <vtkCamera.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkPolyDataNormals.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkDoubleArray.h>
#include <vtkColorTransferFunction.h>
#include <vtkPointData.h>
#include <vtkFloatArray.h>

// Qt Libraries
#include <QMainWindow>
#include <QFileDialog>
#include <QObject>
#include <QProgressDialog>

// Other Libraries
#include <iostream>
#include <string>

// MeshValmet Libraries
#include "MeshValmet/mesh_run.h"

// My Libraries
#include "ui_testMeshValmetGui.h"

class testMeshValmet: public QMainWindow , public Ui::MainWindow
{
    Q_OBJECT

    public:
        testMeshValmet( QWidget * parent = 0 , Qt::WFlags f = 0 );

        void SetupWindow( vtkSmartPointer<vtkPolyData> Data );
        void UpdateArgs();
        vtkSmartPointer <vtkPolyData> ConvertToData( QString File );
        friend void mesh_run(const args *args, model_error *model1, model_error *model2, outbuf *out, prog_reporter *progress, dist_surf_surf_stats *stats, dist_surf_surf_stats *stats_rev, double *abs_sampling_step, double *abs_sampling_dens);
        vtkSmartPointer <vtkActor> drawVertexErrorT( vtkSmartPointer <vtkPolyData> Data , model_error* ModelError );


    public slots:
        void LoadFile();
        void InitializeWindow();
        void CalculateError();
        void DisplayError();

    private:
        QVTKWidget *m_WidgetMesh;

        int m_NumberLoading;
        int m_ActivModel;

        QString m_FileName1;
        QString m_FileName2;

        vtkSmartPointer <vtkPolyData> m_Data1;
        vtkSmartPointer <vtkPolyData> m_Data2;

        vtkSmartPointer <vtkRenderWindow> m_RenderWindow;
        vtkSmartPointer <vtkRenderer> m_Renderer;
        vtkSmartPointer <vtkCamera> m_Camera;

        vtkSmartPointer <vtkActor> m_Actor1;
        vtkSmartPointer <vtkActor> m_Actor2;

        vtkSmartPointer <vtkPolyDataMapper> m_Mapper1;
        vtkSmartPointer <vtkPolyDataMapper> m_Mapper2;

        struct args m_Pargs;

        struct model_error m_ModelError1;
        struct model_error m_ModelError2;

        struct outbuf *m_Out;
        struct prog_reporter m_Progress;
        struct dist_surf_surf_stats m_Stats;
        struct dist_surf_surf_stats m_StatsRev;
        double m_AbsSamplingStep;
        double m_AbsSamplingDens;

        double m_DiceCoefficient[1];
        double m_IntUnionRatio[1];

        double m_Dmax;
        double m_Dmin;

        double m_Downsampling;
        double m_Middle;

        struct look
        {
           float R;
           float G;
           float B;
        };

        look m_Lookuptable[256];
};

extern "C" {
  void QT_prog(void *out, int p);
}

#endif
