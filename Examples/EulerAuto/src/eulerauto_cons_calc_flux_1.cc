/*  File produced by Kranc */

#define KRANC_C

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cctk.h"
#include "cctk_Arguments.h"
#include "cctk_Parameters.h"
#include "GenericFD.h"
#include "Differencing.h"
#include "cctk_Loop.h"
#include "loopcontrol.h"

/* Define macros used in calculations */
#define INITVALUE (42)
#define INV(x) ((CCTK_REAL)1.0 / (x))
#define SQR(x) ((x) * (x))
#define CUB(x) ((x) * SQR(x))
#define QAD(x) (SQR(SQR(x)))

extern "C" void eulerauto_cons_calc_flux_1_SelectBCs(CCTK_ARGUMENTS)
{
  DECLARE_CCTK_ARGUMENTS;
  DECLARE_CCTK_PARAMETERS;
  
  CCTK_INT ierr CCTK_ATTRIBUTE_UNUSED  = 0;
  ierr = Boundary_SelectGroupForBC(cctkGH, CCTK_ALL_FACES, GenericFD_GetBoundaryWidth(cctkGH), -1 /* no table */, "EulerAuto::Den_flux_group","flat");
  if (ierr < 0)
    CCTK_WARN(1, "Failed to register flat BC for EulerAuto::Den_flux_group.");
  ierr = Boundary_SelectGroupForBC(cctkGH, CCTK_ALL_FACES, GenericFD_GetBoundaryWidth(cctkGH), -1 /* no table */, "EulerAuto::En_flux_group","flat");
  if (ierr < 0)
    CCTK_WARN(1, "Failed to register flat BC for EulerAuto::En_flux_group.");
  ierr = Boundary_SelectGroupForBC(cctkGH, CCTK_ALL_FACES, GenericFD_GetBoundaryWidth(cctkGH), -1 /* no table */, "EulerAuto::S1_flux_group","flat");
  if (ierr < 0)
    CCTK_WARN(1, "Failed to register flat BC for EulerAuto::S1_flux_group.");
  ierr = Boundary_SelectGroupForBC(cctkGH, CCTK_ALL_FACES, GenericFD_GetBoundaryWidth(cctkGH), -1 /* no table */, "EulerAuto::S2_flux_group","flat");
  if (ierr < 0)
    CCTK_WARN(1, "Failed to register flat BC for EulerAuto::S2_flux_group.");
  ierr = Boundary_SelectGroupForBC(cctkGH, CCTK_ALL_FACES, GenericFD_GetBoundaryWidth(cctkGH), -1 /* no table */, "EulerAuto::S3_flux_group","flat");
  if (ierr < 0)
    CCTK_WARN(1, "Failed to register flat BC for EulerAuto::S3_flux_group.");
  return;
}

