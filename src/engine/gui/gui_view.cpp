//
// Created by Ion Agorria on 11/05/19
//
#include "engine/core/utils.h"
#include "gui_view.h"
#include <algorithm>

GUIView::GUIView() {

}

GUIView::~GUIView() {
    removeViews();
}

const std::vector<std::unique_ptr<GUIView>>& GUIView::getViews() const {
    return views;
}

void GUIView::addView(std::unique_ptr<GUIView> view) {
    view->moved(this);
    views.emplace_back(std::move(view));
}

std::unique_ptr<GUIView> GUIView::removeView(GUIView* view) {
    std::unique_ptr<GUIView> result;
    if (view) {
        result = Utils::erasePointerFromVector(views, view);
        if (result) {
            result->moved(nullptr);
        }
    }
    return result;
}

void GUIView::removeViews() {
    std::vector<GUIView*> toRemove;
    for (std::unique_ptr<GUIView>& view : views) {
        toRemove.emplace_back(view.get());
    }
    for (GUIView* view : toRemove) {
        removeView(view);
    }
}

void GUIView::moved(GUIView* newParent) {
    this->parent = newParent;
    //Handle root
    if (!newParent) {
        this->root = nullptr;
    } else if (newParent->root) {
        this->root = newParent->root;
    } else {
        this->root = newParent;
    }
    //Update childs
    for (std::unique_ptr<GUIView>& view : views) {
        view->moved(this);
    }
}

void GUIView::show() {
    for (std::unique_ptr<GUIView>& view : views) {
        view->show();
    }
}

void GUIView::hide() {
    for (std::unique_ptr<GUIView>& view : views) {
        view->hide();
    }
}

void GUIView::update() {
    for (std::unique_ptr<GUIView>& view : views) {
        view->update();
    }
}

void GUIView::draw() {
    for (std::unique_ptr<GUIView>& view : views) {
        view->draw();
    }
}

bool GUIView::mouseClick(int x, int y, int button, bool press) {
    bool handled = false;
    for (std::unique_ptr<GUIView>& view : views) {
        if (view->rectangle.isInside(x, y)) {
            handled = view->mouseClick(x, y, button, press);
            if (handled) {
                break;
            }
        }
    }
    return handled;
}

bool GUIView::mouseWheel(int x, int y) {
    bool handled = false;
    for (std::unique_ptr<GUIView>& view : views) {
        if (view->rectangle.isInside(x, y)) {
            handled = view->mouseWheel(x, y);
            if (handled) {
                break;
            }
        }
    }
    return handled;
}

bool GUIView::mouseMove(int x, int y) {
    bool handled = false;
    for (std::unique_ptr<GUIView>& view : views) {
        if (view->rectangle.isInside(x, y)) {
            handled = view->mouseMove(x, y);
            if (handled) {
                break;
            }
        }
    }
    return handled;
}

bool GUIView::keyChange(input_key_t& key) {
    bool handled = false;
    for (std::unique_ptr<GUIView>& view : views) {
        handled = view->keyChange(key);
        if (handled) {
            break;
        }
    }
    return handled;
}