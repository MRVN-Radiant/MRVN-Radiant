# TODO

- [ ] New looks
  - [ ] New icon
    - Includes splashscreen
  - [x] Cooler readme
- [ ] Improve `COMPILING.md`

### Compiler

- [ ] Figure out LevelInfo
- [ ] General formatting
- [ ] Fix patch exporter
- [ ] Collisions
  - [ ] brush
  - [ ] tricoll
- [ ] gamelump exporter ( Verify it works, levelinfo is needed )
- [ ] Ent files
  - [ ] Basic spawns
  - [ ] Ziplines
  - [ ] AI nodes
- [ ] Lightmapper :trollface:
- [ ] basic r5 support
- [ ] basic r1 support

### Decompiler
- [ ] bsp importer so we can remaster tf1 maps
  - [ ] improve importer
  - [x] add tf1 game profile

### Editor
- [ ] mdl importer
- [ ] recursive folder support
  - currently to use a texture the game has at `textures/models/vistas/rise` your radiant path would need to be at `textures/models_vistas/rise` which isn't very user friendly
- [ ] basic vmt/ vtf support
- [ ] some sortof r2recast support ( at least a good .obj exporter )
- [ ] generally improve the editor
  - delete unused features
  - clean up
  - the editor very rarely freezes for a couple seconds -> why?
- [ ] edit flags in the surface menu ?
### Nice to haves ( low priority )
- [ ] vpk tool that takes a `.map` as an input and only packs used assets ? ( call it revpk :trollface: )
- [ ] repak or a bootleg version of repak for minimaps and loadscreens, maybe materials, same as above and other tools regarding input

## Old stuff I havent sorted yet

[-z-]: make rotate dialog non-modal
    


## BUGS

MSI: installer bug with new folders? : create custom dir, click New Folder icon, type "FOLDER\" - gets stuck

GTK2: gtk2 crashes when trying to use bitmap fonts such as MS Sans Serif http://bugzilla.gnome.org/show_bug.cgi?id=142579

GTK2: alt+tab while mouse button is held down: see http://bugzilla.gnome.org/show_bug.cgi?id=145156

UI: changing resolution in floating-windows mode can screw up window positions.

HalfLife: half-life maps saved in q1 map format are not supported - currently have to convert them to hammer map format using hammer editor. And vice versa.

Entity: creating a new entity with all the brushes of another entity selected results in the latter entity having no brushes.

SConscript: build fails if SETUP=1

SConscript: svn.py fails if not using C locale - set LC_ALL?

GUI: can't use arrow keys to navigate in camera view when capslock is enabled

GUI: screensaver causes: gdkgc-win32.c: line 905 (gdk_win32_hdc_get): assertion failed: (win32_gc->hdc == NULL)


### FEATURES

- paint-select or equivalent (e.g. area-selection with occlusion)
- select-complete-tall or equivalent (e.g. subtract-from-selection modifier key)
- texture pane names are often illegible, because 1. they are long and overlap each other and 2. they overlap the outline rectangles around the images themselves.


Build: document build-menu xml format.
The build menu in GtkRadiant 1.5 is entirely customisable - you can make it run qbsp3/qvis3/arghrad or any tool you want. Use 'Build > Customize...' to edit the menu.

Menu commands are the shell commands that Radiant will execute when you choose the menu item. You can add as many commands as you want to a single menu item, and they will be executed in sequence. The commands contain variables, specified using []. The values of variables will be substituted when the command is executed.

For example:
<pre>[q2map] -bsp "[MapFile]"</pre>
becomes:
<pre>"C:\Program Files\GtkRadiant 1.5.0\q2map" -fs_basepath "c:\quake2" -bsp "c:\quake2\baseq2\maps\blah.map"</pre>
This uses the predefined variable 'MapFile' and the custom variable 'q2map'. 'q2map' is defined in the XML file, and 'MapFile' is the full path to your map.
The 'MapFile' variable is enclosed in quotes, because the path to your map may contain spaces.
At the moment you can only create custom variables by editing the XML file. A custom variable for arghrad would look something like this:
<pre><var name="arghrad">"[RadiantPath]arghrad"</var></pre>
This variable could then be used in a command like this:
<pre>[arghrad] "[MapFile]"</pre>

Entity: option to filter non-world entities (e.g. not func_group or func_static)

Rotate Tool: if more than one object is selected, with different local orientations, use parent-space rotation pivot instead of local-space

Brush: MMB+ctrl to paint texture on whole brush/patch.

Camera: add alternative highlighting styles (used to be J).

Doom3: filter func_splinemovers

Entity: draw arrowheads to show direction of connection-lines.

? MMB to select a texture should also apply that texture to all selected faces.

Mouse: support 2-button mouse.

Grid: background colour should be different when the smallest grid is invisible due to being zoomed out.

Brush: option to disable dots on selected faces when not in face mode.

Entity: draw direction arrow for func_door and func_button angle.

Build Menu: support for editing variables.

Shaders: handle doom3 materials with multiple bumpmaps stage - use first stage, ignore later stages.

Brush: warn when a brush is dragged into a configuration with <0 volume

