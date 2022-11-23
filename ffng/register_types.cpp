/* register_types.cpp */

#include "register_types.h"

#include "core/class_db.h"
#include "ffng.h"

void register_ffng_types() {
    ClassDB::register_class<ffng>();
}

void unregister_ffng_types() {
   // Nothing to do here in this example.
}