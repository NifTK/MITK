/*===================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center,
Division of Medical and Biological Informatics.
All rights reserved.

This software is distributed WITHOUT ANY WARRANTY; without
even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.

See LICENSE.txt or http://www.mitk.org for details.

===================================================================*/

// change number
#ifndef VTKMAPPER_H_HEADER_INCLUDED_C1C5453B
#define VTKMAPPER_H_HEADER_INCLUDED_C1C5453B

#include <MitkExports.h>
#include "mitkMapper.h"
#include "mitkBaseRenderer.h"
#include "mitkDataNode.h"
#include "mitkVtkPropRenderer.h"

#include <vtkProp3D.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkLinearTransform.h>
#include <vtkMapper.h>
#include <vtkPropAssembly.h>
#include <vtkPolyDataMapper.h>
#include <vtkProp3DCollection.h>

class vtkProp;
class vtkProp3D;
class vtkActor;

namespace mitk {

/** \brief Base class of all Vtk Mappers in order to display primitives
* by exploiting Vtk functionality.
*
* Rendering of opaque, translucent or volumetric geometry and overlays
* is done in consecutive render passes.
*
* \ingroup Mapper
*/
class MITK_CORE_EXPORT VtkMapper : public Mapper
{
  public:
    mitkClassMacro(VtkMapper,Mapper);

    virtual vtkProp* GetVtkProp(mitk::BaseRenderer* renderer) = 0;

    /** \brief Re-issues all drawing commands required to describe
    * the entire scene each time a new frame is required,
    * regardless of actual changes.
    */
    static void SetVtkMapperImmediateModeRendering(vtkMapper *mapper);

     /**
     * \brief Returns whether this is an vtk-based mapper
     * \deprecatedSince{2013_03} All mappers of superclass VTKMapper are vtk based, use a dynamic_cast instead
     */
    DEPRECATED( virtual bool IsVtkBased() const );


    /** \brief Determines which geometry should be rendered
    * (opaque, translucent, volumetric, overlay)
    * and calls the appropriate function.
    *
    * Called by mitk::VtkPropRenderer::Render
    */
    void MitkRender(mitk::BaseRenderer* renderer, mitk::VtkPropRenderer::RenderType type);

    /** \brief Checks visibility and renders the overlay */
    virtual void MitkRenderOverlay(BaseRenderer* renderer);

    /** \brief Checks visibility and renders untransparent geometry */
    virtual void MitkRenderOpaqueGeometry(BaseRenderer* renderer);

    /** \brief Checks visiblity and renders transparent geometry */
    virtual void MitkRenderTranslucentGeometry(BaseRenderer* renderer);

    /** \brief Checks visibility and renders volumes */
    virtual void MitkRenderVolumetricGeometry(BaseRenderer* renderer);

    /** \brief Returns true if this mapper owns the specified vtkProp for
    * the given BaseRenderer.
    *
    * Note: returns false by default; should be implemented for VTK-based
    * Mapper subclasses. */
    virtual bool HasVtkProp( const vtkProp *prop, BaseRenderer *renderer );

    /** \brief Set the vtkTransform of the m_Prop3D for
    * the current time step of \a renderer
    *
    * Called by mitk::VtkPropRenderer::Update before rendering
    */
    virtual void UpdateVtkTransform(mitk::BaseRenderer *renderer);

    /**
    * \brief Apply color and opacity properties read from the PropertyList
    * \deprecatedSince{2013_03} Use ApplyColorAndOpacityProperties(mitk::BaseRenderer* renderer, vtkActor * actor) instead
    */
    DEPRECATED(inline virtual void ApplyProperties(vtkActor* actor, mitk::BaseRenderer* renderer))
    {
      ApplyColorAndOpacityProperties(renderer, actor);
    }

    /**
    * \brief Apply color and opacity properties read from the PropertyList.
    * Called by mapper subclasses.
    */
    virtual void ApplyColorAndOpacityProperties(mitk::BaseRenderer* renderer, vtkActor * actor);

    /**
    * \brief  Release vtk-based graphics resources that are being consumed by this mapper.
    *
    * The parameter window could be used to determine which graphic
    * resources to release. Must be overwritten in individual subclasses
    * if vtkProps are used.
    *
    * \deprecatedSince{2013_12} Use ReleaseGraphicsResources(mitk::BaseRenderer* renderer) instead
    */
    DEPRECATED(virtual void ReleaseGraphicsResources(vtkWindow* /*renWin*/))
    {
    };

    /**
    * \brief  Release vtk-based graphics resources that are being consumed by this mapper.
    *
    * Method called by mitk::VtkPropRenderer. The parameter renderer could be used to
    * determine which graphic resources to release.  The local storage is accessible
    * by the parameter renderer. Should be overwritten in subclasses.
    */
    virtual void ReleaseGraphicsResources(mitk::BaseRenderer* /*renderer*/)
    {
    };

  protected:

    /** constructor */
    VtkMapper();

    /** virtual destructor in order to derive from this class */
    virtual ~VtkMapper();

  private:

    /** copy constructor */
    VtkMapper( const VtkMapper &);

    /** assignment operator */
    VtkMapper & operator=(const VtkMapper &);

};
} // namespace mitk
#endif /* VTKMAPPER_H_HEADER_INCLUDED_C1C5453B */
