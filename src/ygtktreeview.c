/********************************************************************
 *           YaST2-GTK - http://en.opensuse.org/YaST2-GTK           *
 ********************************************************************/

/* YGtkTreeView widget */
// check the header file for information about this widget

#include <config.h>
#include "ygtktreeview.h"
#include <gtk/gtk.h>
#define YGI18N_C
#include "YGi18n.h"

static guint right_click_signal = 0;

G_DEFINE_TYPE (YGtkTreeView, ygtk_tree_view, GTK_TYPE_TREE_VIEW)

static void ygtk_tree_view_init (YGtkTreeView *view)
{
}

static void ygtk_tree_view_finalize (GObject *object)
{
	YGtkTreeView *view = YGTK_TREE_VIEW (object);
	if (view->empty_text) {
		g_free (view->empty_text);
		view->empty_text = NULL;
	}
	G_OBJECT_CLASS (ygtk_tree_view_parent_class)->finalize (object);
}

static void _gtk_widget_destroy (gpointer widget)
{ gtk_widget_destroy (GTK_WIDGET (widget)); }

void ygtk_tree_view_popup_menu (YGtkTreeView *view, GtkWidget *menu)
{
	GtkWidget *widget = GTK_WIDGET (view);
	// popup hack -- we can't destroy the menu at hide because it may not have
	// emitted signals yet -- destroy it when replaced or the widget destroyed
	g_object_set_data_full (G_OBJECT (view), "popup", menu, _gtk_widget_destroy);

	guint32 time = gtk_get_current_event_time();
	gtk_menu_attach_to_widget (GTK_MENU (menu), widget, NULL);
	gtk_menu_popup (GTK_MENU (menu), NULL, NULL, NULL, NULL,  3, time);
	gtk_widget_show_all (menu);
}

static gboolean ygtk_tree_view_button_press_event (GtkWidget *widget, GdkEventButton *event)
{
	// workaround (based on gedit): we want the tree view to receive this press in order
	// to select the row, but we can't use connect_after, so we throw a dummy mouse press
	if (event->type == GDK_BUTTON_PRESS && event->button == 3) {
		static gboolean safeguard = FALSE;
		if (safeguard) return FALSE;
		safeguard = TRUE;

		GtkTreeView *view = GTK_TREE_VIEW (widget);
		GtkTreeSelection *selection = gtk_tree_view_get_selection (view);

		gboolean outreach;
		outreach = !gtk_tree_view_get_path_at_pos (view, event->x, event->y, 0, 0, 0, 0);
		if (gtk_tree_selection_count_selected_rows (selection) <= 1) {
			// if there is a selection, let it be
			event->button = 1;
			if (!gtk_widget_event (widget, (GdkEvent *) event))
				return FALSE;
		}
		g_signal_emit (widget, right_click_signal, 0, outreach);
		safeguard = FALSE;
		return TRUE;
	}
	return GTK_WIDGET_CLASS (ygtk_tree_view_parent_class)->button_press_event (widget, event);
}

static gboolean _ygtk_tree_view_popup_menu (GtkWidget *widget)
{
	GtkTreeView *view = GTK_TREE_VIEW (widget);
	GtkTreeSelection *selection = gtk_tree_view_get_selection (view);
	gboolean outreach = gtk_tree_selection_count_selected_rows (selection) == 0;

	g_signal_emit (widget, right_click_signal, 0, outreach);
	return TRUE;
}

