# $Id$
<<<<<<< pldefaults.tcl
# $Log$
# Revision 1.10.2.2  2001/01/22 09:09:01  rlaboiss
# Merge of DEBIAN and v5_0_1 branches (conflicts are still to be solved)
#
# Revision 1.10.2.1  2001/01/22 09:05:11  rlaboiss
# Debian stuff corresponding to package version 4.99j-11
#
# Revision 1.10  1995/09/01  20:19:58  mjl
# Run tk_focusFollowsMouse under Tk 4.0 to ensure that the new scale keypress
# bindings are active.  Added bindings for scale focus in/out under Tk 3.6 and
# keypress bindings to emulate the Tk 4.0 behavior.  Changed button 1 binding
# for scales under Tk 4.0 to be the same as in Tk 3.6, which is much more
# useful IMO (button-1 in trough moves slider to that position; button-2 moves
# incrementally).  Now scale behavior should be the same under both Tk 3.6 or
# Tk 4.0 with the exception of the button-2 binding (which can't be reproduced
# under Tk 3.6 because it's scale widget isn't powerful enough).
#
# Revision 1.9  1995/06/02  20:31:53  mjl
# Set zoom starting point to default to center.  Better for zooming in on
# small plot features, although worse for large objects (e.g. entire plots).
#
# Revision 1.8  1995/05/26  20:11:10  mjl
# Added global variables for setting defaults for zoom and save options.
#
# Revision 1.7  1995/05/19  22:21:26  mjl
# Fixes for Tk 4.0.  The default options in the options database changed in
# some cases so that apps look the same as before.  Users can change these
# to taste.  The test for mono system using the line
#      if {[tk colormodel $w] == "color"}
# (this command was eliminated) has been replaced by
#      if {[winfo depth $w] == 1}
# which has the same effect.
#
# Revision 1.6  1995/03/16  22:59:30  mjl
# Revamped modifier key acceleration scheme.  Now controlled by two global
# variables: "global key_scroll_mag", which defaults to 5 and is the
# magnification for each modifier key added, and "key_scroll_speed", which
# defaults to 1 and is the baseline speed.  I probably should convert to
# using real resources for these; maybe for the next cut.
#
# Revision 1.5  1995/01/13  23:19:50  mjl
# Made a bunch of settings global.
#
# Revision 1.4  1994/09/27  21:56:30  mjl
# Changed print key to "P" because it's too easy to hit by mistake.
#
# Revision 1.3  1994/06/17  21:22:15  mjl
# Removed check for color system before setting resources.  Eliminates some
# problems with the Tk/DP drivers on mono displays.
#
# Revision 1.2  1994/04/25  18:50:18  mjl
# Added resource setting for scale fonts (used in palette widgets).
# Also added variables governing fast and faster scrolling speed.
#
# Revision 1.1  1994/04/08  12:07:31  mjl
# Proc to set up defaults.  Should not be modified by the user (use
# plconfig instead).
#
=======
>>>>>>> 1.13
#----------------------------------------------------------------------------
#
# Sets default configuration options for plplot/TK driver.
# Maurice LeBrun
# IFS, University of Texas
#
# This is largely based on options.motif.tk from Ioi K Lam's Tix package.
# The TK options are set to be fairly Motif-like.
#
# It is very easy to customize plplot/TK settings for a particular site
# or user.  The steps are:
#
# 1. Copy the desired settings from here into the plconfig proc in
#    plconfig.tcl, modifying them to taste
#
# 2. Deposit the modified plconfig.tcl in the desired directory for
#    autoloading. The autoload path used by the TK driver and plserver is
#    as follows:
#
#	 user-specified directory(s) (set by -auto_load argument)
#	 Current directory
#	 $PL_LIBRARY 
#	 $HOME/tcl
#	 INSTALL_DIR/tcl
#
# 3. Create a tclIndex file for plconfig.tcl in the same directory.  I
#    use an alias that makes this easy:
#
#    alias mktclidx  "echo 'auto_mkindex . *.tcl; destroy .' | wish"
#
#    Then just "mktclidx" will do the trick.
#
#
# This scheme allows for a clear and easy way to selectively modify
# defaults on a site, user, or project dependent basis (more easily than
# can be done with resources alone).
#
#----------------------------------------------------------------------------

