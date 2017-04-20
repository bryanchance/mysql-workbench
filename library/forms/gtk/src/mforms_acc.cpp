/*
 * Copyright (c) 2017, Oracle and/or its affiliates. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of the
 * License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301  USA
 */

#include "../mforms_acc.h"
#include <gtk/gtk-a11y.h>
#include <atk/atk.h>
#include <atkmm.h>

namespace mforms {
  namespace gtk {

    static AtkRole convertAccessibleRole(mforms::Accessible::Role be_role) {
      AtkRole role = ATK_ROLE_INVALID;

      switch (be_role) {
        case mforms::Accessible::RoleNone:
          role = ATK_ROLE_INVALID;
          break;

        case mforms::Accessible::Client:
          role = ATK_ROLE_WINDOW;
          break;

        case mforms::Accessible::Pane:
          role = ATK_ROLE_PANEL;
          break;

        case mforms::Accessible::Link:
          role = ATK_ROLE_UNKNOWN;
          break;

        case mforms::Accessible::List:
          role = ATK_ROLE_LIST;
          break;

        case mforms::Accessible::ListItem:
          role = ATK_ROLE_LIST_ITEM;
          break;

        case mforms::Accessible::PushButton:
          role = ATK_ROLE_PUSH_BUTTON;
          break;

        case mforms::Accessible::StaticText:
          role = ATK_ROLE_LABEL;
          break;

        case mforms::Accessible::Text:
          role = ATK_ROLE_TEXT;
          break;

        case mforms::Accessible::Outline:
          role = ATK_ROLE_TREE_TABLE;
          break;

        case mforms::Accessible::OutlineButton:
          role = ATK_ROLE_UNKNOWN;
          break;

        case mforms::Accessible::OutlineItem:
        default:
          role = ATK_ROLE_UNKNOWN;

      }
      return role;
    }

    static gpointer mforms_object_accessible_parent_class = nullptr;

    static void mforms_object_accessible_init(mformsObjectAccessible *accessible) {
      mformsObjectAccessiblePrivate *priv = MFORMS_OBJECT_ACCESSIBLE_GET_PRIVATE(accessible);
      priv->mfoacc = nullptr;
    }

    static AtkStateSet *mforms_object_accessible_ref_state_set(AtkObject *accessible) {
      AtkStateSet *state_set = ATK_OBJECT_CLASS(mforms_object_accessible_parent_class)->ref_state_set(accessible);

      GtkWidget *widget = gtk_accessible_get_widget(GTK_ACCESSIBLE(accessible));
      if (widget == NULL)
        atk_state_set_add_state(state_set, ATK_STATE_DEFUNCT);
      else
        atk_state_set_add_state(state_set, ATK_STATE_DEFAULT);

      return state_set;
    }

    static void mforms_object_accessible_initialize(AtkObject *obj, gpointer data) {
      ATK_OBJECT_CLASS(mforms_object_accessible_parent_class)->initialize(obj, data);
    }

    static void mforms_object_accessible_finalize(GObject *object) {
      mformsObjectAccessiblePrivate *priv = MFORMS_OBJECT_ACCESSIBLE_GET_PRIVATE(object);

      if (priv->mfoacc != nullptr) {
        delete priv->mfoacc;
        priv->mfoacc = 0;
      }

      G_OBJECT_CLASS(mforms_object_accessible_parent_class)->finalize(object);
    }


    static void mforms_object_accessible_widget_set(GtkAccessible *accessible) {
      GtkWidget *widget = gtk_accessible_get_widget(accessible);
      if (widget == NULL)
        return;

      mformsObjectAccessiblePrivate *priv = MFORMS_OBJECT_ACCESSIBLE_GET_PRIVATE(accessible);
      if (priv->mfoacc != nullptr)
        delete priv->mfoacc;
//      if (priv->mfoacc == nullptr)
      priv->mfoacc = new mformsGTKAccessible(accessible, widget);
    }

    static void mforms_object_accessible_widget_unset(GtkAccessible *accessible) {
      GtkWidget *widget = gtk_accessible_get_widget(accessible);
      if (widget == NULL)
        return;

      mformsObjectAccessiblePrivate *priv = MFORMS_OBJECT_ACCESSIBLE_GET_PRIVATE(accessible);
      delete priv->mfoacc;
      priv->mfoacc = nullptr;
    }

