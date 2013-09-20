/* $Id: mesh_run.cxx,v 1.4 2011/02/01 15:21:46 xushun Exp $ */


/*
 *
 *  Copyright (C) 2001-2004 EPFL (Swiss Federal Institute of Technology,
 *  Lausanne) This program is free software; you can redistribute it
 *  and/or modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2 of
 *  the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 *  USA.
 *
 *  In addition, as a special exception, EPFL gives permission to link
 *  the code of this program with the Qt non-commercial edition library
 *  (or with modified versions of Qt non-commercial edition that use the
 *  same license as Qt non-commercial edition), and distribute linked
 *  combinations including the two.  You must obey the GNU General
 *  Public License in all respects for all of the code used other than
 *  Qt non-commercial edition.  If you modify this file, you may extend
 *  this exception to your version of the file, but you are not
 *  obligated to do so.  If you do not wish to do so, delete this
 *  exception statement from your version.
 *
 *  Authors : Nicolas Aspert, Diego Santa-Cruz and Davy Jacquet
 *
 *  Web site : http://mesh.epfl.ch
 *
 *  Reference :
 *   "MESH : Measuring Errors between Surfaces using the Hausdorff distance"
 *   in Proceedings of IEEE Intl. Conf. on Multimedia and Expo (ICME) 2002, 
 *   vol. I, pp. 705-708, available on http://mesh.epfl.ch
 *
 */



#ifndef MESH_RUN_C
#define MESH_RUN_C

#include <MeshValmet/mesh_run.h>

/* Runs the mesh program, given the parsed arguments in *args. The models and
 * their respective errors are returned in *model1 and *model2. If
 * args->no_gui is zero a QT window is opened to display the visual
 * results. All normal (non error) output is printed through the output buffer
 * out. If not NULL the progress object is used to report the progress. */

