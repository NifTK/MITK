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

1. Branch: trac-2711-MITK-README

   This README was added, and which is then updated from many branches.

2. Branch: trac-2717-patched-Geometry2DDataVtkMapper3D

   Provide property to turn off black background in Geometry2DDataVtkMapper3D.

   It was found that an image used as a plane in a 3D window using 
   mitkGeometry2DDataVtkMapper3D.h would be visible in one 3D window, and not
   in another 3D window. This may be related to MITK bug 2134. 

3. Branch: trac-2571-data-manager-dnd-derived-nodes

   Provide ability to drag and drop nodes in DataManager, so that
   the dropped node becomes a child of the drop target.

4. Branch: trac-3528-QmitkDataStorageComboBox-autoselection-fix

   Change node-auto-selection behaviour for QmitkDataStorageComboBox to no
   select anything if the Combobox is editable. It used to select a node
   despite auto-selection being turned off if nothing had been selected yet.
   Now it will preserve the user entered text (if any) and the fact that
   nothing has been selected.

5. Branch: trac-4158-LocalFile-dll-export

   Mark a nested class as dll-export so that we can use it in NifTK.

6. Branch: 4082-cmake-prefix-path

   Passes down the CMAKE_PREFIX_PATH variable to MITK external projects


Tickets that are Outstanding (waiting to be merged) with MITK
=============================================================

The following is a list of differences:

1. Branch bug-16895-trac-2627-block-snc-signals

   Introduces a function to block signals from mitk::SliceNavigationController.

2. Branch 4010-PointListWidget

   PointListModel has appropriate beginResetModel/endResetModel to update lists and correctly supports setting of existing pointSetList.

3. Branch bug-17812-slicedgeometry3d-init

   SliceGeometry3D initialisation fix

4. Branch bug-19266-toolmanager-register-tools

   MITK segmentation tools should not be instantiated automatically by mitk::ToolManager.

5. Branch 4398-basicImageProcessing

   Re-factored the BasicImageProcessing code:
    - separated processing code to a BasicImageProcessor class and moved it into MitkCore
    - re-factored the plugin to use the BasicImageProcessor class
    - fixed templating to allow correct handling of all image types
    - added non-binary thresholding
    - changed downsampling interpolation to linear (used to be nearest neighbour)

6. Branch bug-19289-open-images-from-cl

   Pass down command line arguments to CTK. This is needed so that images can be
   opened from the command line. This fix requires another fix in CTK. See pull
   request here:

   https://github.com/commontk/CTK/pull/603

7. Branch bug-19390-SetDataStorage-arg-check

   This bug caused crash in the thumbnail viewer when an editor has been closed and
   re-opened, e.g. because the project has been closed and new image has  been opened.

