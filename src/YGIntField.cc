/********************************************************************
 *           YaST2-GTK - http://en.opensuse.org/YaST2-GTK           *
 ********************************************************************/

#include <config.h>
#include <ycp/y2log.h>
#include "YGUI.h"
#include "YGUtils.h"
#include "YGWidget.h"

#define SLIDER_NICESIZE_IN_CHARS 15

class YGSpinBox : public YGLabeledWidget
{
	YIntField *m_yfield;
	GtkWidget *m_spiner, *m_slider;

public:
	YGSpinBox (YWidget *ywidget, YWidget *parent, const string &label,
	           int minValue, int maxValue, int initialValue, bool show_slider)
	: YGLabeledWidget (ywidget, parent, label, YD_HORIZ, true,
	                   GTK_TYPE_HBOX, NULL)
	{
		m_spiner = gtk_spin_button_new_with_range  (minValue, maxValue, 1);

		if (show_slider) {
			m_slider = gtk_hscale_new_with_range (minValue, maxValue, 1);
			if (maxValue - minValue < 20)
				// GtkScale by default uses a step of 10 -- use a lower for low values
				gtk_range_set_increments (GTK_RANGE (m_slider), 1, 2);
			gtk_scale_set_draw_value (GTK_SCALE (m_slider), FALSE);
			YGLabeledWidget::setBuddy (m_slider);
			gtk_widget_set_size_request (m_slider,
				YGUtils::getCharsWidth (m_slider, SLIDER_NICESIZE_IN_CHARS), -1);

			gtk_box_pack_start (GTK_BOX (getWidget()), m_slider, TRUE, TRUE, 0);
			gtk_box_pack_start (GTK_BOX (getWidget()), m_spiner, FALSE, FALSE, 5);
			gtk_widget_show (m_slider);
		}
		else {
			m_slider = NULL;
			YGLabeledWidget::setBuddy (m_spiner);

			gtk_container_add (GTK_CONTAINER (getWidget()), m_spiner);
		}
		gtk_widget_show (m_spiner);

		doSetValue (initialValue);
		g_signal_connect (G_OBJECT (m_spiner), "value-changed",
		                  G_CALLBACK (spiner_changed_cb), this);
        if (m_slider)
		    g_signal_connect (G_OBJECT (m_slider), "value-changed",
                              G_CALLBACK (slider_changed_cb), this);
	}

	GtkSpinButton *getSpiner()
	{ return GTK_SPIN_BUTTON (m_spiner); }

	bool useSlider()
    { return m_slider != NULL; }
	GtkHScale *getSlider()
	{ return GTK_HSCALE (m_slider); }

	virtual void reportValue (int value) = 0;

	int doGetValue()
	{
		return gtk_spin_button_get_value_as_int (getSpiner());
	}

	void doSetValue (int newValue)
	{
		gtk_spin_button_set_value (getSpiner(), newValue);
		if (useSlider())
			gtk_range_set_value (GTK_RANGE (getSlider()), newValue);
	}

	// Events callbacks
	static void spiner_changed_cb (GtkSpinButton *widget, YGSpinBox *pThis)
	{
		int value = gtk_spin_button_get_value_as_int (pThis->getSpiner());
		pThis->reportValue (value);
		if (pThis->useSlider())
			gtk_range_set_value (GTK_RANGE (pThis->getSlider()), value);
		pThis->emitEvent (YEvent::ValueChanged);
	}

	static void slider_changed_cb (GtkRange *range, YGSpinBox *pThis)
	{
		int value = (int) gtk_range_get_value (range);
		gtk_spin_button_set_value (pThis->getSpiner(), value);
		pThis->reportValue (value);
		pThis->emitEvent (YEvent::ValueChanged);
	}
};

#define YGSPIN_BOX_IMPL_SET_VALUE_CHAIN(ParentClass) \
	virtual void reportValue (int value) {           \
		ParentClass::setValue (value);               \
	}                                                \
	virtual int value() {                            \
		return doGetValue();                         \
	}                                                \
	virtual void setValueInternal (int value) {      \
		doSetValue (value);                          \
	}

#include "YIntField.h"

class YGIntField : public YIntField, public YGSpinBox
{
public:
	YGIntField (YWidget *parent, const string &label, int minValue, int maxValue,
	            int initialValue)
	: YIntField (NULL, label, minValue, maxValue)
	, YGSpinBox (this, parent, label, minValue, maxValue, initialValue, false)
	{}

	YGWIDGET_IMPL_COMMON
	YGLABEL_WIDGET_IMPL_SET_LABEL_CHAIN (YIntField)
	YGSPIN_BOX_IMPL_SET_VALUE_CHAIN (YIntField)
};

YIntField *YGWidgetFactory::createIntField (YWidget *parent, const string &label,
                                            int minValue, int maxValue, int initialValue)
{
	return new YGIntField (parent, label, minValue, maxValue, initialValue);
}

#include "YSlider.h"

class YGSlider : public YSlider, public YGSpinBox
{
public:
	YGSlider (YWidget *parent, const string &label, int minValue, int maxValue,
	          int initialValue)
	: YSlider (NULL, label, minValue, maxValue)
	, YGSpinBox (this, parent, label, minValue, maxValue, initialValue, true)
	{}

	YGWIDGET_IMPL_COMMON
	YGLABEL_WIDGET_IMPL_SET_LABEL_CHAIN (YSlider)
	YGSPIN_BOX_IMPL_SET_VALUE_CHAIN (YIntField)
};

YSlider *YGOptionalWidgetFactory::createSlider (YWidget *parent, const string &label,
                                                int minValue, int maxValue, int initialValue)
{
	IMPL
	return new YGSlider (parent, label, minValue, maxValue, initialValue);
}

