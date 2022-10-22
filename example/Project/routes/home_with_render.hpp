#ifndef _Project_home_with_render_route
#define _Project_home_with_render_route

#include <core/core.hpp>

webframe::core::router home_with_render(webframe::core::application& app) {
    init_routes(home)
        .route("/home", [&app]() {
			return app.render("template.html");
        });
    return home;
}
#endif