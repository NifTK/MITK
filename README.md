Introduction
============

This repository is a variant of MITK, as used by the NifTK project.

The original MITK is here:
http://git.mitk.org/MITK.git

The original MITK is mirrored to GitHub here:
https://github.com/MITK/MITK

This repository is a fork of https://github.com/MITK/MITK.


Updating NifTK/MITK
=========================

NifTK/MITK is based on the latest official MITK release (upstream).

The default branch in the repository is 'niftk'. The repository does
not have a 'master' branch. Upgrading to the next upstream release
follows like this:

```
git remote add mitk git@github.com:MITK/MITK
git fetch mitk
git checkout niftk
git pull
... revert local changes that have been fixed in MITK ...
git merge mitk/v2016.03.0
```

To avoid potential conflicts at merge, it is recommended to revert the
changes in our fork that have been fixed in the upstream since the previous
release. The list of commits that have to be reverted can be found in
later sections of this document.


Differences from the upstream
==================================================

The NifTK project aims to have as few differences as possible between
this fork and the original MITK. All bugfixes should be raised in the
MITK bugzilla http://bugs.mitk.org/ and then a branch created here,
forking from mitk/master and using the branch naming convention

  bug-<MITK bugzilla number>-description

For example:

```
git fetch mitk
git checkout -b bug-16074-run-app-from-current-dir mitk/master
```

The commits on the branch must be signed off by including a line like this
in the commit message:

```
Signed-off-by: John Doe <j.doe@ucl.ac.uk>
```

For more details, see:

  - http://mitk.org/wiki/How_to_contribute

```
git push origin bug-16074-run-app-from-current-dir
```

When the branch is pushed, a pull request (PR) has to be created and the link
to the PR be pasted to the bugzilla ticket. MITK can then merge the bugfix into
their code, and we pick up the fix when we update our fork next time.

To apply the changes to this fork, we need to cherry-pick the changes on the PR
branch upon the niftk branch:

```
git checkout niftk
git checkout -b run-app-from-current-dir
git cherry-pick ^bug-16074-run-app-from-current-dir..bug-16074-run-app-from-current-dir
```

The list of commits have to be recorded in this file. See next sections. When this
is done, the branch can be merged back to the niftk branch and can be deleted.

```
git log --oneline niftk..HEAD
vim README.md
... add short description of the issue, MITK bug number and list of commits ...
git add README.md
git commit -m "Readme file updated"
git checkout niftk
git merge --no-ff run-app-from-current-dir
```

The `run-app-from-current-dir` branch (example) can be deleted, it does not need to
be pushed to github. The `bug-16074-run-app-from-current-dir` branch can as well be
deleted from the origin after it has been merged or rejected by MITK.

There are, however, cases where the NifTK project requires functional differences
that are not going to be part of the official MITK. Again, these should be as few
as possible.  In this case there may not be a bugzilla bug report raised with MITK,
so the branch naming convention should be just

  <CMICLab issue number>-description 

These feature branches should be forked from the niftk branch merged back into it
once testing was complete and the list of changes have been recorded in this file.
See next sections. Once the branch is merged, it can be deleted.


Known Differences to the upstream
========================================================