    static void mforms_object_accessible_class_init(mformsObjectAccessibleClass *klass) {
      GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
      AtkObjectClass *object_class = ATK_OBJECT_CLASS(klass);

      object_class->ref_state_set = mforms_object_accessible_ref_state_set;
      object_class->initialize = mforms_object_accessible_initialize;
      gobject_class->finalize = mforms_object_accessible_finalize;

      GtkAccessibleClass *accessible_class = GTK_ACCESSIBLE_CLASS(klass);
      accessible_class->widget_set = mforms_object_accessible_widget_set;
      accessible_class->widget_unset = mforms_object_accessible_widget_unset;

      object_class->get_name = mformsGTKAccessible::getName;
      object_class->get_description = mformsGTKAccessible::getDescription;
      object_class->get_role = mformsGTKAccessible::getRole;
      object_class->get_n_children = mformsGTKAccessible::getNChildren;
      object_class->ref_child = mformsGTKAccessible::refChild;

      mforms_object_accessible_parent_class = g_type_class_peek_parent(klass);

      g_type_class_add_private(klass, sizeof (mformsObjectAccessiblePrivate));
    }

//    static void mforms_object_accessible_class_init_child(mformsObjectAccessibleClass *klass) {
////      GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
//      AtkObjectClass *object_class = ATK_OBJECT_CLASS(klass);
//
////      object_class->ref_state_set = mforms_object_accessible_ref_state_set;
////      object_class->initialize = mforms_object_accessible_initialize;
////      gobject_class->finalize = mforms_object_accessible_finalize;
//
////          GtkAccessibleClass *accessible_class = GTK_ACCESSIBLE_CLASS(klass);
////          accessible_class->widget_set = mforms_object_accessible_widget_set;
////          accessible_class->widget_unset = mforms_object_accessible_widget_unset;
//
//      object_class->get_name = mformsGTKAccessible::getName;
//      object_class->get_description = mformsGTKAccessible::getDescription;
//      object_class->get_role = mformsGTKAccessible::getRole;
//      object_class->get_n_children = mformsGTKAccessible::getNChildren;
//      object_class->ref_child = mformsGTKAccessible::refChild;
//
//      mforms_object_accessible_parent_class = g_type_class_peek_parent(klass);
//
//      g_type_class_add_private(klass, sizeof (mformsObjectAccessiblePrivate));
//    }

    // @p parent_type is only required on GTK 3.2 to 3.6, and only on the first call
    GType mforms_object_accessible_get_type(GType parent_type G_GNUC_UNUSED) {
      static volatile gsize type_id_result = 0;

      if (g_once_init_enter(&type_id_result)) {
        GTypeInfo tinfo = {
          0,                              /* class size */
          (GBaseInitFunc) NULL,                   /* base init */
          (GBaseFinalizeFunc) NULL,                 /* base finalize */
          (GClassInitFunc) mforms_object_accessible_class_init,  /* class init */
          (GClassFinalizeFunc) NULL,                  /* class finalize */
          NULL,                           /* class data */
          0,                              /* instance size */
          0,                              /* nb preallocs */
          (GInstanceInitFunc) mforms_object_accessible_init,   /* instance init */
          NULL                            /* value table */
        };

        // good, we have gtk-a11y.h, we can use that
        GType derived_atk_type = GTK_TYPE_CONTAINER_ACCESSIBLE;
        tinfo.class_size = sizeof (GtkContainerAccessibleClass);
        tinfo.instance_size = sizeof (GtkContainerAccessible);

        GType type_id = g_type_register_static(derived_atk_type, "mformsObjectAccessible", &tinfo, (GTypeFlags) 0);
//        g_type_add_interface_static(type_id, ATK_TYPE_OBJECT, &atk_object_info);


        g_once_init_leave(&type_id_result, type_id);
      }

      return type_id_result;
    }

//    static GType mforms_object_accessible_get_type_child(GType parent_type G_GNUC_UNUSED) {
//          static volatile gsize type_id_result = 0;
//
//          if (g_once_init_enter(&type_id_result)) {
//            GTypeInfo tinfo = {
//              0,                              /* class size */
//              (GBaseInitFunc) NULL,                   /* base init */
//              (GBaseFinalizeFunc) NULL,                 /* base finalize */
//              (GClassInitFunc) mforms_object_accessible_class_init_child,  /* class init */
//              (GClassFinalizeFunc) NULL,                  /* class finalize */
//              NULL,                           /* class data */
//              0,                              /* instance size */
//              0,                              /* nb preallocs */
//              (GInstanceInitFunc) mforms_object_accessible_init,   /* instance init */
//              NULL                            /* value table */
//            };
//
//            // good, we have gtk-a11y.h, we can use that
////            GType derived_atk_type = GTK_TYPE_CONTAINER_ACCESSIBLE;
////            tinfo.class_size = sizeof (GtkContainerAccessibleClass);
////            tinfo.instance_size = sizeof (GtkContainerAccessible);
//
////            GType type_id = g_type_register_static(derived_atk_type, "mformsObjectAccessibleChild", &tinfo, (GTypeFlags) 0);
////    //        g_type_add_interface_static(type_id, ATK_TYPE_OBJECT, &atk_object_info);heja
//
//
//
//            g_once_init_leave(&type_id_result, type_id);
//          }
//
//          return type_id_result;
//        }

