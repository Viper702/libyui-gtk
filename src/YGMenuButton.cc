//                       YaST2-GTK                                //
// YaST webpage - http://developer.novell.com/wiki/index.php/YaST //

#include <config.h>
#include <ycp/y2log.h>
#include <YGUI.h>
#include "YGUtils.h"
#include "YGWidget.h"
#include "YMenuButton.h"
#include "ygtkmenubutton.h"

class YGMenuButton : public YMenuButton, public YGWidget
{
public:
	YGMenuButton (const YWidgetOpt &opt,
	              YGWidget         *parent,
	              YCPString         label)
	:  YMenuButton (opt, label),
	   YGWidget (this, parent, true, YGTK_TYPE_MENU_BUTTON, NULL)
	{
		IMPL
		setLabel (label);
	}

	virtual ~YGMenuButton() {}

	// YMenuButton
	virtual void setLabel (const YCPString &label)
	{
		IMPL
		string str = YGUtils::mapKBAccel (label->value_cstr());
		ygtk_menu_button_set_label (YGTK_MENU_BUTTON (getWidget()), str.c_str());
		YMenuButton::setLabel (label);
	}

	// YMenuButton
	virtual void createMenu()
	{
		ygtk_menu_button_set_popup (YGTK_MENU_BUTTON (getWidget()),
			doCreateMenu (getToplevelMenu()->itemList()));
	}

	static GtkWidget* doCreateMenu (YMenuItemList &items)
	{
		GtkWidget *menu = gtk_menu_new();
		for (YMenuItemListIterator it = items.begin(); it != items.end(); it++) {
			GtkWidget *entry;
			string str = YGUtils::mapKBAccel ((*it)->getLabel()->value_cstr());
			entry = gtk_menu_item_new_with_mnemonic (str.c_str());

			gtk_menu_shell_append (GTK_MENU_SHELL (menu), entry);

			if ((*it)->hasChildren())
				gtk_menu_item_set_submenu (GTK_MENU_ITEM (entry),
				                           doCreateMenu ((*it)->itemList()));
			else
				g_signal_connect (G_OBJECT (entry), "activate",
				                  G_CALLBACK (selected_item_cb), *it);
		}

		return menu;
	}

	static void selected_item_cb (GtkMenuItem *menuitem, YMenuItem *yitem)
	{
		YGUI::ui()->sendEvent (new YMenuEvent (yitem->getId()));
	}

	// YWidget
	YGWIDGET_IMPL_NICESIZE
	YGWIDGET_IMPL_SET_SIZE
	YGWIDGET_IMPL_SET_ENABLING
};

YWidget *
YGUI::createMenuButton (YWidget *parent, YWidgetOpt &opt,
                        const YCPString &label)
{
	return new YGMenuButton (opt, YGWidget::get (parent), label);
}