static gboolean _ygtk_tree_view_expose_event (GtkWidget *widget, GdkEventExpose *event)
{
	GTK_WIDGET_CLASS (ygtk_tree_view_parent_class)->expose_event (widget, event);

	GtkTreeView *view = GTK_TREE_VIEW (widget);
	YGtkTreeView *yview = YGTK_TREE_VIEW (widget);
	if (yview->empty_text && event->window == gtk_tree_view_get_bin_window (view)) {
		GtkTreeModel *model = gtk_tree_view_get_model (view);
		GtkTreeIter iter;
		if (!model || !gtk_tree_model_get_iter_first (model, &iter)) {  // empty tree-view
			const gchar *text = yview->empty_text;
			if (!model)
				text = _("Loading...");
			cairo_t *cr = gdk_cairo_create (event->window);
			PangoLayout *layout;
			layout = gtk_widget_create_pango_layout (widget, text);

			PangoAttrList *attrs = pango_attr_list_new();
			pango_attr_list_insert (attrs, pango_attr_foreground_new (160<<8, 160<<8, 160<<8));
			pango_layout_set_attributes (layout, attrs);
			pango_attr_list_unref (attrs);

			int width, height;
			pango_layout_get_pixel_size (layout, &width, &height);
			int x, y;
			x = (widget->allocation.width - width) / 2;
			y = (widget->allocation.height - height) / 2;
			cairo_move_to (cr, x, y);

			pango_cairo_show_layout (cr, layout);
			g_object_unref (layout);
			cairo_destroy (cr);
		}
	}
	return FALSE;
}

static void show_column_cb (GtkCheckMenuItem *item, GtkTreeView *view)
{
	int col = GPOINTER_TO_INT (g_object_get_data (G_OBJECT (item), "column"));
	GtkTreeViewColumn *column = gtk_tree_view_get_column (view, col);
	gboolean visible = gtk_check_menu_item_get_active (item);
	gtk_tree_view_column_set_visible (column, visible);
}

GtkWidget *ygtk_tree_view_create_show_columns_menu (YGtkTreeView *view)
{
	GtkWidget *menu = gtk_menu_new();
	GList *columns = gtk_tree_view_get_columns (GTK_TREE_VIEW (view));
	// see ygtk_tree_view_append_column(): we re-order arabic column insertion
	gboolean reverse = gtk_widget_get_default_direction() == GTK_TEXT_DIR_RTL &&
		gtk_widget_get_direction (GTK_WIDGET (view)) != GTK_TEXT_DIR_RTL;
	int n;
	GList *i;
	for (n = 0, i = columns; i; i = i->next, n++) {
		GtkTreeViewColumn *column = (GtkTreeViewColumn *) i->data;
		const gchar *header = gtk_tree_view_column_get_title (column);
		if (header) {
			GtkWidget *item = gtk_check_menu_item_new_with_label (header);
			g_object_set_data (G_OBJECT (item), "column", GINT_TO_POINTER (n));

			gboolean visible = gtk_tree_view_column_get_visible (column);
			gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (item), visible);
			g_signal_connect (G_OBJECT (item), "toggled",
				G_CALLBACK (show_column_cb), view);

			if (reverse)
				gtk_menu_shell_prepend (GTK_MENU_SHELL (menu), item);
			else
				gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);

			// if the user were to remove all columns, the right-click would no longer work
			if (n == 0 && !reverse)
				gtk_widget_set_sensitive (item, FALSE);
		}
	}
	g_list_free (columns);
	return menu;
}

GtkWidget *ygtk_tree_view_append_show_columns_item (YGtkTreeView *view, GtkWidget *menu)
{
	if (!menu)
		menu = gtk_menu_new();

	GList *children = gtk_container_get_children (GTK_CONTAINER (menu));
	g_list_free (children);
	if (children)  // non-null if it has children (in which case, add separator)
		gtk_menu_shell_append (GTK_MENU_SHELL (menu), gtk_separator_menu_item_new());

	GtkWidget *submenu = ygtk_tree_view_create_show_columns_menu (view);
	GtkWidget *item = gtk_menu_item_new_with_mnemonic (_("_Show column"));
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (item), submenu);
	return menu;
}