    mformsGTKAccessible::mformsGTKAccessible(GtkAccessible *accessible, GtkWidget *widget) :
        _accessible(accessible),
        _mfgtk(mformsGTK::FromWidget(widget)) {
    }

    mforms::Accessible* mformsGTKAccessible::getmformsAccessible(AtkObject *accessible) {
      GtkWidget *widget = gtk_accessible_get_widget(GTK_ACCESSIBLE(accessible));
      if (widget == NULL)
        return nullptr;

      MFormsObject *mfo = MFORMSOBJECT(widget);
      if (mfo != nullptr && mfo->pmforms != nullptr)
        return dynamic_cast<mforms::Accessible*>(mfo->pmforms->_owner);

      return nullptr;
    }

    static AtkObject *mforms_object_accessible_new(GType parent_type, GObject *obj) {
      g_return_val_if_fail(MFORMSOBJECT_IS_OBJECT(obj), NULL);

      AtkObject *accessible = (AtkObject *) g_object_new(mforms_object_accessible_get_type(parent_type), "widget", obj,
                                                         NULL);
      atk_object_initialize(accessible, obj);

      return accessible;
    }

    static AtkObject *mforms_object_accessible_new_child(GType parent_type) {
      AtkObject *accessible = (AtkObject *) g_object_new(mforms_object_accessible_get_type(parent_type), "widget", nullptr,
                                                         nullptr);
      atk_object_initialize(accessible, nullptr);

      return accessible;
    }



//    static AtkObject *mforms_object_accessible_new_child(GType parent_type, GObject *obj) {
//      g_return_val_if_fail(MFORMSOBJECT_IS_OBJECT(obj), NULL);
//
//      AtkObject *accessible = (AtkObject *) g_object_new(mforms_object_accessible_get_type_child(parent_type), "widget", obj,
//                                                         NULL);
//      atk_object_initialize(accessible, obj);
//
//      return accessible;
//    }



    AtkObject *mformsGTKAccessible::WidgetGetAccessibleImpl(GtkWidget *widget, AtkObject **cache, gpointer widget_parent_class G_GNUC_UNUSED) {
      if (*cache != nullptr) {
        return *cache;
      }

      *cache = mforms_object_accessible_new(0, G_OBJECT(widget));

      return *cache;
    }

    const gchar* mformsGTKAccessible::getName(AtkObject *accessible) {
      auto mformsGtkAcc = FromAccessible(accessible);
      if (mformsGtkAcc != nullptr) {
        if (mformsGtkAcc->_name.empty())
          mformsGtkAcc->_name = dynamic_cast<mforms::Accessible*>(mformsGtkAcc->_mfgtk->_owner)->get_acc_name();

        if (!mformsGtkAcc->_name.empty())
          return mformsGtkAcc->_name.c_str();
      }

      return ATK_OBJECT_CLASS(mforms_object_accessible_parent_class)->get_name(accessible);
    }

    const gchar* mformsGTKAccessible::getDescription(AtkObject *accessible) {
      auto mformsGtkAcc = FromAccessible(accessible);
      if (mformsGtkAcc != nullptr) {
        if (mformsGtkAcc->_description.empty())
          mformsGtkAcc->_description = dynamic_cast<mforms::Accessible*>(mformsGtkAcc->_mfgtk->_owner)->get_acc_description();

        if (!mformsGtkAcc->_description.empty())
          return mformsGtkAcc->_description.c_str();
      }

      return ATK_OBJECT_CLASS(mforms_object_accessible_parent_class)->get_description(accessible);
    }

    AtkRole mformsGTKAccessible::getRole(AtkObject *accessible) {
      auto acc = getmformsAccessible(accessible);
      if (acc != nullptr && convertAccessibleRole(acc->get_acc_role()) != ATK_ROLE_UNKNOWN)
        return convertAccessibleRole(acc->get_acc_role());

      return ATK_OBJECT_CLASS(mforms_object_accessible_parent_class)->get_role(accessible);
    }

    gint mformsGTKAccessible::getNChildren(AtkObject *accessible) {
      auto acc = getmformsAccessible(accessible);
      if (acc != nullptr)
        return acc->get_acc_child_count();

      return ATK_OBJECT_CLASS(mforms_object_accessible_parent_class)->get_n_children(accessible);
    }

