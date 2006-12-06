//                       YaST2-GTK                                //
// YaST webpage - http://developer.novell.com/wiki/index.php/YaST //

/* YGtkRatioBox is an improvement over the GtkBox container that
   allows the programmer to set stretch weights to containees.

   It is similar to GtkBox in usage but instead of feeding an
   expand boolean, a weight is given instead. In fact, it should
   behave just the same as a GtkBox if you give 0 for not expand
   and 1 to.
*/

#ifndef YGTK_RATIO_BOX_H
#define YGTK_RATIO_BOX_H

#include <gdk/gdk.h>
#include <gtk/gtkcontainer.h>
G_BEGIN_DECLS

#define YGTK_TYPE_RATIO_BOX            (ygtk_ratio_box_get_type ())
#define YGTK_RATIO_BOX(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
                                        YGTK_TYPE_RATIO_BOX, YGtkRatioBox))
#define YGTK_RATIO_BOX_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  \
                                        YGTK_TYPE_RATIO_BOX, YGtkRatioBoxClass))
#define YGTK_IS_RATIO_BOX(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
                                        YGTK_TYPE_RATIO_BOX))
#define YGTK_IS_RATIO_BOX_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  \
                                        YGTK_TYPE_RATIO_BOX))
#define YGTK_RATIO_BOX_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  \
                                        YGTK_TYPE_RATIO_BOX, YGtkRatioBoxClass))

typedef struct _YGtkRatioBox       YGtkRatioBox;
typedef struct _YGtkRatioBoxClass  YGtkRatioBoxClass;
typedef struct _YGtkRatioBoxChild  YGtkRatioBoxChild;

struct _YGtkRatioBox
{
	GtkContainer container;

	// members (read-only)
	GList *children;
	gint16 spacing;
	gboolean homogeneous;
};

struct _YGtkRatioBoxClass
{
	GtkContainerClass parent_class;
};

struct _YGtkRatioBoxChild
{
	GtkWidget *widget;
	// Proprieties
	guint16 padding;
	gfloat ratio;
	guint xfill : 1;
	guint yfill : 1;
	guint pack : 1;
	guint fully_expandable : 1;
};

GType ygtk_ratio_box_get_type (void) G_GNUC_CONST;

void ygtk_ratio_box_set_homogeneous (YGtkRatioBox *box, gboolean homogeneous);
void ygtk_ratio_box_set_spacing (YGtkRatioBox *box, gint spacing);

void ygtk_ratio_box_pack_start (YGtkRatioBox *box, GtkWidget *child, gfloat ratio,
                                gboolean xfill, gboolean yfill, guint padding);
void ygtk_ratio_box_pack_end (YGtkRatioBox *box, GtkWidget *child, gfloat ratio,
                              gboolean xfill, gboolean yfill, guint padding);

// Since ratio may be of any value, this function is provided to let programmers
// specify this child to be fully expandable (avoid its use; stick to some fixed range)
// NOTE: when disabling expand, you should call child_packing to define a ratio
// since it is lost.
void ygtk_ratio_box_set_child_expand (YGtkRatioBox *box, GtkWidget *child,
                                      gboolean expand);
void ygtk_ratio_box_set_child_ratio (YGtkRatioBox *box, GtkWidget *child,
                                     gfloat ratio);

void ygtk_ratio_box_set_child_packing (YGtkRatioBox *box, GtkWidget *child,
                                       gfloat ratio, gboolean xfill,
                                       gboolean yfill, guint padding,
                                       GtkPackType pack_type);

void ygtk_ratio_box_get_child_packing (YGtkRatioBox *box, GtkWidget *child,
                                       gfloat *ratio, gboolean *xfill,
                                       gboolean *yfill, guint *padding,
                                       gboolean *fully_expandable,
                                       GtkPackType *pack_type);

/* RatioHBox */

#define YGTK_TYPE_RATIO_HBOX            (ygtk_ratio_hbox_get_type ())
#define YGTK_RATIO_HBOX(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
                                        YGTK_TYPE_RATIO_HBOX, YGtkRatioHBox))
