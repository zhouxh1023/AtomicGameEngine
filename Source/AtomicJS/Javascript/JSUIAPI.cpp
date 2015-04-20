// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include <TurboBadger/tb_window.h>

#include "JSUIAPI.h"
#include "JSVM.h"

#include <Atomic/UI/UISelectItem.h>
#include <Atomic/UI/UIMenuWindow.h>
#include <Atomic/UI/UIButton.h>
#include <Atomic/UI/UIWindow.h>

namespace Atomic
{

static int UIButton_Popup(duk_context* ctx)
{

    if (!duk_is_object(ctx, 0))
    {
        duk_push_string(ctx, "UIButton.popup first argument must be an object");
        duk_throw(ctx);
    }
    if (!duk_is_callable(ctx, 1))
    {
        duk_push_string(ctx, "UIButton.popup second argument must be callable");
        duk_throw(ctx);
    }

    JSVM* vm = JSVM::GetJSVM(ctx);

    duk_enum(ctx, 0, DUK_ENUM_OWN_PROPERTIES_ONLY);

    UISelectItemSource* source = new UISelectItemSource(vm->GetContext());

    while (duk_next(ctx, -1, 0)) {

        String key = duk_get_string(ctx, -1);

        duk_get_prop(ctx, 0);

        if (duk_is_array(ctx, -1))
        {
            // need to support this, for skin image, etc
            assert(0);
        }
        else if (duk_is_string(ctx, -1))
        {
            // id
            String id = duk_get_string(ctx, -1);            
            source->AddItem(new UISelectItem(vm->GetContext(), key, id));
        }
        else
        {
            duk_push_string(ctx, "UIButton.popup data object key is not an array or string");
            duk_throw(ctx);
        }

        duk_pop(ctx);  // pop key value
    }

    duk_pop(ctx);  // pop enum object

    duk_push_this(ctx);

    duk_dup(ctx, 1);
    duk_put_prop_string(ctx, -2, "__popup_menu_callback");

    UIButton* button = js_to_class_instance<UIButton>(ctx, -1, 0);
    UIMenuWindow* menuWindow = new UIMenuWindow(vm->GetContext(), button, "__popup-menu");
    menuWindow->Show(source);
    duk_pop(ctx);

    return 0;
}

int UIWindow_GetResizeToFitContentRect(duk_context* ctx)
{
    duk_push_this(ctx);
    UIWindow* window = js_to_class_instance<UIWindow>(ctx, -1, 0);
    duk_pop(ctx);

    tb::TBWindow* tbwindow = (tb::TBWindow*) window->GetInternalWidget();

    tb::TBRect rect = tbwindow->GetResizeToFitContentRect();

    duk_push_object(ctx);
    duk_push_number(ctx, rect.x);
    duk_put_prop_string(ctx, -2, "x");
    duk_push_number(ctx, rect.y);
    duk_put_prop_string(ctx, -2, "y");
    duk_push_number(ctx, rect.w);
    duk_put_prop_string(ctx, -2, "width");
    duk_push_number(ctx, rect.h);
    duk_put_prop_string(ctx, -2, "height");
    return 1;

}


void jsapi_init_ui(JSVM* vm)
{
    duk_context* ctx = vm->GetJSContext();

    js_class_get_prototype(ctx, "UIButton");
    duk_push_c_function(ctx, UIButton_Popup, 2);
    duk_put_prop_string(ctx, -2, "popup");
    duk_pop(ctx);

    js_class_get_prototype(ctx, "UIWindow");
    duk_push_c_function(ctx, UIWindow_GetResizeToFitContentRect, 0);
    duk_put_prop_string(ctx, -2, "getResizeToFitContentRect");
    duk_pop(ctx);


}

}