    AtkObject* mformsGTKAccessible::refChild(AtkObject *accessible, gint i) {
      auto acc = getmformsAccessible(accessible);
      if (acc != nullptr) {
           auto accChild = acc->get_acc_child(i);
           if (accChild != nullptr) {
             auto mformsGtkAcc = FromAccessible(accessible);
             auto it = mformsGtkAcc->_childMapping.find(accChild);
             if (it != mformsGtkAcc->_childMapping.end())
               return it->second;

             auto aobject = mforms_object_accessible_new_child(0);
             mformsGtkAcc->_childMapping.insert({accChild, aobject});
             return aobject;
           }
      }

      return ATK_OBJECT_CLASS(mforms_object_accessible_parent_class)->ref_child(accessible, i);
    }

    mformsGTKAccessible* mformsGTKAccessible::FromAccessible(GtkAccessible *accessible) {
      GtkWidget *widget = gtk_accessible_get_widget(accessible);
      if (! widget)
        return 0;

      return MFORMS_OBJECT_ACCESSIBLE_GET_PRIVATE(accessible)->mfoacc;
    }

    mformsGTKAccessible* mformsGTKAccessible::FromAccessible(AtkObject *accessible) {
      return FromAccessible(GTK_ACCESSIBLE(accessible));
    }



    mformsGTK::mformsGTK(_MFormsObject *mfo) : _accessible(nullptr), _owner(nullptr) {
      _mfo = mfo;
      _windowMain = GTK_WIDGET(mfo);
    }

    AtkObject* mformsGTK::GetAccessibleThis(GtkWidget *widget) {
      return mformsGTKAccessible::WidgetGetAccessibleImpl(widget, &_accessible, mforms_object_accessible_parent_class);
    }

    void mformsGTK::SetMFormsOwner(mforms::View *view) {
      _owner = view;
    }

    AtkObject* mformsGTK::GetAccessible(GtkWidget *widget) {
      MFormsObject *mfo = MFORMSOBJECT(widget);
      if (mfo != nullptr && mfo->pmforms != nullptr)
        return mfo->pmforms->GetAccessibleThis(widget);
      return nullptr;
    }

    void mformsGTK::ClassInit(GObjectClass* object_class, GtkWidgetClass *widget_class, GtkEventBoxClass *container_class) {
      widget_class->get_accessible = GetAccessible;
      object_class->finalize = Destroy;
    }

    static GObjectClass *mforms_class_parent_class = nullptr;

    void mformsGTK::Destroy(GObject *object) {
      try {
        MFormsObject *mfo = MFORMSOBJECT(object);

        if (mfo->pmforms == nullptr)
          return;

        mfo->pmforms->Finalise();

        delete mfo->pmforms;
        mfo->pmforms = 0;
        mforms_class_parent_class->finalize(object);
      } catch (...) {
        // no need to log
      }
    }

    mformsGTK *mformsGTK::FromWidget(GtkWidget *widget) {
      MFormsObject *mfo = MFORMSOBJECT(widget);
      return mfo->pmforms;
    }

    void mformsGTK::Finalise() {
      if (_accessible != nullptr) {
        gtk_accessible_set_widget(GTK_ACCESSIBLE(_accessible), nullptr);
        g_object_unref(_accessible);
        _accessible = nullptr;
      }
    }



    void mforms_class_init(MFormsClass *klass) {
      try {
        GObjectClass *gobject_class = (GObjectClass*) klass;
        GtkWidgetClass *widget_class = (GtkWidgetClass*) klass;
        GtkEventBoxClass *container_class = (GtkEventBoxClass*) klass;

        mforms_class_parent_class = G_OBJECT_CLASS(g_type_class_peek_parent(klass));
        mformsGTK::ClassInit(gobject_class, widget_class, container_class);
      } catch (...) {
      }
    }

    void mforms_init(MFormsObject *mf) {
      try {
        gtk_widget_set_can_focus(GTK_WIDGET(mf), TRUE);
        mf->pmforms = new mformsGTK(mf);
      } catch (...) {
      }
    }

    GType mforms_get_type() {
      static GType mforms_type = 0;
      try {

        if (!mforms_type) {
          mforms_type = g_type_from_name("MFormsObject");
          if (!mforms_type) {
            static GTypeInfo mforms_info = {
              (guint16) sizeof (MFormsObjectClass),
              NULL, //(GBaseInitFunc)
              NULL, //(GBaseFinalizeFunc)
              (GClassInitFunc) mforms_class_init,
              NULL, //(GClassFinalizeFunc)
              NULL, //gconstpointer data
              (guint16) sizeof (MFormsObject),
              0, //n_preallocs
              (GInstanceInitFunc) mforms_init,
              NULL //(GTypeValueTable*)
            };
            mforms_type = g_type_register_static(
                        GTK_TYPE_EVENT_BOX, "MFormsObject", &mforms_info, (GTypeFlags) 0);
          }
        }

      } catch (...) {
      }
      return mforms_type;
    }

    GtkWidget* mforms_new() {
      return GTK_WIDGET(g_object_new(mforms_get_type(), NULL));
    }
  }
}
