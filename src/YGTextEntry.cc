//                       YaST2-GTK                                //
// YaST webpage - http://developer.novell.com/wiki/index.php/YaST //

#include <config.h>
#include <ycp/y2log.h>
#include <YGUI.h>
#include "YTextEntry.h"
#include "YGUtils.h"
#include "YGWidget.h"

class YGTextEntry : public YTextEntry, public YGLabeledWidget
{
public:
	YGTextEntry (const YWidgetOpt &opt,
		             YGWidget *parent,
		             const YCPString &label,
		             const YCPString &text)
		: YTextEntry (opt, label)
		, YGLabeledWidget (this, parent, label, YD_VERT, true, GTK_TYPE_ENTRY, NULL)
	{
		setText (text);

		if (opt.passwordMode.value())
			gtk_entry_set_visibility (GTK_ENTRY (getWidget()), FALSE);

		// Signals to know of any text modification
		g_signal_connect (G_OBJECT (getWidget()), "insert-text",
		                  G_CALLBACK (text_inserted_cb), this);
		g_signal_connect (G_OBJECT (getWidget()), "delete-text",
		                  G_CALLBACK (text_deleted_cb), this);
	}

	virtual ~YGTextEntry() {}

	// YTextEntry
	virtual YCPString getText()
	{
		return YCPString (gtk_entry_get_text (GTK_ENTRY (getWidget())));
	}

	virtual void setInputMaxLength (const YCPInteger &numberOfChars)
	{
		gtk_entry_set_width_chars (GTK_ENTRY (getWidget()),
		                           numberOfChars->asInteger()->value());
	}

	virtual void setText (const YCPString &text)
	{
		/* No need to check for valid chars as that's the responsible of the YCP
		   application programmer. */
		gtk_entry_set_text (GTK_ENTRY (getWidget()), text->value_cstr());
	}

	// YWidget
	YGWIDGET_IMPL_NICESIZE
	YGWIDGET_IMPL_SET_ENABLING
	YGWIDGET_IMPL_SET_SIZE
	YGWIDGET_IMPL_KEYBOARD_FOCUS

	// YGLabelWidget
	YGLABEL_WIDGET_IMPL_SET_LABEL_CHAIN(YTextEntry)

	static void text_inserted_cb (GtkEditable *editable, gchar *text,
	                              gint length, gint *position,
	                              YGTextEntry *pThis)
	{
		if (YGUtils::filterText (text, length, pThis->getValidChars()->value_cstr())
		    != text) {
			g_signal_stop_emission_by_name (editable, "insert_text");
			gdk_beep();
		}
		else
			pThis->emitEvent (YEvent::ValueChanged);
	}

	static void text_deleted_cb (GtkEditable *editable, gint start_pos,
	                             gint end_pos, YGTextEntry *pThis)
	{
		pThis->emitEvent (YEvent::ValueChanged);
	}
};

YWidget *
YGUI::createTextEntry (YWidget *parent, YWidgetOpt & opt,
                       const YCPString & label, const YCPString & text)
{
	return new YGTextEntry (opt, YGWidget::get (parent), label, text);
}