Textures: add option to give new brushes a specific texture instead of the last selected.

? QE-tool: click anywhere on xy view to drag entity instead of requiring clicking directly on entity.

UserDocs: how to use multi-vertex selection - replaces vertex-edit-splits-faces option:

UserDocs: how to use parent-selection:
  - Parent-selection works like Maya: it allows you to 'reparent' brushes
  - onto other entities than the one they're currently part of. To use it,
  - select some brushes, select an entity, Edit -> Parent.

Textures: add anisotropic filtering.

Preferences: allow preference settings to be shared across games.

Preferences: add colour 'theme' files using prefs format.

Preferences: sensible default size for prefs window.

Doom3: add model browser.

Doom3: s_diversity light key.

HalfLife: enable HL-mode on linux/osx.

Renderer: doom3 'parallel' and 'spot' light support.

Entity: add mouse-editing for doom3 light_center key

Shaders: add support for texture transforms.

Shaders: add support for 'addnormals' keyword - e.g. models/mapobjects/healthgui/healthguidirty

TGA Loader: check that true-colour images with palettes are properly handled.

Module System: reinstate 'refresh' feature.

Surface Inspector: add button for 'axial' projection for doom3.

Build: fix hardcoded engine-launch commands - use similar system to build-menu command description.

Filters: use q2/heretic2 content flags to filter brushes.

? Surface Inspector: allow material names not relative to 'textures/' for doom3

Module System: add versioning for module-system api.

svn: remove install/ dir, create it during build process on win32

Editing: add option to choose the default startup tool mode.

Renderer: lighting for doom3 materials without bumpmaps (e.g. mcity/mchangar2)

Renderer: realtime doom3 materials preview

Renderer: realtime doom3 shadows preview

Linux: Provide .tar.gz of example-map data for et/wolf.

Textures Window: add inner dark outline to distinguish 'is-shader' outline from white textures.

HalfLife2: add HL2 map load/save.

Selection: add move-pivot mode to allow rotation/scale around a custom pivot-point.

Selection: add rotate increment for rotate manipulator.

Selection: visibly distinguish between entity and brush selections

Selection: need 'add to selection' and 'subtract from selection' modifiers

Selection: Finish scale manipulator.

FaceCopy/PasteTexture: Make face-copy/paste-texture shortcuts customisable.

Manual: add documentation about search paths for .ent/.def/.fgd, shaders etc for each game.

Halflife: add support for cstrike fgd.

HalfLife: disable patches

HalfLife: add HL .mdl model loader.

HalfLife: add HL .spr support.

HalfLife: support fgd 'flags' attributes.

Model: add support for doom3 md5anim format

Model: support doom3 ragdolls

VFS: add ability to browse VFS from file-open dialogs.

Installer: enable q3 brush-primitives map support.

Installer: add editor manual to linux installer

Map: add conversion between map formats

Map: add conversion between entity definition formats

Build: add build-menu dmap support (doom3)

Entity: optionally draw target connection lines thicker than one pixel.

Entity: add specialised attribute-entry in entity-inspector for integer/real/color attribute types.

Patch: add cap-texture, fit-texture and natural-texture toolbar buttons

Patch: draw patches in wireframe from the back, make patches selectable from the back

Patch: add option for convert-selection-to-new-brush/patch

Patch: fix bobtoolz merge-patches feature

Patch: fix insert/remove rows/cols indicated by current selected patch vertices.

Autosave/Snapshots: Add support for multi-file maps.

Quake2: Q2 hint transparency support

Shortcuts: make shortcut list editable within radiant.

Shortcuts: convert shortcuts.ini to xml.

Shortcuts: warn when duplicate shortcuts are registered

Shortcuts: rename commands in order to group shortcuts list better.

upgrade to new API for SymGetModuleInfo - required for compiling with Visual Studio 8.0

Doom3: lights should stay in place while resizing


### LOW priority features

Selection: Add shear manipulator?

Textures Window: Improve texture-manipulation and texture-browsing tools.

Undo: make selections undoable?

Win32 Installer: Automatically upgrade existing installation.

General: refactor game-specific hacks to be parameterised by .game file

Patch: Overlays, Bend Mode, Thicken.

Brush: Add brush-specific plugin API.

Entity: Draw light style numbers.

Entity: Show models with model2 key.

Entity: Interpret _remap* key (_MindLink_).

Entity: Support _origin _angles _scale on groups.

Selection: Add Primitive-mode shortcut key/button.

Selection: Customisable manipulator size - +/- to change the size of the translate/rotate tool. 

Selection: Add optional screen-relative control for constrained rotations.

Clipper: Change selection/manipulation to be consistent with other component editing.

Filtering: Either deselect filtered nodes, or render filtered nodes that are selected.

Filtering: Add customisable filter presets to set/unset multiple filters at once.

Texdef: Make texdef formats abstract, add conversion between texdef formats (use generic affine-texture-matrix format for conversions).

Textures Window: Precise display of texture size when selecting.  (tooltip, possibly)

Status: 'Size of brush' display on status bar.

Colours: maya scheme default?

Quake: add support for adjusting gamma on quake palette?