static void eulerauto_cons_calc_flux_1_Body(cGH const * restrict const cctkGH, int const dir, int const face, CCTK_REAL const normal[3], CCTK_REAL const tangentA[3], CCTK_REAL const tangentB[3], int const imin[3], int const imax[3], int const n_subblock_gfs, CCTK_REAL * restrict const subblock_gfs[])
{
  DECLARE_CCTK_ARGUMENTS;
  DECLARE_CCTK_PARAMETERS;
  
  
  /* Include user-supplied include files */
  
  /* Initialise finite differencing variables */
  ptrdiff_t /*const*/ di CCTK_ATTRIBUTE_UNUSED  = 1;
  ptrdiff_t /*const*/ dj CCTK_ATTRIBUTE_UNUSED  = CCTK_GFINDEX3D(cctkGH,0,1,0) - CCTK_GFINDEX3D(cctkGH,0,0,0);
  ptrdiff_t /*const*/ dk CCTK_ATTRIBUTE_UNUSED  = CCTK_GFINDEX3D(cctkGH,0,0,1) - CCTK_GFINDEX3D(cctkGH,0,0,0);
  ptrdiff_t /*const*/ cdi CCTK_ATTRIBUTE_UNUSED  = sizeof(CCTK_REAL) * di;
  ptrdiff_t /*const*/ cdj CCTK_ATTRIBUTE_UNUSED  = sizeof(CCTK_REAL) * dj;
  ptrdiff_t /*const*/ cdk CCTK_ATTRIBUTE_UNUSED  = sizeof(CCTK_REAL) * dk;
  CCTK_REAL /*const*/ dx CCTK_ATTRIBUTE_UNUSED  = ToReal(CCTK_DELTA_SPACE(0));
  CCTK_REAL /*const*/ dy CCTK_ATTRIBUTE_UNUSED  = ToReal(CCTK_DELTA_SPACE(1));
  CCTK_REAL /*const*/ dz CCTK_ATTRIBUTE_UNUSED  = ToReal(CCTK_DELTA_SPACE(2));
  CCTK_REAL /*const*/ dt CCTK_ATTRIBUTE_UNUSED  = ToReal(CCTK_DELTA_TIME);
  CCTK_REAL /*const*/ t CCTK_ATTRIBUTE_UNUSED  = ToReal(cctk_time);
  CCTK_REAL /*const*/ dxi CCTK_ATTRIBUTE_UNUSED  = INV(dx);
  CCTK_REAL /*const*/ dyi CCTK_ATTRIBUTE_UNUSED  = INV(dy);
  CCTK_REAL /*const*/ dzi CCTK_ATTRIBUTE_UNUSED  = INV(dz);
  CCTK_REAL /*const*/ khalf CCTK_ATTRIBUTE_UNUSED  = 0.5;
  CCTK_REAL /*const*/ kthird CCTK_ATTRIBUTE_UNUSED  = 1/3.0;
  CCTK_REAL /*const*/ ktwothird CCTK_ATTRIBUTE_UNUSED  = 2.0/3.0;
  CCTK_REAL /*const*/ kfourthird CCTK_ATTRIBUTE_UNUSED  = 4.0/3.0;
  CCTK_REAL /*const*/ keightthird CCTK_ATTRIBUTE_UNUSED  = 8.0/3.0;
  CCTK_REAL /*const*/ hdxi CCTK_ATTRIBUTE_UNUSED  = 0.5 * dxi;
  CCTK_REAL /*const*/ hdyi CCTK_ATTRIBUTE_UNUSED  = 0.5 * dyi;
  CCTK_REAL /*const*/ hdzi CCTK_ATTRIBUTE_UNUSED  = 0.5 * dzi;
  
  /* Initialize predefined quantities */
  CCTK_REAL /*const*/ p1o1 CCTK_ATTRIBUTE_UNUSED  = 1.;
  CCTK_REAL /*const*/ p1odx CCTK_ATTRIBUTE_UNUSED  = INV(dx);
  CCTK_REAL /*const*/ p1ody CCTK_ATTRIBUTE_UNUSED  = INV(dy);
  CCTK_REAL /*const*/ p1odz CCTK_ATTRIBUTE_UNUSED  = INV(dz);
  
  /* Assign local copies of arrays functions */
  
  
  
  /* Calculate temporaries and arrays functions */
  
  /* Copy local copies back to grid functions */
  
  /* Loop over the grid points */
  #pragma omp parallel
  CCTK_LOOP3(eulerauto_cons_calc_flux_1,
    i,j,k, imin[0],imin[1],imin[2], imax[0],imax[1],imax[2],
    cctk_ash[0],cctk_ash[1],cctk_ash[2])
  {
    ptrdiff_t /*const*/ index CCTK_ATTRIBUTE_UNUSED  = di*i + dj*j + dk*k;
    
    /* Assign local copies of grid functions */
    
    CCTK_REAL DenLeftL CCTK_ATTRIBUTE_UNUSED = DenLeft[index];
    CCTK_REAL DenRightL CCTK_ATTRIBUTE_UNUSED = DenRight[index];
    CCTK_REAL EnLeftL CCTK_ATTRIBUTE_UNUSED = EnLeft[index];
    CCTK_REAL EnRightL CCTK_ATTRIBUTE_UNUSED = EnRight[index];
    CCTK_REAL pLeftL CCTK_ATTRIBUTE_UNUSED = pLeft[index];
    CCTK_REAL pRightL CCTK_ATTRIBUTE_UNUSED = pRight[index];
    CCTK_REAL rhoLeftL CCTK_ATTRIBUTE_UNUSED = rhoLeft[index];
    CCTK_REAL rhoRightL CCTK_ATTRIBUTE_UNUSED = rhoRight[index];
    CCTK_REAL S1LeftL CCTK_ATTRIBUTE_UNUSED = S1Left[index];
    CCTK_REAL S1RightL CCTK_ATTRIBUTE_UNUSED = S1Right[index];
    CCTK_REAL S2LeftL CCTK_ATTRIBUTE_UNUSED = S2Left[index];
    CCTK_REAL S2RightL CCTK_ATTRIBUTE_UNUSED = S2Right[index];
    CCTK_REAL S3LeftL CCTK_ATTRIBUTE_UNUSED = S3Left[index];
    CCTK_REAL S3RightL CCTK_ATTRIBUTE_UNUSED = S3Right[index];
    CCTK_REAL v1LeftL CCTK_ATTRIBUTE_UNUSED = v1Left[index];
    CCTK_REAL v1RightL CCTK_ATTRIBUTE_UNUSED = v1Right[index];
    CCTK_REAL v2LeftL CCTK_ATTRIBUTE_UNUSED = v2Left[index];
    CCTK_REAL v2RightL CCTK_ATTRIBUTE_UNUSED = v2Right[index];
    CCTK_REAL v3LeftL CCTK_ATTRIBUTE_UNUSED = v3Left[index];
    CCTK_REAL v3RightL CCTK_ATTRIBUTE_UNUSED = v3Right[index];
    
    
    /* Include user supplied include files */
    
    /* Precompute derivatives */
    CCTK_REAL /*const*/ ShiftMinus1DenRight CCTK_ATTRIBUTE_UNUSED  = ShiftMinus1(&DenRight[index]);
    CCTK_REAL /*const*/ ShiftMinus1EnRight CCTK_ATTRIBUTE_UNUSED  = ShiftMinus1(&EnRight[index]);
    CCTK_REAL /*const*/ ShiftMinus1pRight CCTK_ATTRIBUTE_UNUSED  = ShiftMinus1(&pRight[index]);
    CCTK_REAL /*const*/ ShiftMinus1rhoRight CCTK_ATTRIBUTE_UNUSED  = ShiftMinus1(&rhoRight[index]);
    CCTK_REAL /*const*/ ShiftMinus1S1Right CCTK_ATTRIBUTE_UNUSED  = ShiftMinus1(&S1Right[index]);
    CCTK_REAL /*const*/ ShiftMinus1S2Right CCTK_ATTRIBUTE_UNUSED  = ShiftMinus1(&S2Right[index]);
    CCTK_REAL /*const*/ ShiftMinus1S3Right CCTK_ATTRIBUTE_UNUSED  = ShiftMinus1(&S3Right[index]);
    CCTK_REAL /*const*/ ShiftMinus1v1Right CCTK_ATTRIBUTE_UNUSED  = ShiftMinus1(&v1Right[index]);
    CCTK_REAL /*const*/ ShiftMinus1v2Right CCTK_ATTRIBUTE_UNUSED  = ShiftMinus1(&v2Right[index]);
    CCTK_REAL /*const*/ ShiftMinus1v3Right CCTK_ATTRIBUTE_UNUSED  = ShiftMinus1(&v3Right[index]);
    
    /* Calculate temporaries and grid functions */
    CCTK_REAL CCTK_ATTRIBUTE_UNUSED DenFluxLeft = rhoLeftL*v1LeftL;
    
    CCTK_REAL CCTK_ATTRIBUTE_UNUSED DenFluxRight = 
      ShiftMinus1rhoRight*ShiftMinus1v1Right;
    
    CCTK_REAL CCTK_ATTRIBUTE_UNUSED DenFluxL = 0.5*(DenFluxLeft + 
      DenFluxRight + (-1.*DenLeftL + 
      ShiftMinus1DenRight)*ToReal(hlleAlpha));
    
    CCTK_REAL CCTK_ATTRIBUTE_UNUSED S1FluxLeft = pLeftL + 
      rhoLeftL*SQR(v1LeftL);
    
    CCTK_REAL CCTK_ATTRIBUTE_UNUSED S1FluxRight = ShiftMinus1pRight + 
      ShiftMinus1rhoRight*SQR(ShiftMinus1v1Right);
    
    CCTK_REAL CCTK_ATTRIBUTE_UNUSED S1FluxL = 0.5*(S1FluxLeft + 
      S1FluxRight + (-1.*S1LeftL + ShiftMinus1S1Right)*ToReal(hlleAlpha));
    
    CCTK_REAL CCTK_ATTRIBUTE_UNUSED S2FluxLeft = 
      rhoLeftL*v1LeftL*v2LeftL;
    
    CCTK_REAL CCTK_ATTRIBUTE_UNUSED S2FluxRight = 
      ShiftMinus1rhoRight*ShiftMinus1v1Right*ShiftMinus1v2Right;
    
    CCTK_REAL CCTK_ATTRIBUTE_UNUSED S2FluxL = 0.5*(S2FluxLeft + 
      S2FluxRight + (-1.*S2LeftL + ShiftMinus1S2Right)*ToReal(hlleAlpha));
    
    CCTK_REAL CCTK_ATTRIBUTE_UNUSED S3FluxLeft = 
      rhoLeftL*v1LeftL*v3LeftL;
    
    CCTK_REAL CCTK_ATTRIBUTE_UNUSED S3FluxRight = 
      ShiftMinus1rhoRight*ShiftMinus1v1Right*ShiftMinus1v3Right;
    
    CCTK_REAL CCTK_ATTRIBUTE_UNUSED S3FluxL = 0.5*(S3FluxLeft + 
      S3FluxRight + (-1.*S3LeftL + ShiftMinus1S3Right)*ToReal(hlleAlpha));
    
    CCTK_REAL CCTK_ATTRIBUTE_UNUSED EnFluxLeft = (EnLeftL + 
      pLeftL)*v1LeftL;
    
    CCTK_REAL CCTK_ATTRIBUTE_UNUSED EnFluxRight = (ShiftMinus1EnRight + 
      ShiftMinus1pRight)*ShiftMinus1v1Right;
    
    CCTK_REAL CCTK_ATTRIBUTE_UNUSED EnFluxL = 0.5*(EnFluxLeft + 
      EnFluxRight + (-1.*EnLeftL + ShiftMinus1EnRight)*ToReal(hlleAlpha));
    
    /* Copy local copies back to grid functions */
    DenFlux[index] = DenFluxL;
    EnFlux[index] = EnFluxL;
    S1Flux[index] = S1FluxL;
    S2Flux[index] = S2FluxL;
    S3Flux[index] = S3FluxL;
  }
  CCTK_ENDLOOP3(eulerauto_cons_calc_flux_1);
}

