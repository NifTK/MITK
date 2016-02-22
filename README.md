Introduction
============

This repository is a version of MITK, as used by the NifTK project.

The original MITK is here:
http://git.mitk.org/MITK.git

The original MITK is mirrored to GitHub here:
https://github.com/MITK/MITK

This repository is a fork of https://github.com/MITK/MITK.


Updating the master branch
==========================

The master branch should exactly match the MITK master. We update
a local branch here, just so we have a reference of which commit
we think we have merged up to.

So:

git remote add upstream https://github.com/MITK/MITK
git checkout master
git pull upstream master
git push origin master


Updating the niftk branch
=========================

git checkout niftk
git merge --no-ff master

(and try to put something useful in the commit message)


Differences between niftk branch and master branch
==================================================

The NifTK project aims to have as few differences as possible between
this version of MITK and the original MITK. All bugfixes should be 
raised in the MITK bugzilla http://bugs.mitk.org/ and then a branch
created here, forking from master and using the branch naming convention

bug-<MITK bugzilla number>-trac-<trac ticket number>-description

for example

bug-16074-trac-2742-run-app-from-current-dir

MITK can then merge the bugfix into their code, and we pick up the fix
when we update our master branch, and consequently merge into the
niftk branch.

There are however cases where the NifTK project requires functional differences.
Again, these should be as few as possible.  In this case there may or may not
be a bugzilla bug report raised with MITK, so the branch naming convention should
either be the same as the above, or just

trac-<trac ticket number>-description 

These functional differences should still be forked from master so that if MITK 
want to merge it back to their code-base, they can. We would however immediately
merge it into the niftk branch once testing was complete.


Known Differences between niftk branch and master branch
========================================================

The following is a list of differences as of 2013-10-06:

 * Branch: trac-2711-MITK-README

   This README was added, and which is then updated from many branches.

 * Branch: trac-2717-patched-Geometry2DDataVtkMapper3D

   Provide property to turn off black background in Geometry2DDataVtkMapper3D.

   It was found that an image used as a plane in a 3D window using 
   mitkGeometry2DDataVtkMapper3D.h would be visible in one 3D window, and not
   in another 3D window. This may be related to MITK bug 2134. 

 * Branch: trac-2571-data-manager-dnd-derived-nodes

   Provide ability to drag and drop nodes in DataManager, so that
   the dropped node becomes a child of the drop target.

   Branch: 2571-data-manager-dnd-crash

   Fix for crash when dragging node to the bottom.

 * Branch: trac-3528-QmitkDataStorageComboBox-autoselection-fix

   Change node-auto-selection behaviour for QmitkDataStorageComboBox to no
   select anything if the Combobox is editable. It used to select a node
   despite auto-selection being turned off if nothing had been selected yet.
   Now it will preserve the user entered text (if any) and the fact that
   nothing has been selected.

 * Branch: trac-4158-LocalFile-dll-export

   Mark a nested class as dll-export so that we can use it in NifTK.

 * Branch: 4082-cmake-prefix-path

   Passes down the CMAKE_PREFIX_PATH variable to MITK external projects

 * Branch: trac-4463-DisableMITKColormap

   Removes the mitkColormap dropdown menu in the datamanager.
   
 * Branch 4501-MITK-Labeled-Map-Rescaling-Fix
 
   vtkMitkLevelWindowFilter does not rescale vtkLookupTable if IndexedLookup is set to true.

 * Branch 4478-display-position-no-point-picking

   mitk::DisplayPositionEvent should use the display geometry functions to convert
   between display and world coordinates, not the VTK point picker. The point picker messes
   up the z coordinate.

 * Branch 4524-disable-nifti-io

   Disable nifti IO of MITK as NifTK has its own one. Having two is confusing because
   the user is shown a dialog to choose at File / Open, but they are not supposed to
   know the difference. Moreover, our reader is 'superior', as it has not just the fix
   of the MITK one but also more.

