#ifndef FFNG_H
#define FFNG_H

#include "scene/2d/node_2d.h"

class Application;

class ffng : public Node2D {
    GDCLASS(ffng, Node2D);

protected:
    static void _bind_methods();
    //static void _register_methods();
    void _notification(int what);
    void _input(const Ref<InputEvent> event);
    //void _unhandled_input(const Ref<InputEvent> event);

    void _frame_post_draw();
    void size_changed();

    Application* app;

public:
    ffng();
    ~ffng();
};

#endif // FFNG_H