extern "C" void eulerauto_cons_calc_flux_1(CCTK_ARGUMENTS)
{
  DECLARE_CCTK_ARGUMENTS;
  DECLARE_CCTK_PARAMETERS;
  
  
  if (verbose > 1)
  {
    CCTK_VInfo(CCTK_THORNSTRING,"Entering eulerauto_cons_calc_flux_1_Body");
  }
  
  if (cctk_iteration % eulerauto_cons_calc_flux_1_calc_every != eulerauto_cons_calc_flux_1_calc_offset)
  {
    return;
  }
  
  const char *const groups[] = {
    "EulerAuto::Den_flux_group",
    "EulerAuto::Den_lr_group",
    "EulerAuto::En_flux_group",
    "EulerAuto::En_lr_group",
    "EulerAuto::p_lr_group",
    "EulerAuto::rho_lr_group",
    "EulerAuto::S1_flux_group",
    "EulerAuto::S1_lr_group",
    "EulerAuto::S2_flux_group",
    "EulerAuto::S2_lr_group",
    "EulerAuto::S3_flux_group",
    "EulerAuto::S3_lr_group",
    "EulerAuto::v1_lr_group",
    "EulerAuto::v2_lr_group",
    "EulerAuto::v3_lr_group"};
  GenericFD_AssertGroupStorage(cctkGH, "eulerauto_cons_calc_flux_1", 15, groups);
  
  GenericFD_EnsureStencilFits(cctkGH, "eulerauto_cons_calc_flux_1", 1, 1, 1);
  
  GenericFD_LoopOverInterior(cctkGH, eulerauto_cons_calc_flux_1_Body);
  
  if (verbose > 1)
  {
    CCTK_VInfo(CCTK_THORNSTRING,"Leaving eulerauto_cons_calc_flux_1_Body");
  }
}
