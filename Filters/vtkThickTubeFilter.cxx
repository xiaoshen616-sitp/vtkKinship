// Copyright (c) 2010, Jérôme Velut
// All rights reserved.
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
// * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT OWNER ``AS IS'' AND ANY EXPRESS 
// OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN 
// NO EVENT SHALL THE COPYRIGHT OWNER BE LIABLE FOR ANY DIRECT, INDIRECT, 
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
// OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


#include "vtkThickTubeFilter.h"

#include "vtkTubeFilter.h"
#include "vtkCellArray.h"
#include "vtkAppendPolyData.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkSmartPointer.h"

vtkCxxRevisionMacro(vtkThickTubeFilter, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkThickTubeFilter);

vtkThickTubeFilter::vtkThickTubeFilter()
{
   this->NumberOfSides = 6;
   this->InnerRadius = 1;
   this->OuterRadius = 2;
}


void vtkThickTubeFilter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

int vtkThickTubeFilter::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
   // get the info objects
   vtkInformation *inMeshInfo = inputVector[0]->GetInformationObject(0);
   vtkInformation *outMeshInfo = outputVector->GetInformationObject(0);

   vtkPolyData* inputMesh = vtkPolyData::SafeDownCast(
    inMeshInfo->Get(vtkDataObject::DATA_OBJECT()));
   vtkPolyData* outputMesh = vtkPolyData::SafeDownCast(
    outMeshInfo->Get(vtkDataObject::DATA_OBJECT()));

   vtkSmartPointer<vtkPolyData> shallowInput;
   shallowInput = vtkSmartPointer<vtkPolyData>::New( );
   shallowInput->ShallowCopy( inputMesh );

   // Inner tube setup
   vtkSmartPointer<vtkTubeFilter> innerTubeFilter;
   innerTubeFilter = vtkSmartPointer<vtkTubeFilter>::New( );
   innerTubeFilter->SetInputData( shallowInput );
   innerTubeFilter->SetRadius( this->InnerRadius );
   innerTubeFilter->SetNumberOfSides( this->NumberOfSides );
   innerTubeFilter->CappingOff( );

   // Outer tube setup
   vtkSmartPointer<vtkTubeFilter> outerTubeFilter;
   outerTubeFilter = vtkSmartPointer<vtkTubeFilter>::New( );
   outerTubeFilter->SetInputData( shallowInput );
   outerTubeFilter->SetRadius( this->OuterRadius );
   outerTubeFilter->SetNumberOfSides( this->NumberOfSides );
   outerTubeFilter->CappingOff( );

   // Join both onner and outer tube
   vtkSmartPointer<vtkAppendPolyData> append;
   append = vtkSmartPointer<vtkAppendPolyData>::New( );
   append->AddInputConnection( innerTubeFilter->GetOutputPort( ));   
   append->AddInputConnection( outerTubeFilter->GetOutputPort( ));   
   append->Update( );

   // At this point, outputMesh contains two non-connected tubes
   outputMesh->DeepCopy( append->GetOutput( ) );

   // Get the number of point of a cylinder ('spine' length x num. sides)
   //! \note this implies that only one polyline is taken into account
   // Better way would be to check each input polyline cell.
   vtkPolyData* innerTube = static_cast<vtkPolyData*>(innerTubeFilter->GetOutput( ));
   int length = innerTube->GetNumberOfPoints( );

   vtkCellArray* outputTriangles = outputMesh->GetPolys( );
   for( int ptId = 0; ptId < this->NumberOfSides; ptId ++)
   {
      // Triangle one extremity
      vtkIdType triangle[3];
      triangle[0] = ptId;
      triangle[1] = ptId + length;
      triangle[2] = (ptId + 1)%this->NumberOfSides + length;
      outputTriangles->InsertNextCell( 3, triangle );

      triangle[1] = (ptId+1)%this->NumberOfSides;
      triangle[2] = ptId;
      triangle[0] = (ptId + 1)%this->NumberOfSides + length;
      outputTriangles->InsertNextCell( 3, triangle );
 
      // Triangle the other extremity
      int offset = length - this->NumberOfSides;
      triangle[0] = ptId + offset;
      triangle[1] = ptId + +offset + length;
      triangle[2] = (ptId + 1)%this->NumberOfSides + offset + length;
      outputTriangles->InsertNextCell( 3, triangle );

      triangle[1] = (ptId+1)%this->NumberOfSides + offset;
      triangle[2] = ptId + offset;
      triangle[0] = (ptId + 1)%this->NumberOfSides + length + offset;
      outputTriangles->InsertNextCell( 3, triangle );
   }

   return( 1 );
}