void ygtk_tree_view_append_column (YGtkTreeView *view, GtkTreeViewColumn *column)
{
	int pos = -1;
	if (gtk_widget_get_default_direction() == GTK_TEXT_DIR_RTL) {
		gtk_widget_set_direction (GTK_WIDGET (view), GTK_TEXT_DIR_LTR);

		GList *renderers = gtk_cell_layout_get_cells (GTK_CELL_LAYOUT (column)), *i;
		for (i = renderers; i; i = i->next) {
			GtkCellRenderer *renderer = (GtkCellRenderer *) i->data;
			if (GTK_IS_CELL_RENDERER_TEXT (renderer)) {
				PangoAlignment alignment;
				g_object_get (G_OBJECT (renderer), "alignment", &alignment, NULL);
				if (alignment == PANGO_ALIGN_LEFT)
					alignment = PANGO_ALIGN_RIGHT;
				else if (alignment == PANGO_ALIGN_RIGHT)
					alignment = PANGO_ALIGN_LEFT;
				g_object_set (G_OBJECT (renderer), "alignment", alignment, NULL);

				gfloat xalign;
				g_object_get (G_OBJECT (renderer), "xalign", &xalign, NULL);
				xalign = 1.0 - xalign;
				g_object_set (G_OBJECT (renderer), "xalign", xalign, NULL);

				PangoEllipsizeMode ellipsize;
				g_object_get (G_OBJECT (renderer), "ellipsize", &ellipsize, NULL);
				if (ellipsize == PANGO_ELLIPSIZE_END)
					ellipsize = PANGO_ELLIPSIZE_START;
				else if (ellipsize == PANGO_ELLIPSIZE_START)
					ellipsize = PANGO_ELLIPSIZE_END;
				g_object_set (G_OBJECT (renderer), "ellipsize", ellipsize, NULL);
			}
		}
		g_list_free (renderers);
		pos = 0;
	}
	gtk_tree_view_insert_column (GTK_TREE_VIEW (view), column, pos);
}

GtkTreeViewColumn *ygtk_tree_view_get_column (YGtkTreeView *view, gint nb)
{
	GtkTreeViewColumn *column;
	if (gtk_widget_get_default_direction() == GTK_TEXT_DIR_RTL) {
		GList *columns = gtk_tree_view_get_columns (GTK_TREE_VIEW (view));
		nb = g_list_length (columns) - nb - 1;
		column = g_list_nth_data (columns, nb);
		g_list_free (columns);
	}
	else
		column = gtk_tree_view_get_column (GTK_TREE_VIEW (view), nb);
	return column;
}


void ygtk_tree_view_set_empty_text (YGtkTreeView *view, const gchar *empty_text)
{
	if (view->empty_text)
		g_free (view->empty_text);
	view->empty_text = empty_text ? g_strdup (empty_text) : NULL;
}

GtkWidget *ygtk_tree_view_new (const gchar *empty_text)
{
	YGtkTreeView *view = (YGtkTreeView *) g_object_new (YGTK_TYPE_TREE_VIEW, NULL);
	view->empty_text = empty_text ? g_strdup (empty_text) : NULL;
	return GTK_WIDGET (view);
}

static void ygtk_tree_view_class_init (YGtkTreeViewClass *klass)
{
	GtkWidgetClass *gtkwidget_class = GTK_WIDGET_CLASS (klass);
	gtkwidget_class->button_press_event = ygtk_tree_view_button_press_event;
	gtkwidget_class->popup_menu = _ygtk_tree_view_popup_menu;
	gtkwidget_class->expose_event = _ygtk_tree_view_expose_event;

	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
	gobject_class->finalize = ygtk_tree_view_finalize;

	right_click_signal = g_signal_new ("right-click",
		G_OBJECT_CLASS_TYPE (klass), G_SIGNAL_RUN_LAST,
		G_STRUCT_OFFSET (YGtkTreeViewClass, right_click),
		NULL, NULL, gtk_marshal_VOID__BOOLEAN, G_TYPE_NONE, 1, G_TYPE_BOOLEAN);
}