Tickets that are Outstanding (waiting to be merged) with MITK
=============================================================

The following is a list of differences:

 * Branch bug-16895-trac-2627-block-snc-signals

   Introduces a function to block signals from mitk::SliceNavigationController.

 * Branch bug-17812-slicedgeometry3d-init

   SliceGeometry3D initialisation fix

 * Branch bug-19266-toolmanager-register-tools

   MITK segmentation tools should not be instantiated automatically by mitk::ToolManager.

 * Branch 4398-basicImageProcessing
   
   Re-factored the BasicImageProcessing code:
    - separated processing code to a BasicImageProcessor class and moved it into MitkCore
    - re-factored the plugin to use the BasicImageProcessor class
    - fixed templating to allow correct handling of all image types
    - added non-binary thresholding
    - changed downsampling interpolation to linear (used to be nearest neighbour)

   Branch 4448-imageproc-casting-fix

   Cast binary threshold output image to unsigned char image.

   MITK bug: 19407

 * Branch bug-19289-open-images-from-cl

   Pass down command line arguments to CTK. This is needed so that images can be
   opened from the command line. This fix requires another fix in CTK. See pull
   request here:

   https://github.com/commontk/CTK/pull/603

 * Branch bug-19255-mitkLookupTablePropertySerializer_Separate_Header_File

   Commit on niftk branch: 4cf84412f60fd0161fab103ddc20057c97b25e7f
 
   mitkLookupTableProperty.cpp is split into a header and source file to enable inheritance.

 * Branch bug-19390-SetDataStorage-arg-check

   This bug caused crash in the thumbnail viewer when an editor has been closed and
   re-opened, e.g. because the project has been closed and new image has  been opened.

 * Branch 4449-clippingplane-revamp

   Improvements to make the clipping plane plugin work for surfaces and images, too,
   not only segmentations.

   MITK bug: 19411

 * Branch 19339-merge-fix-external-python-projs 

   Fixes to get the Python console work.

 * Branch bug-19431-storescp-path

   Fix for the DICOM plugin CMake file to find the storescp command if DCMTK
   is provided in its install directory.

 * Branch 4491-python-console-crash

   Cherry-picked changes from MITK to fix crash with Python console at the
   first run. See MITK bug 19066.

 * Branch bug-19289-ctk-hash-update

   Cherry-picked commits from MITK to update CTK hash, to get fix for passing
   down command line arguments to CTK.

 * Branch 4490-dcmtk-dir

   Fix for putting correct DCMTK path to MITKConfig.cmake

   MITK bug: 19442

 * bug-19467-tool-activation-interactor-config

   Save/restore display interactor configuration at tool activation/deactivation
   from mitk::Tool functions rather then mitk::ToolManager.

   MITK bug: 19467

 * Branch trac-4495-merge-silence-persistence-service

   Suppress output from MITK persistence service.

 * Crash when rendering crosshair

   The crash happens after the application starts, when the first render window is created.
   It happens when the MITK display is enabled.

   http://bugs.mitk.org/show_bug.cgi?id=19247

   Fixed in upstream. Cherry-picked commits (in reverse order):

   * 30b5cb1 COMP: gcc prior to 4.8.0 does not support emplace
   * 857c5a9 COMP: Doing as clang suggests
   * a372b87 COMP: rewrite code to work with MSVS 2012
   * 25cda08 render entire crosshair for gap size 0
   * 9576a79 Added Rostislavs comments
   * bee64ec Make sure helper class is only available locally
   * 1fc0594 fix linux compile issue
   * 83e3b3c More renaming
   * 011264d some renaming
   * 5823ab1 Allow 2D plane geometry mapper to render without reference geometry. Use plane geometry itself to determine bounds if reference geometry is not available. Test for actual in
   * 26a2f62 line representing the plane is computed using the PlaneGeometry bounds corrected intersection detection
   * a0c33a7 Own implementation of interval arithmetic needed.
   * f372c4c Plane geometry data mapper crash and correctness fix