The following is a list of differences that are not going to be integrated to the
upstream. Note that in the past the branch name was recorded here, but the entries
should have a short description, the CMICLab issue number and a list of commits
(`git log --oneline`) instead. Merged branches can be removed.

 * Branch: trac-2711-MITK-README

   This README was added, and which is then updated from many branches.

   CMICLab issue: 2711

   - 76d163f Merge branch 'trac-2711-MITK-audit-response-to-Miklos' into niftk
   - 17a49c1 Updated README following Miklos's suggestions
   - 1047bca Merge branch 'trac-2711-MITK-README' into niftk
   - 3dde151 Provide MITK instructions in README file
   - 74dd388 Added README on niftk branch
   - b3344f6 Merge branch 'trac-2711-MITK-audit' into niftk

 * Provide property to turn off black background in Geometry2DDataVtkMapper3D.

   It was found that an image used as a plane in a 3D window using 
   mitkGeometry2DDataVtkMapper3D.h would be visible in one 3D window, and not
   in another 3D window. This may be related to MITK bug 2134.

   CMICLab issue: 2717

   - 452e3e2 Provide visibility property for background actor

 * Branch: trac-2571-data-manager-dnd-derived-nodes

   Provide ability to drag and drop nodes in DataManager, so that
   the dropped node becomes a child of the drop target.

   CMICLab issue: 2571

   - f86e71e Merge branch 'b2571-data-manager-dnd-derived-nodes' into niftk
   - c0855b8 Support for moving nodes to another parent in the data manager
   - a57061e Merge branch 'bug-15488-data-manager-wrong-drop-position' into niftk
   - cfeed4e Fix wrong insertion point with DnD in the data manager

   Branch: 2571-data-manager-dnd-crash

   Fix for crash when dragging node to the bottom.

   - dbcd1a5 Merge branch '2571-data-manager-dnd-crash' into niftk
   - 72fa572 Fix for crash when dragging nodes to the bottom in the data manager

   CMICLab issue: 2571

 * Branch: trac-3528-QmitkDataStorageComboBox-autoselection-fix

   Change node-auto-selection behaviour for QmitkDataStorageComboBox to no
   select anything if the Combobox is editable. It used to select a node
   despite auto-selection being turned off if nothing had been selected yet.
   Now it will preserve the user entered text (if any) and the fact that
   nothing has been selected.

   CMICLab issue: 3528

   - 1f355f0 Merge branch 'trac-3528-QmitkDataStorageComboBox-autoselection-fix' into niftk
   - 39fd6b3 Dont auto-select a different node if current selection is deleted

 * Mark a nested class as dll-export so that we can use it in NifTK.

   CMICLab issue: 4158

   - f1ff7fb Merge branch 'trac-4158-LocalFile-dll-export' into niftk
   - 89ce233 Bug #4158: mark nested LocalFile class for dll export too, so that we can use it from niftk

 * Pass down the CMAKE_PREFIX_PATH variable to MITK external projects

   CMICLab issue: 4082

   - b7fe729 Merge branch '4082-cmake-prefix-path' into niftk
   - 6e26ccb CMake prefix path passed down to the MITK subproject

 * Remove the mitkColormap dropdown menu in the datamanager.

   CMICLab issue: 4463

   - 440efa6 Merge pull request #2 from NifTK/trac-4463-DisableMITKColormap
   - ffca398 Bug #4463: Adding to readme.md change list.
   - 3d48c26 Bug #4463: removing all references to color maps in the datamanager view.
   
 * vtkMitkLevelWindowFilter does not rescale vtkLookupTable if IndexedLookup is set to true.

   CMICLab issue: 4501

   - 1862fe1 Rescaling according to maximum possible value, not maxIndex value.
   - e88c723 Merge branch 'trac-4501-MITK-Labeled-Map-Rescaling-Fix' into niftk
   - 73d9614 Bug #4501: vtkMitkLevelWindowFilter fix to not rescale the lookuptable  when indexedlookup is set to true.
   - 34ddba7 Separating mitkLookupTablePropertySerliazer into header and source

 * Disable nifti IO of MITK as NifTK has its own one. Having two is confusing because
   the user is shown a dialog to choose at File / Open, but they are not supposed to
   know the difference. Moreover, our reader is 'superior', as it has not just the fix
   of the MITK one but also more.

   CMICLab issue: 4524

   - 0b4aff3 Nifti IO is disabled


Changes that are outstanding (waiting to be merged) with MITK
=============================================================