proc pldefaults {} {
    global tk_version
    global gen_font
    global gen_bold_font
    global gen_menu_font
    global gen_italic_font
    global gen_font_small
    global gen_bold_font_small
    global gen_fixed_font

    global dialog_font
    global dialog_bold_font

# Font-related resources.

    set gen_font		-*-helvetica-medium-r-normal-*-*-180-*
    set gen_bold_font		-*-helvetica-bold-r-normal-*-*-180-*
    set gen_menu_font		-*-helvetica-medium-o-normal-*-*-180-*
    set gen_italic_font		-*-helvetica-bold-o-normal-*-*-180-*
    set gen_font_small		-*-helvetica-medium-r-normal-*-*-120-*
    set gen_bold_font_small	-*-helvetica-bold-r-normal-*-*-120-*
    set gen_fixed_font		-*-courier-medium-r-normal-*-*-180-*

    set dialog_font		-*-times-medium-r-normal-*-*-180-*
    set dialog_bold_font	-*-times-bold-r-normal-*-*-180-*

    option add *font		$gen_font
    option add *Entry.font	$gen_font
    option add *Menu*font	$gen_menu_font
    option add *Menubutton*font	$gen_menu_font
    option add *Scale.font	$gen_bold_font_small
    option add *color.font	$gen_fixed_font

#----------------------------------------------------------------------------
# Color-related resources. 
# Sets colors in a Motif-like way.
# It doesn't actually hurt to do this if not on a color system.

    set gen_bg		lightgray
    set gen_fg		black
    set gen_darker_bg	gray
    set gen_darker_fg	black
    set gen_active_bg	$gen_bg
    set gen_active_fg	$gen_fg

    option add *background			$gen_bg
    option add *foreground			$gen_fg
    option add *activeBackground		$gen_active_bg
    option add *activeForeground		$gen_active_fg
    option add *disabledForeground		gray45
    option add *Checkbutton.selector		yellow
    option add *Radiobutton.selector		yellow
    option add *Entry.background		#c07070
    option add *Entry.foreground		black
    option add *Entry.insertBackground		black
    option add *Listbox.background		$gen_darker_bg
    option add *Scale.foreground		$gen_fg
    option add *Scale.activeForeground		$gen_bg
    option add *Scale.background		$gen_bg
    option add *Scale.sliderForeground		$gen_bg
    option add *Scale.sliderBackground		$gen_darker_bg

    if {$tk_version < 4.0} then {
	option add *Scrollbar.foreground	$gen_bg
	option add *Scrollbar.activeForeground	$gen_bg
	option add *Scrollbar.background	$gen_darker_bg

    } else {
	option add *Scrollbar.background	$gen_bg
	option add *Scrollbar.troughColor	$gen_darker_bg
    }

# End of page indicator

    option add *leop.off			$gen_bg
    option add *leop.on				gray45

# This specifies the default plplot widget background color.
# A white background looks better on grayscale or mono.

    if {[winfo depth .] == 1} {
	option add *plwin.background		white
    } else {
	option add *plwin.background		black
    }

#----------------------------------------------------------------------------
# Miscellaneous 

    option add *anchor				w
    option add *Button.borderWidth		2
    option add *Button.anchor			c
    option add *Checkbutton.borderWidth		2
    option add *Radiobutton.borderWidth		2
    option add *Label.anchor			w
    option add *Labelframe.borderWidth		2
    option add *Entry.relief			sunken
    option add *Scrollbar.relief		sunken

# I have this in here so that applications written before Tk 4.0 still
# look the same.  More selectivity might be better.

    option add *highlightThickness		0

# Have focus follow mouse, only available in Tk 4.0+
# This is needed if you want to control scales using keystrokes.

    if {$tk_version >= 4.0} then {
	tk_focusFollowsMouse
    }

# Various options -- use global variables for simplicity.
# Not a great solution but will have to do for now.

# zoom options:
#  0:	0=don't preserve aspect ratio, 1=do
#  1:	0=stretch from corner, 1=stretch from center

    global zoomopt_0;		set zoomopt_0 1
    global zoomopt_1;		set zoomopt_1 1

# save options:
#  0:   name of default save device
#  1:   0=save 1 plot/file, 1=save multi plots/file (must close!)

    global saveopt_0;		set saveopt_0 psc
    global saveopt_1;		set saveopt_1 0

# Scale widget bindings
# Emulate the Tk 3.6 behavior in Tk 4.0+ for button-1 presses.
# Emulate the Tk 4.0+ behavior in Tk 3.6 for keystroke presses.

    if {$tk_version >= 4.0} then {
	bind Scale <Button-1>  {%W set [%W get %x %y] }
	bind Scale <B1-Motion> {%W set [%W get %x %y] }
    } else {
	bind Scale <Any-Enter> "focus %W"
	bind Scale <Any-Leave> "focus none"
	bind Scale <Left>  {%W set [expr [%W get] - 1]}
	bind Scale <Right> {%W set [expr [%W get] + 1]}
	bind Scale <Down>  {%W set [expr [%W get] - 1]}
	bind Scale <Up>    {%W set [expr [%W get] + 1]}
    }

# Key shortcut definitions -- change them if you want!
# Turn them into an empty string to disable.

    global key_zoom_select;	set key_zoom_select	"z"
    global key_zoom_back;	set key_zoom_back	"b"
    global key_zoom_forward;	set key_zoom_forward	"f"    
    global key_zoom_reset;	set key_zoom_reset	"r"
    global key_print;		set key_print		"P"
    global key_save_again;	set key_save_again	"s"
    global key_scroll_right;	set key_scroll_right	"Right"
    global key_scroll_left;	set key_scroll_left	"Left"
    global key_scroll_up;	set key_scroll_up	"Up"
    global key_scroll_down;	set key_scroll_down	"Down"
    global key_scroll_mag;	set key_scroll_mag	"5"
    global key_scroll_speed;	set key_scroll_speed	"1"

# enable/disable top plot and file menu

    global file_menu_on;	set file_menu_on "1"
    global plot_menu_on;	set plot_menu_on "1"
}
