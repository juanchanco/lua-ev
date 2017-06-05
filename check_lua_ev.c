/**
 * Create a table for ev.check that gives access to the constructor for
 * check objects.
 *
 * [-0, +1, ?]
 */
static int luaopen_ev_check(lua_State *L) {
    lua_pop(L, create_check_mt(L));

    lua_createtable(L, 0, 1);

    lua_pushcfunction(L, check_new);
    lua_setfield(L, -2, "new");

    return 1;
}

/**
 * Create the check metatable in the registry.
 *
 * [-0, +1, ?]
 */
static int create_check_mt(lua_State *L) {

    static luaL_Reg fns[] = {
        { "stop",          check_stop },
        { "start",         check_start },
        { "invoke",        check_invoke },
        { NULL, NULL }
    };
    luaL_newmetatable(L, CHECK_MT);
    add_watcher_mt(L);
    luaL_setfuncs(L, fns, 0);

    return 1;
}

/**
 * Create a new check object.  Arguments:
 *   1 - callback function.
 *
 * @see watcher_new()
 *
 * [+1, -0, ?]
 */
static int check_new(lua_State* L) {
    ev_check*  check;
    check = watcher_new(L, sizeof(ev_check), CHECK_MT);
    ev_check_init(check, &check_cb);
    return 1;
}

/**
 * @see watcher_cb()
 *
 * [+0, -0, m]
 */
static void check_cb(struct ev_loop* loop, ev_check* check, int revents) {
    watcher_cb(loop, check, revents);
}

/**
 * Stops the check so it won't be called by the specified event loop.
 *
 * Usage:
 *     check:stop(loop)
 *
 * [+0, -0, e]
 */
static int check_stop(lua_State *L) {
    ev_check*          check     = check_check(L, 1);
    struct ev_loop* loop   = *check_loop_and_init(L, 2);

    loop_stop_watcher(L, 2, 1);
    ev_check_stop(loop, check);

    return 0;
}

/**
 * Starts the check so it won't be called by the specified event loop.
 *
 * Usage:
 *     check:start(loop [, is_daemon])
 *
 * [+0, -0, e]
 */
static int check_start(lua_State *L) {
    ev_check*       check  = check_check(L, 1);
    struct ev_loop* loop   = *check_loop_and_init(L, 2);
    int is_daemon          = lua_toboolean(L, 3);

    ev_check_start(loop, check);
    loop_start_watcher(L, 2, 1, is_daemon);

    return 0;
}

static int check_invoke(lua_State *L) {
    ev_check*       check  = check_check(L, 1);
    struct ev_loop* loop   = *check_loop_and_init(L, 2);
    int revents          = lua_tointeger(L, 3);

    ev_invoke(loop, check, revents);

    return 0;
}