void mesh_run(const struct args *args, struct model_error *model1,
              struct model_error *model2, struct outbuf *out,
              struct prog_reporter *progress, struct dist_surf_surf_stats *stats,
              struct dist_surf_surf_stats *stats_rev, double *abs_sampling_step,
              double *abs_sampling_dens)
{
  clock_t start_time;
  double bbox1_diag,bbox2_diag;
  struct model_info *m1info,*m2info;
  int nv_empty,nf_empty;

  memset(model1,0,sizeof(*model1));
  memset(model2,0,sizeof(*model2));

  memset(stats,0,sizeof(*stats));
  memset(stats_rev,0,sizeof(*stats_rev));


  m1info = (struct model_info*) xa_malloc(sizeof(*m1info));
  m2info = (struct model_info*) xa_malloc(sizeof(*m2info));

  outbuf_printf(out,"Reading %s ... ",args -> m1_fname);
  outbuf_flush(out);

  start_time = clock();

  vtkSmartPointer <vtkPolyData> Data1 = vtkSmartPointer <vtkPolyData> ::New();
  Data1 = ConvertFileToData( args -> m1_fname );
  model1->mesh = ConvertDataToModel( Data1 );

  outbuf_printf(out,"Done (%.2f secs)\n", (double)(clock()-start_time)/CLOCKS_PER_SEC);
  outbuf_printf(out,"Reading %s ... ",args -> m2_fname);
  outbuf_flush(out);

  start_time = clock();

  vtkSmartPointer <vtkPolyData> Data2 = vtkSmartPointer <vtkPolyData> ::New();
  Data2 = ConvertFileToData( args -> m2_fname );
  model2->mesh = ConvertDataToModel( Data2 );

  outbuf_printf(out,"Done (%.2f secs)\n", (double)(clock()-start_time)/CLOCKS_PER_SEC);
  outbuf_flush(out);

  start_time = clock();

  bbox1_diag = dist_v( &model1 -> mesh -> bBox[ 0 ] , &model1 -> mesh -> bBox[ 1 ] );
  bbox2_diag = dist_v( &model2 -> mesh -> bBox[ 0 ] , &model2 -> mesh -> bBox[ 1 ] );

  analyze_model( model1->mesh , m1info , 0 , args->verb_analysis , out , "model 1" );
  model1->info = m1info;

  analyze_model(model2->mesh,m2info,1,args->verb_analysis,out,"model 2");
  model2->info = m2info;

  *abs_sampling_step = args->sampling_step*bbox2_diag;
  *abs_sampling_dens = 1/((*abs_sampling_step)*(*abs_sampling_step));

  outbuf_printf(out,"\nSampling_step : %f\n",*abs_sampling_step);
  
  /* Print available model information */
  outbuf_printf(out,"\n\tModel information\n(degenerate faces ignored for manifold/closed info)\n\n");
  outbuf_printf(out,"Number of vertices:\t\t%11d\t%11d\n", model1->mesh->num_vert , model2->mesh->num_vert );
  outbuf_printf(out,"Number of triangles:\t\t%11d\t%11d\n", model1->mesh->num_faces , model2->mesh->num_faces );
  outbuf_printf(out,"Degenerate triangles:\t\t%11d\t%11d\n", m1info->n_degenerate , m2info->n_degenerate );
  outbuf_printf(out,"BoundingBox diagonal:\t\t%11g\t%11g\n", bbox1_diag , bbox2_diag );
  outbuf_printf(out,"Number of disjoint parts:\t%11d\t%11d\n", m1info->n_disjoint_parts , m2info->n_disjoint_parts );
  outbuf_printf(out,"Manifold:\t\t\t%11s\t%11s\n", (m1info->manifold ? "yes" : "no") , (m2info->manifold ? "yes" : "no") );
  outbuf_printf(out,"Originally oriented:\t\t%11s\t%11s\n", (m1info->orig_oriented ? "yes" : "no") , (m2info->orig_oriented ? "yes" : "no") );
  outbuf_printf(out,"Orientable:\t\t\t%11s\t%11s\n", (m1info->orientable ? "yes" : "no") , (m2info->orientable ? "yes" : "no") );
  outbuf_printf(out,"Closed:\t\t\t%11s\t%11s\n" , (m1info->closed ? "yes" : "no") , (m2info->closed ? "yes" : "no") );
  outbuf_flush(out);

  if(args->do_symmetric == 1 || args->do_symmetric == 3)
  {

    outbuf_printf( out , "\n\tCompute the distance from A to B\n" );

    /* Compute the distance from one model to the other */
    dist_surf_surf(model1,model2->mesh,*abs_sampling_dens,args->min_sample_freq,
                   stats,1,(args->quiet ? NULL : progress));

    /* Print results */
    outbuf_printf(out,"Surface area:\t%11g\t%11g\n", stats->m1_area , stats->m2_area );

    outbuf_printf(out,"Min:\t%11g\t%11g\n" , stats->min_dist , fabs(stats->min_dist/bbox2_diag)*100 );
    outbuf_printf(out,"Max:\t%11g\t%11g\n", stats->max_dist , fabs(stats->max_dist/bbox2_diag)*100 );
    outbuf_printf(out,"Mean:\t%11g\t%11g\n" , stats->mean_dist , fabs(stats->mean_dist/bbox2_diag)*100 );
    outbuf_printf(out,"RMS:\t%11g\t%11g\n", stats->rms_dist , fabs(stats->rms_dist/bbox2_diag)*100 );
    outbuf_printf(out,"\n");
    outbuf_flush(out);
  }
  if(args->do_symmetric == 2 || args->do_symmetric == 3) 
  { 
     outbuf_printf( out , "\n\tCompute the distance from B to A\n" );

     /* Invert models and recompute distance */
     dist_surf_surf(model2,model1->mesh,*abs_sampling_dens,args->min_sample_freq,
                   stats_rev,0,(args->quiet ? NULL : progress));

     outbuf_printf(out,"Min:\t%11g\t%11g\n" , stats->min_dist , fabs(stats->min_dist/bbox2_diag)*100 );
     outbuf_printf(out,"Max:\t%11g\t%11g\n", stats->max_dist , fabs(stats->max_dist/bbox2_diag)*100 );
     outbuf_printf(out,"Mean:\t%11g\t%11g\n" , stats->mean_dist , fabs(stats->mean_dist/bbox2_diag)*100 );
     outbuf_printf(out,"RMS:\t%11g\t%11g\n", stats->rms_dist , fabs(stats->rms_dist/bbox2_diag)*100 );
     outbuf_printf(out,"\n");
     outbuf_flush(out);
   }
   if(args->do_symmetric == 3)
   {
     /* Print symmetric distance measures */
     outbuf_printf(out, "\n\tSymmetric distance between model 1 and model 2\n");
     outbuf_printf(out,"Sym_Min:\t%11g\t%11g\n", min(stats->min_dist,stats_rev->min_dist) , min(stats->min_dist,stats_rev->min_dist)/bbox2_diag*100 );
     outbuf_printf(out,"Sym_Max:\t%11g\t%11g\n", max(stats->max_dist,stats_rev->max_dist) , max(stats->max_dist,stats_rev->max_dist)/bbox2_diag*100 );

     //-------------------------------------------------------------------------------------------------------------------------------------------------------------------
     //Here the Sym_Mean and Sym_RMS aren't quite right.
     outbuf_printf(out,"Sym_Mean:\t%11g/%11g\t%11g/%11g\n" , stats->mean_dist,stats_rev->mean_dist , stats->mean_dist/bbox2_diag*100,stats_rev->mean_dist/bbox2_diag*100 );
     outbuf_printf(out,"Sym_RMS:\t%11g/%11g\t%11g/%11g\n" , stats->rms_dist,stats_rev->rms_dist , stats->rms_dist/bbox2_diag*100,stats_rev->rms_dist/bbox2_diag*100 );
     outbuf_printf(out,"\n");
     //-------------------------------------------------------------------------------------------------------------------------------------------------------------------
   }


  outbuf_printf(out,"\tAbsolute\t%%BBox diag\tExpected samples\n\t\tmodel 2\tmodel 1\tmodel 2\n");
                
                
  if (args->do_symmetric == 1) 
  {
    outbuf_printf(out,"Sampling step:   \t%8g\t   %7g     \t   %7d\t%7d\n",
                  *abs_sampling_step,(*abs_sampling_step)/bbox2_diag*100,
                  (int)(stats->m1_area*(*abs_sampling_dens)),0);
    outbuf_printf(out,"\n");
  }
  else if(args->do_symmetric == 2)
  {
    outbuf_printf(out,"Sampling step:   \t%8g\t   %7g     \t   %7d\t%7d\n",
                  *abs_sampling_step,(*abs_sampling_step)/bbox2_diag*100,
                  (int)0,
                  (int)(stats->m2_area*(*abs_sampling_dens)));
    outbuf_printf(out,"\n");
  } 
  else if(args->do_symmetric == 3)
  {
    outbuf_printf(out,"Sampling step:   \t%8g\t   %7g     \t   %7d\t%7d\n",
                  *abs_sampling_step,(*abs_sampling_step)/bbox2_diag*100,
                  (int)(stats->m1_area*(*abs_sampling_dens)),
                  (int)(stats->m2_area*(*abs_sampling_dens)));
    outbuf_printf(out,"\n");
  }
      
  outbuf_printf(out,"        \t    Total\t    Avg. / triangle\t\t"
                  "Tot (%%) area of\n"
                  "        \t         \tmodel 1 \tmodel 2 \t"
                  "sampled triang.\n");
  
  if(args->do_symmetric == 1 || args->do_symmetric == 3)
    outbuf_printf(out,"Samples (1->2):\t%9d\t%7.2g\t%15.2g\t%18.2f\n",
                  stats->m1_samples,
                  ((double)stats->m1_samples)/model1->mesh->num_faces,
                  ((double)stats->m1_samples)/model2->mesh->num_faces,
                  stats->st_m1_area/stats->m1_area*100.0);
  if(args->do_symmetric == 2 || args->do_symmetric == 3)
    outbuf_printf(out,"Samples (2->1):\t%9d\t%7.2g\t%15.2g\t%18.2f\n",
                  stats_rev->m1_samples,
                  ((double)stats_rev->m1_samples)/model1->mesh->num_faces,
                  ((double)stats_rev->m1_samples)/model2->mesh->num_faces,
                  stats_rev->st_m1_area/stats_rev->m1_area*100.0);
  outbuf_printf(out,"\n");
  
  outbuf_printf(out,"                                \t     "
                  "X\t    Y\t   Z\t   Total\n");
  if(args->do_symmetric == 1 || args->do_symmetric == 3)
    outbuf_printf(out,"Partitioning grid size (1 to 2):\t%6d\t%5d\t%4d\t%8d\n",
                  stats->grid_sz.x,stats->grid_sz.y,stats->grid_sz.z,
                  stats->grid_sz.x*stats->grid_sz.y*stats->grid_sz.z);
  if(args->do_symmetric == 2 || args->do_symmetric == 3)
    outbuf_printf(out,"Partitioning grid size (2 to 1):\t%6d\t%5d\t%4d\t%8d\n",
                  stats_rev->grid_sz.x,stats_rev->grid_sz.y,stats_rev->grid_sz.z,
                  stats_rev->grid_sz.x*stats_rev->grid_sz.y*stats_rev->grid_sz.z);
  if(args->do_symmetric == 1 || args->do_symmetric == 3)
    outbuf_printf(out,"\nAvg. number of triangles per non-empty cell (1 to 2):"
                  "\t%.2f\n",stats->n_t_p_nec);
  if(args->do_symmetric == 2 || args->do_symmetric == 3)
    outbuf_printf(out,"Avg. number of triangles per non-empty cell (2 to 1):"
                  "\t%.2f\n",stats_rev->n_t_p_nec);
  if(args->do_symmetric == 1 || args->do_symmetric == 3)
    outbuf_printf(out,
                  "Proportion of non-empty cells (1 to 2):          \t%.2f%%\n",
                  (double)stats->n_ne_cells/(stats->grid_sz.x*stats->grid_sz.y*
                                            stats->grid_sz.z)*100.0);
  if(args->do_symmetric == 2 || args->do_symmetric == 3)
    outbuf_printf(out,
                  "Proportion of non-empty cells (2 to 1):          \t%.2f%%\n",
                  (double)stats_rev->n_ne_cells/
                  (stats_rev->grid_sz.x*stats_rev->grid_sz.y*
                   stats_rev->grid_sz.z)*100.0);
                   
  outbuf_printf(out,"\n");
  outbuf_printf(out,"Analysis and measuring time (secs.):\t%.2f\n",
                (double)(clock()-start_time)/CLOCKS_PER_SEC);
  outbuf_flush(out);

//--------------------------------------------------------------------------------------
  /* Get the per vertex error metric */
  nv_empty = nf_empty = 0; /* keep compiler happy */
  if(args->do_symmetric == 1 || args->do_symmetric == 3)
  {

    calc_vertex_error( model1 , &nv_empty , &nf_empty );

    if (nv_empty>0) {
       outbuf_printf(out,
                      "WARNING: %.2f%% of vertices (%i out of %i) have no error "
                      "samples\n",100.0*nv_empty/model1->mesh->num_vert,
                      nv_empty,model1->mesh->num_vert);
    }
    if (nf_empty>0) {
        outbuf_printf(out,
                      "WARNING: %.2f%% of faces (%i out of %i) have no error "
                      "samples\n",100.0*nf_empty/model1->mesh->num_faces,
                      nf_empty,model1->mesh->num_faces);
    }
  }
  if(args->do_symmetric == 2 || args->do_symmetric == 3)
  {
      outbuf_printf(out,"in mesh_run\n");
    calc_vertex_error(model2,&nv_empty,&nf_empty);
    if (nv_empty>0) {
        outbuf_printf(out,
                      "WARNING: %.2f%% of vertices (%i out of %i) have no error "
                      "samples\n",100.0*nv_empty/model1->mesh->num_vert,
                      nv_empty,model1->mesh->num_vert);
    }
    if (nf_empty>0) {
        outbuf_printf(out,
                      "WARNING: %.2f%% of faces (%i out of %i) have no error "
                      "samples\n",100.0*nf_empty/model1->mesh->num_faces,
                      nf_empty,model1->mesh->num_faces);
    }
  }
  outbuf_flush(out);
}