#define YGTK_RATIO_HBOX_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  \
                                        YGTK_TYPE_RATIO_HBOX, YGtkRatioHBoxClass))
#define YGTK_IS_RATIO_HBOX(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
                                        YGTK_TYPE_RATIO_HBOX))
#define YGTK_IS_RATIO_HBOX_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  \
                                        YGTK_TYPE_RATIO_HBOX))
#define YGTK_RATIO_HBOX_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  \
                                        YGTK_TYPE_RATIO_HBOX, YGtkRatioHBoxClass))

typedef struct _YGtkRatioHBox       YGtkRatioHBox;
typedef struct _YGtkRatioHBoxClass  YGtkRatioHBoxClass;

struct _YGtkRatioHBox
{
	YGtkRatioBox ratiobox;
};

struct _YGtkRatioHBoxClass
{
	YGtkRatioBoxClass parent_class;
};

GtkWidget* ygtk_ratio_hbox_new (gboolean homogeneous, gint spacing);
GType ygtk_ratio_hbox_get_type (void) G_GNUC_CONST;

/* RatioVBox */

#define YGTK_TYPE_RATIO_VBOX            (ygtk_ratio_vbox_get_type ())
#define YGTK_RATIO_VBOX(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
                                        YGTK_TYPE_RATIO_VBOX, YGtkRatioVBox))
#define YGTK_RATIO_VBOX_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  \
                                        YGTK_TYPE_RATIO_VBOX, YGtkRatioVBoxClass))
#define YGTK_IS_RATIO_VBOX(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
                                        YGTK_TYPE_RATIO_VBOX))
#define YGTK_IS_RATIO_VBOX_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  \
                                        YGTK_TYPE_RATIO_VBOX))
#define YGTK_RATIO_VBOX_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  \
                                        YGTK_TYPE_RATIO_VBOX, YGtkRatioVBoxClass))

typedef struct _YGtkRatioVBox       YGtkRatioVBox;
typedef struct _YGtkRatioVBoxClass  YGtkRatioVBoxClass;

struct _YGtkRatioVBox
{
	YGtkRatioBox ratiobox;
};

struct _YGtkRatioVBoxClass
{
	YGtkRatioBoxClass parent_class;
};

GtkWidget* ygtk_ratio_vbox_new (gboolean homogeneous, gint spacing);
GType ygtk_ratio_vbox_get_type (void) G_GNUC_CONST;

G_END_DECLS
#endif /* YGTK_RATIO_BOX_H */

/* YGtkMinSize is container where a widget may be installed so that one can
   set a minimum size to it. */
#ifndef YGTK_MIN_SIZE_H
#define YGTK_MIN_SIZE_H

#include <gtk/gtkbin.h>
G_BEGIN_DECLS

#define YGTK_TYPE_MIN_SIZE            (ygtk_min_size_get_type ())
#define YGTK_MIN_SIZE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
                                       YGTK_TYPE_MIN_SIZE, YGtkMinSize))
#define YGTK_MIN_SIZE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  \
                                       YGTK_TYPE_MIN_SIZE, YGtkMinSizeClass))
#define YGTK_IS_MIN_SIZE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
                                       YGTK_TYPE_MIN_SIZE))
#define YGTK_IS_MIN_SIZE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  \
                                       YGTK_TYPE_MIN_SIZE))
#define YGTK_MIN_SIZE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  \
                                       YGTK_TYPE_MIN_SIZE, YGtkMinSizeClass))

typedef struct _YGtkMinSize       YGtkMinSize;
typedef struct _YGtkMinSizeClass  YGtkMinSizeClass;

struct _YGtkMinSize
{
	GtkBin bin;
	// members
	guint min_width, min_height;
};

struct _YGtkMinSizeClass
{
	GtkBinClass parent_class;
};

GType ygtk_min_size_get_type (void) G_GNUC_CONST;
GtkWidget* ygtk_min_size_new (guint min_width, guint min_height);

void ygtk_min_size_set_width (YGtkMinSize *min_size, guint min_width);
void ygtk_min_size_set_height (YGtkMinSize *min_size, guint min_height);

G_END_DECLS
#endif /*YGTK_MIN_SIZE_H*/