The following is a list of differences that are expected to be integrated to the
upstream. Note that in the past the branch name was recorded here, but the entries
should have a short description, the MITK bug number and a list of commits
(`git log --oneline`) instead. Merged branches can be removed.

 * Introduce a function to block signals from mitk::SliceNavigationController.

   - 0ec694f Function for blocking signals from slice navigation controller

 * Re-factored the BasicImageProcessing code:
    - separated processing code to a BasicImageProcessor class and moved it into MitkCore
    - re-factored the plugin to use the BasicImageProcessor class
    - fixed templating to allow correct handling of all image types
    - added non-binary thresholding
    - changed downsampling interpolation to linear (used to be nearest neighbour)

   CMICLab issue: 4398

   - d791383 Fix warnings treated as errors in BasicImageProcessor class
   - f23673b Bug #4398: Removed traces of ISelectionListener
   - 17f81f6 Bug #4398: Updated the export symbol
   - f83b2d6 Bug #4398: Updated the plugin to use the BasicImageProcessor from Core module
   - 75f0151 Bug #4398: Added the BasicImageProcessing class

 * Cast binary threshold output image to unsigned char image.

   CMICLab issue: 4448

   MITK bug: 19407

   - c5be978 Fix compiler warning with gcc in basic image processor code
   - 8951dfa Bug 4448: Now force-casting the output of binary threshold to unsigned char pixel type

 * Pass down command line arguments to CTK. This is needed so that images can be
   opened from the command line. This fix requires another fix in CTK. See pull
   request here:

   https://github.com/commontk/CTK/pull/603

   MITK bug: 19289

   - cfc4cf2 Merge branch 'bug-19289-open-images-from-cl' into niftk
   - 7f7ffb9 Pass down command line arguments to CTK

 * Branch bug-19255-mitkLookupTablePropertySerializer_Separate_Header_File

   Commit on niftk branch: 4cf84412f60fd0161fab103ddc20057c97b25e7f
  
   mitkLookupTableProperty.cpp is split into a header and source file to enable inheritance.

   MITK bug: 19255

 * Improvements to make the clipping plane plugin work for surfaces and images, too,
   not only segmentations.

   - 6fffad5 Merge remote-tracking branch 'origin/4449-clippingplane-revamp' into niftk
   - f3a6322 Bug #4449: Not clipping surfaces with deformed planes.
   - 815a2b4 Bug #4449: Copy the level-window of the reference to the clipped image
   - a3b9a13 Bug #4449: Tidy up
   - 85e09b5 Bug #4449: Changed which side of the plane is clipped and made sure that volume is actually clipped (i.e. set to zero and not identical to input)
   - cde4bd7 Bug #4449: Changed how the clipping planes are re-centred, changed UI interaction logic and the clipped node naming.
   - 1264536 Bug #4449: Modified the deformableClippingPlane plugin to clip surfaces as well as images.

   MITK bug: 19411

 * Branch 19339-merge-fix-external-python-projs 

   Fixes to get the Python console work.

   MITK bug: 19411

 * Branch bug-19431-storescp-path

   Fix for the DICOM plugin CMake file to find the storescp command if DCMTK
   is provided in its install directory.

   MITK bug: 19411

 * Branch 4491-python-console-crash

   Cherry-picked changes from MITK to fix crash with Python console at the
   first run.

   MITK bug: 19066.

 * Cherry-picked commits from MITK to update CTK hash, to get fix for passing
   down command line arguments to CTK.

   MITK bug: 19289

   - c2ef88a Merge branch 'bug-19289-ctk-hash-update' into niftk
   - 0d7ee6c Updated CTK to a version that is Qt 5.5 compatible
   - bd2c11f Updated CTK

 * Branch 4490-dcmtk-dir

   MITK bug: 19442

   Fix for putting correct DCMTK path to MITKConfig.cmake

 * Branch trac-4495-merge-silence-persistence-service

   Suppress output from MITK persistence service.

 * DCMTK not found because of directory path case mismatch

   MITK bug: 19588

   * 9afa98f Get canonical directory name before comparing with STREQUAL
