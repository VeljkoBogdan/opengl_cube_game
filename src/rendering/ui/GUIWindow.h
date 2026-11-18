#ifndef GUIWINDOW_H
#define GUIWINDOW_H

class GUIWindow {
public:
    GUIWindow(const int width, const int height) : width(width), height(height) {}
    void update(float delta);
    bool wantsMouse();
    bool wantsKeyboard();
private:
    int width = 0;
    int height = 0;

    void showCursor();
    void hideCursor();
};

#endif //GUIWINDOW_H
