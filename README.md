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
git log --one-line niftk..HEAD
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
(`git log --one-line`) instead. Merged branches can be removed.

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

 * Branch 4524-disable-nifti-io

   Disable nifti IO of MITK as NifTK has its own one. Having two is confusing because
   the user is shown a dialog to choose at File / Open, but they are not supposed to
   know the difference. Moreover, our reader is 'superior', as it has not just the fix
   of the MITK one but also more.


Changes that are outstanding (waiting to be merged) with MITK
=============================================================

The following is a list of differences that are expected to be integrated to the
upstream. Note that in the past the branch name was recorded here, but the entries
should have a short description, the MITK bug number and a list of commits
(`git log --one-line`) instead. Merged branches can be removed.

 * Branch bug-16895-trac-2627-block-snc-signals

   Introduces a function to block signals from mitk::SliceNavigationController.

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

 * Branch trac-4495-merge-silence-persistence-service

   Suppress output from MITK persistence service.

 * DCMTK not found because of directory path case mismatch

   MITK bug: 19588

   Commits:

   * 9afa98f Get canonical directory name before comparing with STREQUAL