vtkSmartPointer <vtkPolyData> ConvertFileToData( char* File )
{
    vtkSmartPointer <vtkPolyData> Data = vtkSmartPointer <vtkPolyData> ::New();
    vtkSmartPointer <vtkPolyDataReader> Reader = vtkSmartPointer <vtkPolyDataReader> ::New();

    Reader -> SetFileName( File );
    Reader -> Update();
    Data = Reader -> GetOutput();

    return Data;
}


model* ConvertDataToModel( vtkSmartPointer <vtkPolyData> Data )
{
    model *Model = new model;

    vtkSmartPointer <vtkPoints> Points = vtkSmartPointer <vtkPoints> ::New();
    vtkSmartPointer <vtkCellArray> Polys = vtkSmartPointer <vtkCellArray> ::New();
    /*vtkSmartPointer <vtkPolyDataNormals> NormalFilter = vtkSmartPointer <vtkPolyDataNormals> ::New();
    vtkSmartPointer <vtkPolyData> OutputNormalFilter = vtkSmartPointer <vtkPolyData> ::New();*/

    Points = Data -> GetPoints();
    Polys = Data -> GetPolys();

    vtkIdType NumberOfPoints = Points -> GetNumberOfPoints();
    vtkIdType NumberOfCells = Polys -> GetNumberOfCells();

    /*vertex_t *VNormals = new vertex_t[ NumberOfPoints ];
    vertex_t *FNormals = new vertex_t[ NumberOfCells ];*/

    if( NumberOfCells == 0 || NumberOfPoints == 0 )
    {
        std::cout << " error " << std::endl;
        Model = 0 ;
        return Model;
    }

    vertex_t *Vertices = new vertex_t[ NumberOfPoints ];
    face_t *Faces = new face_t[ NumberOfCells ];

    vertex_t Bmin;
    vertex_t Bmax;

    vtkIdType Id = 0;
    vtkIdType Row = 0;
    vtkIdType *f;

    double v[3];
    int i = 0;

    for( Id = 0 ; Id < NumberOfPoints ; Id++ )
    {

        Points-> GetPoint( Id , v );
        Vertices[ Id ].x = v[ 0 ];
        Vertices[ Id ].y = v[ 1 ];
        Vertices[ Id ].z = v[ 2 ];

        if( Id == 0)
        {
            Bmin.x = Bmax.x = Vertices[ Id ].x;
            Bmin.y = Bmax.y = Vertices[ Id ].y;
            Bmin.z = Bmax.z = Vertices[ Id ].z;
        }

        // fiding Bmin
        if( Vertices[ Id ].x < Bmin.x )
        {
            Bmin.x = Vertices[ Id ].x;
        }
        if( Vertices[ Id ].y < Bmin.y )
        {
            Bmin.y = Vertices[ Id ].y;
        }
        if( Vertices[ Id ].z < Bmin.z )
        {
            Bmin.z = Vertices[ Id ].z;
        }

        // fiding Bmax
        if( Vertices[ Id ].x > Bmax.x )
        {
            Bmax.x = Vertices[ Id ].x;
        }
        if( Vertices[ Id ].y > Bmax.y )
        {
            Bmax.y = Vertices[ Id ].y;
        }
        if( Vertices[ Id ].z > Bmax.z )
        {
            Bmax.z = Vertices[ Id ].z;
        }
     }

    while( Polys -> GetNextCell( Row , f ) )
    {
        Faces[ i ].f0 = f[ 0 ];
        Faces[ i ].f1 = f[ 1 ];
        Faces[ i ].f2 = f[ 2 ];
        i++;
    }



    /*NormalFilter -> SetInputData( Data );

    NormalFilter -> ComputeCellNormalsOff();
    NormalFilter -> ComputePointNormalsOn();

    NormalFilter -> Update();
    OutputNormalFilter = NormalFilter -> GetOutput();

    for( Id = 0 ; Id < NumberOfPoints ; Id++ )
    {
        OutputNormalFilter -> GetPoint( Id , v );
        VNormals[ Id ].x = v[ 0 ];
        VNormals[ Id ].y = v[ 1 ];
        VNormals[ Id ].z = v[ 2 ];
    }

    NormalFilter -> ComputePointNormalsOff();
    NormalFilter -> ComputeCellNormalsOn();

    NormalFilter -> Update();
    OutputNormalFilter = NormalFilter -> GetOutput();

    for( Id = 0 ; Id < NumberOfCells ; Id ++ )
    {
        OutputNormalFilter -> GetPoint( Id , v );
        FNormals[ Id ].x = v[ 0 ];
        FNormals[ Id ].y = v[ 1 ];
        FNormals[ Id ].z = v[ 2 ];
    }*/

    Model->num_faces = NumberOfCells;
    Model->num_vert = NumberOfPoints;
    Model->vertices = Vertices;
    Model->faces = Faces;
    Model->bBox[ 0 ] = Bmin;
    Model->bBox[ 1 ] = Bmax;
    Model->normals = NULL;
    Model->face_normals = NULL;
    Model->builtin_normals = 1;

    return Model;
}

#endif
